#include "parser.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

#include <boost/algorithm/string.hpp>

#include <zlib.h>

#include <exception>

namespace pictomir2course {

using namespace std;
using namespace boost::property_tree;
using namespace boost::property_tree::json_parser;
using namespace boost::algorithm;

typedef vector<char> bytes;

game_t parser::parse(istream & stream)
{
    ptree pt;
    game_t game;
    string preprocessed = preprocess_input(stream);
    istringstream buffer(preprocessed);
    try {
        read_json(buffer, pt);
        read_game(pt, game);
    }
    catch (const exception & e) {
        throw string(e.what());
    }
    return game;
}

string parser::fix_corrupted_data(const string &in)
{
    string out = in;
    replace_all(out, "mimetype:", "\"mimetype\":");
    replace_all(out, "compressed:", "\"compressed\":");
    replace_all(out, "encoding:", "\"encoding\":");
    replace_all(out, "data:", "\"data\":");
    return out;
}

string parser::preprocess_input(istream &stream)
{
    streampos start = stream.tellg();
    stream.seekg(0, ios::end);
    streampos end = stream.tellg();
    size_t file_size = end - start;
    stream.seekg(start, ios::beg);
    char bom_test[3];
    static const char * BOM = "\xEF\xBB\xBF";
    stream.read(bom_test, 3u);
    if (strncmp(bom_test, BOM, 3u) == 0) {
        // Matched BOM, keep stream position to prevent its reading
        file_size -= 3u;
    }
    else {
        // Seek stream back
        stream.seekg(start, ios::beg);
    }
    char * buffer = new char[file_size + 1];
    stream.read(buffer, file_size);
    buffer[file_size] = '\0';
    string s0(buffer);
    delete[] buffer;
    string s1 = fix_corrupted_data(s0);
    return s1;
}

void parser::read_game(const ptree &pt, game_t &game)
{
    game.title = pt.get<string>("title");
    game.copyright = pt.get<string>("copyright", string());
    game.license = pt.get<string>("license", string("unspecified"));
    game.homepage = pt.get<string>("homepage", string());

    if (pt.count("authors")) {
        BOOST_FOREACH(const ptree::value_type & v, pt.get_child("authors")) {
                game.authors.push_back(v.second.data());
        }
    }

    BOOST_FOREACH(const ptree::value_type & v, pt.get_child("tasks")) {
        task_t task;
        read_task(v.second, task);
        game.tasks.push_back(task);
    }
}

void parser::read_task(const ptree &pt, task_t &task)
{
    task.title = pt.get<string>("title");
    read_environment(pt.get_child("environment"), task.environment);

    if (pt.count("program")) {
        BOOST_FOREACH(const ptree::value_type & v, pt.get_child("program")) {
            algorithm_t algorithm;
            read_algorithm(v.second, algorithm);
            task.program.push_back(algorithm);
        }
    }

    if (pt.count("hint")) {
        read_hint(pt.get_child("hint"), task.hint);
    }
}

void parser::read_environment(const ptree &pt, environment_t &environment)
{
    read_size2d(pt.get_child("size"), environment.size);
    read_point(pt.get_child("actor.position"), environment.actor.position);

    string direction = pt.get<string>("actor.direction");
    to_lower(direction);
    static const string South = "south";
    static const string East = "east";
    static const string North = "north";
    static const string West = "west";

    if (direction == South)
        environment.actor.direction = SOUTH;
    else if (direction == East)
        environment.actor.direction = EAST;
    else if (direction == North)
        environment.actor.direction = NORTH;
    else if (direction == West)
        environment.actor.direction = WEST;
    else
        throw string("Unknown initial actor direction: ") + direction;

    if (pt.count("painted")) {
        BOOST_FOREACH(const ptree::value_type & v,
                      pt.get_child("painted"))
        {
            point_t point;
            read_point(v.second, point);
            environment.painted.push_back(point);
        }
    }

    if (pt.count("pointed")) {
        BOOST_FOREACH(const ptree::value_type & v,
                      pt.get_child("pointed"))
        {
            point_t point;
            read_point(v.second, point);
            environment.pointed.push_back(point);
        }
    }

    if (pt.count("walls")) {
        BOOST_FOREACH(const ptree::value_type & v,
                      pt.get_child("walls"))
        {
            wall_t wall;
            read_wall(v.second, wall);
            environment.walls.push_back(wall);
        }
    }
}

void parser::read_size2d(const ptree &pt, size2d_t &size)
{
    size.width = pt.get<size_t>("width");
    size.height = pt.get<size_t>("height");
}

void parser::read_point(const ptree &pt, point_t &point)
{
    point.x = pt.get<unsigned int>("x");
    point.y = pt.get<unsigned int>("y");
}

void parser::read_wall(const ptree &pt, wall_t &wall)
{
    read_point(pt.get_child("first"), wall.first);
    read_point(pt.get_child("second"), wall.second);
}

void parser::read_hint(const ptree &pt, hint_t &hint)
{
    string mime_type = pt.get("mimetype", string());
    to_lower(mime_type);
    if (mime_type.length() == 0) {
        hint.hint_type = NO_HINT;
    }
    else {
        to_lower(mime_type);
        if (string("text/plain") == mime_type) {
            hint.hint_type = TEXT_HINT;
            const string plain_text = pt.get<string>("data");
            hint.data.resize(plain_text.length() + 1);
            strncpy(hint.data.data(), plain_text.c_str(), plain_text.length());
            hint.data[plain_text.length()] = '\0';
        }
        else if (string("image/png") == mime_type) {
            read_image(pt, hint);
        }
        else {
            throw string("Unknown Hint MIME type: ") + mime_type;
        }
    }
}

void parser::read_image(const ptree &pt, hint_t &hint)
{
    string encoding = pt.get<string>("encoding");
    to_lower(encoding);
    bytes buffer;
    if (string("notencoded") == encoding) {
        const string plain_text = pt.get<string>("data");
        buffer.resize(plain_text.length() + 1);
        strncpy(buffer.data(), plain_text.c_str(), plain_text.length());
        buffer[plain_text.length()] = '\0';
    }
    else if (string("base64") == encoding) {
        buffer = decode_base64(pt.get<string>("data"));
    }
    else {
        throw string("Unknown data encoding: ") + encoding;
    }
    bool compressed = pt.get("compressed", false);
    if (compressed) {
        buffer = decompress(buffer);
    }
    hint.hint_type = IMAGE_HINT;
    hint.data = buffer;
}

bytes parser::decode_base64(const string &src)
{
    bytes result;

    result.resize(src.length() * 3 / 4);
    for (size_t i=0; i<result.size(); i++)
        result[i] = '\0';

    size_t offset = 0u;
    unsigned int buffer = 0u;
    size_t nbits = 0u;

    for (size_t i=0; i<src.length(); ++i) {
        const char ch = src[i];
        char d = -1;
        if (ch >= 'A' && ch <= 'Z')
            d = ch - 'A';
        else if (ch >= 'a' && ch <= 'z')
            d = ch - 'a' + 26;
        else if (ch >= '0' && ch <= '9')
            d = ch - '0' + 52;
        else if (ch == '+')
            d = 62;
        else if (ch == '/')
            d = 63;

        if (d != -1) {
            buffer = (buffer << 6) | d;
            nbits += 6u;
            if (nbits >= 8u) {
                nbits -= 8u;
                result[offset++] = buffer >> nbits;
                buffer &= (1 << nbits) - 1;
            }
        }
    }
    result.resize(offset);
    return result;
}

bytes parser::decompress(const bytes &src)
{
    if (src.size() <= 4) {
        if (src.size() < 4 || (src[0]!='\0' || src[1]!='\0' || src[1]!='\0' || src[1]!='\0'))
            throw string("Can't decompress bundled data: input data is corrupred");
    }

    size_t expected_size =
            ((unsigned char)src[0] << 24) |
            ((unsigned char)src[1] << 16) |
            ((unsigned char)src[2] << 8 ) |
            ((unsigned char)src[3]      ) ;


    uLongf size;



    size = expected_size > 1ul ? expected_size : 1ul;
    static const size_t Max = 16 * 1024 * 1024;
        // 16Mb is enought for even very large picture

    if (size > Max)
        throw string("Can't decompress bundled data: expected size is too big ot input data header is corrupred");

    const bytes result(size);

    int status = ::uncompress((unsigned char*)result.data(), &size,
                              (unsigned char*)src.data() + 4, src.size() - 4);


    if (status != Z_OK) {
        throw string("Can't decompress bundled data: ZLib error");
    }

    return result;
}

void parser::read_algorithm(const ptree &pt, algorithm_t &algorithm)
{
    size_t block_width = pt.get<size_t>("width");
    size_t block_height = pt.get<size_t>("height");
    size_t program_size = block_width * block_height;
    bool has_condition = pt.get<bool>("condition");
    bool has_repeater = pt.get<bool>("repeater");
    algorithm.commands.resize(program_size);
    size_t index = 0u;
    BOOST_FOREACH(const ptree::value_type & v, pt.get_child("data")) {
        const string & cmd = v.second.data();
        read_command(cmd, algorithm.commands[index]);
        ++index;
    }
    if (has_condition) {
        const string & cmd = pt.get<string>("condition_data");
        read_command(cmd, algorithm.condition);
    }
    else {
        algorithm.condition = NO_COMMAND;
    }
    if (has_repeater) {
        const string & cmd = pt.get<string>("repeater_data");
        read_command(cmd, algorithm.repeater);
    }
    else {
        algorithm.repeater = NO_COMMAND;
    }
}

void parser::read_command(const string & command_name, command_t &command)
{
    const string name = to_lower_copy(command_name);

    static const string CmdNone = string("cmdnone");
    static const string CmdTurnLeft = string("cmdturnleft");
    static const string CmdTurnRight = string("cmdturnright");
    static const string CmdGoForward = string("cmdgoforward");
    static const string CmdDoAction = string("cmddoaction");
    static const string CmdCall1 = string("cmdcall1");
    static const string CmdCall2 = string("cmdcall2");
    static const string CmdCall3 = string("cmdcall3");
    static const string CmdCall4 = string("cmdcall4");
    static const string CmdCall5 = string("cmdcall5");
    static const string CmdCall6 = string("cmdcall6");
    static const string CondWasAction = string("condwasaction");
    static const string CondNotAction = string("condnotaction");
    static const string CondIsWall = string("condiswall");
    static const string CondNotWall = string("condnotwall");
    static const string CondNoWall = string("condnowall");
    static const string RepForever = string("repforever");
    static const string Rep2 = string("rep2");
    static const string Rep3 = string("rep3");
    static const string Rep4 = string("rep4");
    static const string Rep5 = string("rep5");
    static const string Rep6 = string("rep6");

    if (CmdNone == name)
        command = NO_COMMAND;
    else if (CmdTurnLeft == name)
        command = TURN_LEFT;
    else if (CmdTurnRight == name)
        command = TURN_RIGHT;
    else if (CmdDoAction == name)
        command = DO_ACTION;
    else if (CmdGoForward == name)
        command = GO_FORWARD;
    else if (CmdCall1 == name)
        command = CALL_1;
    else if (CmdCall2 == name)
        command = CALL_2;
    else if (CmdCall3 == name)
        command = CALL_3;
    else if (CmdCall4 == name)
        command = CALL_4;
    else if (CmdCall5 == name)
        command = CALL_5;
    else if (CmdCall6 == name)
        command = CALL_6;
    else if (CondWasAction == name)
        command = COND_WAS_ACTION;
    else if (CondNotAction == name)
        command = COND_NOT_ACTION;
    else if (CondIsWall == name)
        command = COND_IS_WALL;
    else if (CondNotWall == name || CondNoWall == name)
        command = COND_NOT_WALL;
    else if (RepForever == name)
        command = REP_FOREVER;
    else if (Rep2 == name)
        command = REP_2;
    else if (Rep3 == name)
        command = REP_3;
    else if (Rep4 == name)
        command = REP_4;
    else if (Rep5 == name)
        command = REP_5;
    else if (Rep6 == name)
        command = REP_6;
    else
        throw string("Unknown command: ") + command_name;
}


}

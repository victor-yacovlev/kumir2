#include "generator.h"

#include <string>
#include <fstream>
#include <vector>

#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

namespace pictomir2course {

using namespace std;
using boost::lexical_cast;
using namespace boost::algorithm;
typedef std::vector<char> bytes;

static const char * BOM = "\xEF\xBB\xBF";

void generator::create_course(const game_t &game,
                              const std::string &filename,
                              const std::string &resources_path)

{
    create_resource_files(game, resources_path);

    ofstream f;
    f.open(filename.c_str());
    if (!f.is_open())
        throw string("Can't create file: ") + filename;

    f.write(BOM, 3u);

    f << "<?xml version='1.0' encoding='UTF-8'?>\n";
    f << "<KURS xml:id=\"0\" ";
    f << "xmlns:xml=\"http://www.w3.org/XML/1998/namespace\"";
    f << "xml:name=\"" << game.title << "\">\n";

    for (size_t index = 0u; index < game.tasks.size(); ++index) {
        const task_t & task = game.tasks[index];
        const string task_data = create_task(task, index, resources_path);
        f.write(task_data.c_str(), task_data.length());
    }

    f << "</KURS>\n";
    f.close();
}

#if defined(WIN32) || defined(_WIN32)
static const string SEP = string("\\");
#else
static const string SEP = string("/");
#endif

void generator::create_resource_files(const game_t &game,
                                      const string &resources_path)
{
    for (size_t index = 0u; index < game.tasks.size(); ++index) {
        const string env_file_name =
                resources_path + SEP + lexical_cast<string>(index) + ".env.json";
        const string kum_file_name =
                resources_path + SEP + lexical_cast<string>(index) + ".kum";
        const string png_file_name =
                resources_path + SEP + lexical_cast<string>(index) + ".png";

        const environment_t & environment = game.tasks[index].environment;
        const hint_t & hint = game.tasks[index].hint;
        const program_t & program = game.tasks[index].program;

        ofstream f_env, f_kum, f_png;

        f_env.open(env_file_name.c_str());
        if (!f_env.is_open())
            throw string("Can't create resource file: ") + env_file_name;
        const string env_data = create_env_data(environment);
        f_env.write(BOM, 3u);
        f_env.write(env_data.c_str(), env_data.length());
        f_env.close();

        f_kum.open(kum_file_name.c_str());
        if (!f_kum.is_open())
            throw string("Can't create resource file: ") + kum_file_name;
        const string kum_data = create_kum_data(program);
        f_kum.write(BOM, 3u);
        f_kum.write(kum_data.c_str(), kum_data.length());
        f_kum.close();

        if (hint.hint_type == IMAGE_HINT) {
            f_png.open(png_file_name.c_str());
            if (!f_png.is_open())
                throw string("Can't create resource file: ") + png_file_name;
            f_png.write(hint.data.data(), hint.data.size());
            f_png.close();
        }
    }
}

string generator::create_env_data(const environment_t &environment)
{
    string result = "{\n";
    result += "\t\"size\": { \"width\": " +
            lexical_cast<string>(environment.size.width) + ", " +
            "\"height\": " + lexical_cast<string>(environment.size.height) +
            " },\n";

    result += "\t\"actor\": { \"position\": { \"x\": " +
            lexical_cast<string>(environment.actor.position.x) + ", \"y\": " +
            lexical_cast<string>(environment.actor.position.y) + " }, " +
            "\"direction\": \"";
    if (environment.actor.direction == SOUTH)
        result += "South";
    else if (environment.actor.direction == NORTH)
        result += "North";
    else if (environment.actor.direction == EAST)
        result += "East";
    else if (environment.actor.direction == WEST)
        result += "West";
    result += "\" }, \n";

    result += "\t\"painted\": [\n";
    for (size_t i=0u; i<environment.painted.size(); ++i) {
        const point_t & p = environment.painted[i];
        result += "\t\t{ \"x\": " + lexical_cast<string>(p.x) +
                ", \"y\": " + lexical_cast<string>(p.y) + " }";
        if (i < environment.painted.size() - 1)
            result += ",";
        result += "\n";
    }
    result += "\t],\n";

    result += "\t\"pointed\": [\n";
    for (size_t i=0u; i<environment.pointed.size(); ++i) {
        const point_t & p = environment.pointed[i];
        result += "\t\t{ \"x\": " + lexical_cast<string>(p.x) +
                ", \"y\": " + lexical_cast<string>(p.y) + " }";
        if (i < environment.pointed.size() - 1)
            result += ",";
        result += "\n";
    }
    result += "\t],\n";

    result += "\t\"walls\": [\n";
    for (size_t i=0u; i<environment.walls.size(); ++i) {
        const wall_t & w = environment.walls[i];
        const point_t & f = w.first;
        const point_t & s = w.second;
        result += "\t\t{ \"first\": { \"x\": " + lexical_cast<string>(f.x) +
                ", \"y\": " + lexical_cast<string>(f.y) + " }, \"second\": { " +
                "\"x\": " + lexical_cast<string>(s.x) + ", \"y\": " +
                lexical_cast<string>(s.y) + " } }";
        if (i < environment.walls.size() - 1)
            result += ",";
        result += "\n";
    }
    result += "\t]\n";

    result += "}\n";
    return result;
}

string generator::create_kum_data(const program_t &program)
{
    string result;
    for (size_t i=0; i<program.size(); ++i) {
        result += create_kum_algorithm(program[i], i);
    }
    return result;
}

string generator::create_kum_algorithm(const algorithm_t &algorithm,
                                       const size_t algorithm_index)
{
    static const char * Commands = " АБВГДЕ";
    const string title = algorithm_index == 0u
            ? string("программа")
            : string("команда ") + Commands[algorithm_index];

    string beg = "нач\n";
    string end = "кон\n";

    if (algorithm.repeater != NO_COMMAND) {
        beg += "нц";
        if (algorithm.repeater == REP_FOREVER)
            beg += "\n";
        else if (algorithm.repeater == REP_2)
            beg += " 2 раз\n";
        else if (algorithm.repeater == REP_3)
            beg += " 3 раз\n";
        else if (algorithm.repeater == REP_4)
            beg += " 4 раз\n";
        else if (algorithm.repeater == REP_5)
            beg += " 5 раз\n";
        else if (algorithm.repeater == REP_6)
            beg += " 6 раз\n";
        end = "кц\n" + end;
    }

    if (algorithm.condition != NO_COMMAND) {
        beg += "если ";
        if (algorithm.condition == COND_IS_WALL) {
            beg += "впереди стена";
        }
        if (algorithm.condition == COND_NOT_WALL) {
            beg += "впереди не стена";
        }
        if (algorithm.condition == COND_WAS_ACTION) {
            beg += "клетка закрашена";
        }
        if (algorithm.condition == COND_NOT_WALL) {
            beg += "клетка не закрашена";
        }
        beg += " то\n";
        end = "все\n" + end;
    }

    string body;
    BOOST_FOREACH(command_t cmd, algorithm.commands) {
        if (TURN_LEFT == cmd)
            body += "повернуть налево\n";
        else if (TURN_RIGHT == cmd)
            body += "повернуть направо\n";
        else if (GO_FORWARD == cmd)
            body += "вперед\n";
        else if (DO_ACTION == cmd)
            body += "закрасить\n";
        else if (CALL_1 == cmd)
            body += "команда А\n";
        else if (CALL_2 == cmd)
            body += "команда Б\n";
        else if (CALL_3 == cmd)
            body += "команда В\n";
        else if (CALL_4 == cmd)
            body += "команда Г\n";
        else if (CALL_5 == cmd)
            body += "команда Д\n";
        else if (CALL_6 == cmd)
            body += "команда Е\n";
    }

    string result = "алг " + title + "\n" + beg + body + end;
    return result;
}

string generator::create_task(const task_t &task,
                              const size_t task_index,
                              const string & resources_path
                              )
{
    const string title = "Задание " + lexical_cast<string>(1 + task_index);
    string result = "    <T xml:id=\"" + lexical_cast<string>(1 + task_index) +
            "\" " +
            "xmlns:xml=\"http://www.w3.org/XML/1998/namespace\"" +
            "xml:name=\""+ title + "\">\n";
    result += "        <TITLE>" + title + "</TITLE>\n";
    result += "        <DESC>\n";
    result += "            <html>\n";
    result += "                <head></head>\n";
    result += "                <body>\n";
    result += "                    <p>" + task.title + "</p>\n";
    if (task.hint.hint_type != NO_HINT) {
        result += string("                    <p>") +
                "<span style=\"text-weight: bold;\">Подсказка: </span>";
        string hint = string(task.hint.data.data(), task.hint.data.size());
        if (task.hint.hint_type == TEXT_HINT) {
            hint = string(task.hint.data.data(), task.hint.data.size());
        }
        else {
            const string img_src =
                    resources_path + SEP + lexical_cast<string>(task_index) + ".png";
            hint = "<br><img src=\"" + img_src + "\">";
        }
        result += hint + "</p>\n";
    }
    result += "                </body>\n";
    result += "            </html>\n";
    result += "        </DESC>\n";
    result += "        <CS>Кумир</CS>\n";
    result += "        <ISP xml:ispname=\"IsometricRobot\" " +
            string("xmlns:xml=\"http://www.w3.org/XML/1998/namespace\">") +
            "\n            <ENV>" +
            resources_path + SEP + lexical_cast<string>(task_index) + ".env.json";
    result += "</ENV>\n        </ISP>\n";
    result +="        <PROGRAM>" +
            resources_path + SEP + lexical_cast<string>(task_index) + ".kum";
    result += "</PROGRAM>\n";
    result += "        <READY>false</READY>\n";
    result += "        <MARK>0</MARK>\n";
    result += "    </T>\n";
    return result;
}

}

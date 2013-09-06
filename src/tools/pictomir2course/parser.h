#ifndef PARSER_H
#define PARSER_H

#include "data.h"

#include <boost/property_tree/ptree.hpp>

#include <istream>
#include <sstream>

namespace pictomir2course {

class parser
{
public:
    /**
     * Parses PictoMir game from arbitary stream
     * @param stream Source stream
     * @throw std::string In case of something goes wrong
     * @return Parsed game
     */
    static game_t parse(std::istream & stream);

private:

    typedef boost::property_tree::ptree ptree;
    typedef std::string string;
    typedef std::vector<char> bytes;

    /**
     * Fixed Pictomir <= 0.15 file format bug, where some JSON keys are
     *   not wrapper in quotes symbols
     * @param Input source data (ASCII or UTF-8)
     * @return Fixed string
     */
    static string fix_corrupted_data(const string & in);


    /**
     * Skips UTF-8 BOM and preprocesses input data
     * @param stream Input stream to read from
     * @return String ready to parse
     */
    static string preprocess_input(std::istream & stream);


    static void read_game(const ptree & pt, game_t & game);


    static void read_task(const ptree & pt, task_t & task);


    static void read_environment(const ptree & pt, environment_t & environment);


    static void read_algorithm(const ptree & pt, algorithm_t & algorithm);


    static void read_command(const string & name, command_t & command);


    static void read_hint(const ptree & pt, hint_t & hint);


    static void read_size2d(const ptree & pt, size2d_t & size);


    static void read_point(const ptree & pt, point_t & point);


    static void read_wall(const ptree & pt, wall_t & wall);


    static void read_image(const ptree & pt, hint_t & hint);


    static bytes decode_base64(const string & src);


    static bytes decompress(const bytes & src);
};

}


#endif

#ifndef OPTIONS_H
#define OPTIONS_H

#include <string>

namespace pictomir2course {

class options {
public:
    static void parse(int argc, char * argv[]);
    static void print_help();

    static std::string input_file_name;
    static std::string output_file_name;
    static bool show_help_and_exit;
    static int exit_code;

private:
    static bool parse_named_parameter(const std::string & prefix,
                                      const std::string & key,
                                      const std::string & value);

    static bool parse_positional_parameter(size_t index,
                                           const std::string & value);

    static void create_default_output_file_name();

    static std::string program_name_;
};

}


#endif

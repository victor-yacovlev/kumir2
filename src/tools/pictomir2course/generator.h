#ifndef GENERATOR_H
#define GENERATOR_H

#include "data.h"

#include <string>
#include <ostream>

namespace pictomir2course {

class generator {
public:
    static void create_course(const game_t & game,
                              const std::string & filename,
                              const std::string & resources_path);

private:

    typedef std::string string;
    typedef std::ostream ostream;
    typedef std::vector<char> bytes;



    static void create_resource_files(const game_t & game,
                                      const std::string & resources_path);



    static string create_env_data(const environment_t & environment);


    static string create_kum_data(const program_t & program);


    static string create_kum_algorithm(const algorithm_t & algorithm,
                                       const size_t algorithm_index);


    static string create_task(const task_t & task,
                              const size_t task_index,
                              const string & resources_path
                              );


    static string create_desc_data(const task_t & task,
                                   const size_t task_index);

};

}


#endif

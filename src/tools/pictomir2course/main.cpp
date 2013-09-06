#include "data.h"
#include "parser.h"
#include "generator.h"

#include <string>
#include <fstream>
#include <iostream>

#if defined(WIN32) || defined(_WIN32)
#   include <Windows.h>
#else
#   include <sys/stat.h>
#endif

namespace pictomir2course {

using namespace std;

static bool make_dir(const string & name)
{
#if defined(WIN32) || defined(_WIN32)
    DWORD dwAttrib = ::GetFileAttributesA(fileName.c_str());
    BOOL exists = (dwAttrib!=INVALID_FILE_ATTRIBUTES &&
             (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
    if (exists)
        return true;
    else
        return ::CreateDirectoryA(name.c_str(), NULL);
#else
    struct ::stat st;
    int res = ::stat(name.c_str(), &st);
    bool exists = res==0 && S_ISDIR(st.st_mode);
    if (exists)
        return true;
    else
        return ::mkdir(name.c_str(), 0755) == 0;
#endif
}

static int process_file(const string & input_filename,
                 const string & output_filename)
{
    int status = 0;
    ifstream stream;
    stream.open(input_filename.c_str(), ios::in);
    if (!stream.is_open()) {
        cerr << "Can't open file: " << input_filename << endl;
    }
    game_t game;
    try {
        game = parser::parse(stream);
    }
    catch (const string & e) {
        cerr << "Can't parse source file: " << e << endl;
        status = 1;
    }
    stream.close();
    if (status != 0) {
        return status;
    }
    if (!make_dir(output_filename + ".resources")) {
        cerr << "Can't create directory for storing course resources: "
             << output_filename + ".resources" << endl;
        return 2;
    }
    try {
        generator::create_course(game,
                                 output_filename,
                                 output_filename + ".resources");
    }
    catch (const string & e) {
        cerr << "Can't create course: " << e << endl;
        status = 3;
    }
    return status;
}

}

int main(int argc, char * argv[])
{
    using namespace std;

    // TODO program options handler

    const string input_filename = string(argv[1]);
    string output_filename = input_filename;
    size_t dot_pos = output_filename.rfind(".pm.json");
    if (dot_pos != string::npos)
        output_filename.resize(dot_pos);
    output_filename += ".kurs.xml";

    pictomir2course::process_file(input_filename, output_filename);

    return 0;
}

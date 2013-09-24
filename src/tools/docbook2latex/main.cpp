#include "options.h"
#include "generator.h"

#include "docbookviewer/docbookfactory.h"

#include <string>
#include <iostream>

#include <QUrl>

namespace docbook2latex {

using namespace std;

static int process_file(const string & input_file_name,
                        const string & output_file_name)
{
    using namespace DocBookViewer;

    const QUrl input_url = QUrl::fromLocalFile(
                QString::fromStdString(input_file_name)
                );
    if (!QFile::exists(input_url.toLocalFile())) {
        cerr << "File not found: " << input_file_name << endl;
        return 1;
    }

    QString parse_error;
    Document document = DocBookFactory::self()->parseDocument(
                input_url,
                &parse_error
                );

    if (parse_error.length() > 0) {
        cerr << "Error parsing " << input_file_name << ": "
                << parse_error.toStdString() << std::endl;
        return 2;
    }

    try {
        generator::self()->reset();
        generator::self()->renderData(document.documentRoot());
        generator::self()->saveToFile(output_file_name);
    }
    catch (const std::string & error) {
        cerr << error << std::endl;
        return 3;
    }

    return 0;
}

}

int main(int argc, char * argv[])
{
    using namespace docbook2latex;

    options::parse(argc, argv);

    if (options::show_help_and_exit || options::exit_code) {
        options::print_help();
        return options::exit_code;
    }

    return process_file(options::input_file_name, options::output_file_name);
}

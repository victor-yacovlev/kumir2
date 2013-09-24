#ifndef DOCBOOK2LATEX_GENERATOR_H
#define DOCBOOK2LATEX_GENERATOR_H

#include "options.h"

#include "docbookviewer/contentrenderer.h"
#include <QString>

namespace docbook2latex {

class generator
        : public DocBookViewer::ContentRenderer
{
    typedef DocBookViewer::ModelPtr ModelPtr;
public:
    static generator * self();

    bool isEmpty() const;
    void reset();
    void renderData(ModelPtr data);
    void saveToFile(const std::string & filename) const;

private:
    explicit generator();

    QString renderBookTitle();

    QString renderElement(ModelPtr data);
    QString renderChapter(ModelPtr data);
    QString renderSection(ModelPtr data);
    QString renderPara(ModelPtr data);
    QString renderEmphasis(ModelPtr data);
    QString normalizeText(QString text) const;
    QString renderOrderedList(ModelPtr data);
    QString renderItemizedList(ModelPtr data);
    QString renderListItem(ModelPtr data);
    QString renderProgramListing(ModelPtr data);
    QString renderCode(ModelPtr data);
    QString renderExample(ModelPtr data);


    ModelPtr root_;
    QString data_;
    bool addSpace_;

};

}

#endif // DOCBOOK2LATEX_GENERATOR_H

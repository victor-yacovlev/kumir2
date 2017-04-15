#ifndef DOCBOOK2LATEX_GENERATOR_H
#define DOCBOOK2LATEX_GENERATOR_H

#include "options.h"

#include <kumir2-libs/docbookviewer/contentrenderer.h>
#include <QString>
#include <QStringList>

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
    QString renderArticleTitle();

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

    QString renderMediaObject(ModelPtr data);
    QString renderExample(ModelPtr data);
    QString renderTable(ModelPtr data);
    QString renderTableRow(ModelPtr data);

    void calculateTableColumnsCount(ModelPtr data);
    static size_t calculateRowColumnsCount(ModelPtr data);

    QString renderSubscript(ModelPtr data);
    QString renderSuperscript(ModelPtr data);

    QString renderFuncSynopsis(ModelPtr data);
    QString renderFuncPrototype(ModelPtr data);
    QString renderFuncDef(ModelPtr data);
    QString renderFunction(ModelPtr data);
    QString renderParamDef(ModelPtr data);
    QString renderParameter(ModelPtr data);
    QString renderType(ModelPtr data);

    QString renderGuiMenu(ModelPtr data);
    QString renderGuiMenuItem(ModelPtr data);
    QString renderGuiButton(ModelPtr data);
    QString renderKeyCombo(ModelPtr data);
    QString renderKeySym(ModelPtr data);


    QString renderMath(ModelPtr data);
    QString renderMRow(ModelPtr data);
    QString renderMN(ModelPtr data);
    QString renderMO(ModelPtr data);
    QString renderMI(ModelPtr data);
    QString renderMSup(ModelPtr data);
    QString renderMSqrt(ModelPtr data);
    QString renderMFrac(ModelPtr data);



    ModelPtr root_;
    QString data_;
    bool addSpace_;
    size_t tableColumns_;
    bool twoColumnLayout_;
    bool allowRasterImages_;
    QStringList sectionFloats_;

};

}

#endif // DOCBOOK2LATEX_GENERATOR_H

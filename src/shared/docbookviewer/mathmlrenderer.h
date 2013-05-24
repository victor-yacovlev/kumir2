#ifndef DOCBOOKVIEWER_MATHMLRENDERER_H
#define DOCBOOKVIEWER_MATHMLRENDERER_H

#include "docbookmodel.h"

#include <QtGlobal>
#include <QImage>
#include <QFont>

namespace DocBookViewer {

class MathMLRenderer
{
public:
    static MathMLRenderer* self();
    const QImage& render(ModelPtr data);

private:

    QImage renderBlock(ModelPtr block);
    QImage renderElement(ModelPtr element);
    QImage renderPlainText(ModelPtr element);
    QImage renderIdentifier(ModelPtr element);
    QImage renderNumber(ModelPtr element);
    QImage renderOperator(ModelPtr element);
    QImage renderText(ModelPtr element);


    QImage renderSqrt(ModelPtr element);
    QImage renderFrac(ModelPtr element);
    QImage renderSup(ModelPtr element);

    static QFont mathFont(qreal fontSize);
    static QFont regularFont(qreal fontSize);

    MathMLRenderer();

    QFont font_;
};

} // namespace DocBookViewer

#endif // DOCBOOKVIEWER_MATHMLRENDERER_H

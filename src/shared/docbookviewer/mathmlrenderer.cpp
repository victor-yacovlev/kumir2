#include "mathmlrenderer.h"

#include <QtCore>
#include <QtGui>

namespace DocBookViewer {

static const qreal BASE_FONT_SIZE = 14.0; /* pt */
static const qreal MIN_FONT_SIZE = 9.0; /* pt */
static const int GRAPHICS_SPACING = 1; /* px */
static const int REGULAR_LINE_WIDTH  = 2; /* px */
static const int FRAC_LINE_WIDTH  = 1; /* px */

MathMLRenderer* MathMLRenderer::self()
{
    static MathMLRenderer * instance = new MathMLRenderer();
    return instance;
}

const QImage& MathMLRenderer::render(ModelPtr data)
{
    if (data->cachedImage_.isNull()) {
        font_ = mathFont(BASE_FONT_SIZE);
        data->cachedImage_ = renderBlock(data);
    }
    return data->cachedImage_;
}

QImage MathMLRenderer::renderBlock(ModelPtr block)
{
    QList<QImage> blockImages;
    foreach (ModelPtr child, block->children()) {
        QImage blockImage = renderElement(child);
        if (!blockImage.isNull()) {
            blockImages.append(blockImage);
        }
    }
    int height = 0;
    int width = 0;
    foreach (const QImage & blockImage, blockImages) {
        height = qMax(height, blockImage.height());
        width += blockImage.width();
    }
    QImage result;
    if (height > 0 && width > 0) {
        result = QImage(width, height, QImage::Format_ARGB32);
        result.fill(0);
        QPainter painter(&result);
        int x = 0;
        foreach (const QImage & blockImage, blockImages) {
            int y = (height - blockImage.height()) / 2;
            painter.drawImage(x, y, blockImage);
            x += blockImage.width();
        }
        painter.end();
    }
    return result;
}

QImage MathMLRenderer::renderElement(ModelPtr element)
{
    if (element == DocBookModel::Text) {
        return renderPlainText(element);
    }
    if (element == DocBookModel::MathML_MI) {
        return renderIdentifier(element);
    }
    if (element == DocBookModel::MathML_MN) {
        return renderNumber(element);
    }
    if (element == DocBookModel::MathML_MO) {
        return renderOperator(element);
    }
    if (element == DocBookModel::MathML_MText) {
        return renderText(element);
    }
    else if (element == DocBookModel::MathML_MSqrt) {
        return renderSqrt(element);
    }
    else if (element == DocBookModel::MathML_MFrac) {
        return renderFrac(element);
    }
    else if (element == DocBookModel::MathML_MSup) {
        return renderSup(element);
    }
    else {
        return QImage();
    }
}

QImage MathMLRenderer::renderIdentifier(ModelPtr element)
{
    static const QStringList StandardNames = QStringList()
            << "cos" << "sin" << "tan" << "ctan" << "tg" << "ctg"
            << "arccos" << "acos" << "arcsin" << "asin" << "atan"
            << "arctan" << "actan" << "arcctan" << "arctg" << "atg"
            << "arcctg" << "actg" << "ln" << "lg" << "log";


    ModelPtr textElement;
    foreach (ModelPtr child, element->children()) {
        if (child == DocBookModel::Text) {
            textElement = child;
            break;
        }
    }
    if (textElement && StandardNames.contains(textElement->text().trimmed())) {
        font_ = regularFont(font_.pointSizeF());
    }
    else {
        font_ = mathFont(font_.pointSizeF());
    }
    return renderBlock(element);
}

QImage MathMLRenderer::renderNumber(ModelPtr element)
{
    font_ = regularFont(font_.pointSizeF());
    return renderBlock(element);
}

QImage MathMLRenderer::renderText(ModelPtr element)
{
    font_ = regularFont(font_.pointSizeF());
    return renderBlock(element);
}

QImage MathMLRenderer::renderOperator(ModelPtr element)
{
    ModelPtr textElement;
    foreach (ModelPtr child, element->children()) {
        if (child == DocBookModel::Text) {
            textElement = child;
            break;
        }
    }
    QImage result;
    if (textElement) {
        QString op = textElement->text().trimmed();
        if (op == "-")
            op = QChar(0x2212);
        if (element->parent()) {
            int index = element->parent()->children().indexOf(element);
            if (index > 0) {
                op = " " + op;
            }
            if (index < element->parent()->children().size()-1) {
                op = op + " ";
            }
        }
        QFont font = regularFont(font_.pointSizeF());
        QFontMetrics fm(font);
        result = QImage(fm.width(op), fm.lineSpacing(), QImage::Format_ARGB32);
        result.fill(0);
        QPainter painter(&result);
        painter.setFont(font);
        painter.drawText(0, result.height() - fm.descent() - fm.leading(), op);
        painter.end();
    }
    return result;
}

QImage MathMLRenderer::renderPlainText(ModelPtr element)
{
    const QString plainText = element->text().simplified();
    QFontMetrics fm(font_);


    QImage image(fm.width(plainText) + 2, fm.lineSpacing(), QImage::Format_ARGB32);
    image.fill(0);
    QPainter painter(&image);
    painter.setFont(font_);
    painter.drawText(1, image.height() - fm.descent() - fm.leading(), plainText);
    painter.end();
    return image;
}

QImage MathMLRenderer::renderSqrt(ModelPtr element)
{
    QImage inner = renderBlock(element);
    QFontMetrics fm(font_);
    int sqrtSymbolWidth = fm.width('m');
    int sqrtSymbolHeight = fm.lineSpacing() / 2;
    QImage result(inner.width() + 2 * GRAPHICS_SPACING + sqrtSymbolWidth,
                  inner.height() + GRAPHICS_SPACING,
                  QImage::Format_ARGB32);
    result.fill(0);
    QPainter painter(&result);
    QPen pen;
    pen.setColor(Qt::black);
    pen.setWidth(REGULAR_LINE_WIDTH);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawLine(0, result.height() - sqrtSymbolHeight,
                     sqrtSymbolWidth / 2, result.height());
    painter.drawLine(sqrtSymbolWidth / 2, result.height(),
                     sqrtSymbolWidth, REGULAR_LINE_WIDTH + 1);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.drawLine(sqrtSymbolWidth, REGULAR_LINE_WIDTH,
                     result.width(), REGULAR_LINE_WIDTH
                     );
    painter.drawImage(sqrtSymbolWidth + GRAPHICS_SPACING,
                      GRAPHICS_SPACING,
                      inner);
    painter.end();
    return result;
}

QImage MathMLRenderer::renderFrac(ModelPtr element)
{
    QImage result;
    ModelPtr first, second;
    if (element->children().size() > 1) {
        first = element->children()[0];
        second = element->children()[1];
        QImage firstImage = renderBlock(first);
        QImage secondImage = renderBlock(second);
        int width = qMax(firstImage.width(), secondImage.width());
        int height = firstImage.height() + secondImage.height() +
                2 * GRAPHICS_SPACING + FRAC_LINE_WIDTH;
        result = QImage(width, height, QImage::Format_ARGB32);
        result.fill(0);
        QPainter painter(&result);
        painter.drawImage((result.width()-firstImage.width())/2, 0, firstImage);
        painter.drawImage((result.width()-secondImage.width())/2,
                          firstImage.height() + 2 * GRAPHICS_SPACING +
                          FRAC_LINE_WIDTH, secondImage);
        QPen pen;
        pen.setColor(Qt::black);
        pen.setWidth(FRAC_LINE_WIDTH);
        painter.setPen(pen);
        painter.drawLine(0,
                         firstImage.height() + 2 * GRAPHICS_SPACING,
                         result.width(),
                         firstImage.height() + 2 * GRAPHICS_SPACING
                         );
        painter.end();
    }
    return result;
}

QImage MathMLRenderer::renderSup(ModelPtr element)
{
    qreal prevFontSize = font_.pointSizeF();
    qreal fontSize = qMax(font_.pointSizeF() * 0.66, MIN_FONT_SIZE);
    qreal baseHeight = QFontMetricsF(font_).lineSpacing();
    int supBaseLine = int(baseHeight * 0.66);
    font_.setPointSizeF(fontSize);
    QImage content = renderBlock(element);
    QImage result;
    if (!content.isNull()) {
        int width = content.width();
        int height = content.height() + supBaseLine;
        result = QImage(width, height, QImage::Format_ARGB32);
        result.fill(0);
        QPainter painter(&result);
        painter.drawImage(0, 0, content);
        painter.end();
    }
    font_.setPointSizeF(prevFontSize);
    return result;
}

QFont MathMLRenderer::mathFont(qreal fontSize)
{
    QFont font("serif");
    font.setPointSizeF(fontSize);
    font.setItalic(true);
    return font;
}

QFont MathMLRenderer::regularFont(qreal fontSize)
{
    QFont font("serif");
    font.setPointSizeF(fontSize);
    return font;
}


MathMLRenderer::MathMLRenderer()
{
}

} // namespace DocBookViewer

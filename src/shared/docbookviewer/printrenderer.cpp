#include "printrenderer.h"

#include <QFontDatabase>
#include <QFontMetricsF>
#include <QPainter>

namespace DocBookViewer {

PrintRenderer::PrintRenderer()
    : ContentRenderer()
{
    const QFontDatabase fontDatabase;
    baseFont_ = fontDatabase.font("serif", "regular", 12);
}

bool PrintRenderer::isEmpty() const
{
    return pagesCount() == 0;
}

int PrintRenderer::pagesCount() const
{
    return pages_.size();
}

PrintRenderer* PrintRenderer::self()
{
    static PrintRenderer* instance = new PrintRenderer();
    return instance;
}

void PrintRenderer::setBaseFont(QFont font)
{
    font.setBold(false);
    font.setItalic(false);
    baseFont_ = font;
}

void PrintRenderer::reset()
{
    currentRoot_.clear();
    pages_.clear();
}

void PrintRenderer::renderData(ModelPtr data)
{
    currentRoot_ = data;
    Cursor cursor = Cursor(data);
    while (cursor) {
        consumeData(cursor);
    }
}

PrintRenderer::Cursor& PrintRenderer::consumeData(Cursor &cursor)
{
    if (cursor) {
        if (cursor.item->modelType() == DocBookModel::Book ||
                cursor.item->modelType() == DocBookModel::Article) {
            // Make title page
            Frame titleFrame = newFrame(FullWidthTextFrame);
            QString titleText = cursor.item->title();
            QFont titleFont(baseFont_);
            titleFont.setBold(true);
            titleFont.setCapitalization(QFont::Capitalize);
            addTextToFrame(titleFrame, titleFont, titleText);
            finalizeFrame(titleFrame);
        }
    }
    return cursor;
}

PrintRenderer::Frame& PrintRenderer::newFrame(const FrameType type)
{
    Page& page = pages_.isEmpty() ? newPage() : pages_.last();
    page.append(Frame());
    Frame& frame = page.last();
    if (type == FullWidthTextFrame ||
            type == MainTextFrame ||
            type == FloatBlockFrame
            )
    {
        const qreal requiredWidth = type == FullWidthTextFrame
                ? pageSize_.width() : columnWidth();
        for (frame.rect=freeContentArea(page);
             frame.rect.height() < lineHeight() ||
             frame.rect.width() < requiredWidth;
             page=newPage()
             )
        {
        }
    }
    return frame;
}

QRectF PrintRenderer::freeContentArea(const Page &page) const
{
    QRectF result(QPointF(0, 0), pageSize_);
    foreach (const Frame & frame, page) {
        if (frame.type == HeaderTextFrame) {
            result.setTop(frame.rect.bottom() + lineHeight());
        }
        else if (frame.type == FooterTextFrame) {
            result.setBottom(frame.rect.top() - lineHeight());
        }
    }
    foreach (const Frame & frame, page) {
        if (frame.type == FullWidthTextFrame) {
            result.setTop(qMax(
                              result.top(),
                              frame.rect.bottom() + lineHeight()
                              ));
        }
    }
    if (!isTwoColumnLayout()) {
        foreach (const Frame & frame, page) {
            if (frame.type == MainTextFrame || frame.type == FloatBlockFrame) {
                result.setTop(qMax(
                                  result.top(),
                                  frame.rect.bottom() + lineHeight()
                                  ));
            }
        }
    }
    else {
        QRectF firstColumn(result), secondColumn(result);
        firstColumn.setWidth(columnWidth());
        secondColumn.setWidth(columnWidth());
        secondColumn.moveRight(secondColumnLeftPos());
        foreach (const Frame & frame, page) {
            if (frame.type == MainTextFrame || frame.type == FloatBlockFrame) {
                bool inRightColumn = frame.rect.left() > columnWidth();
                QRectF & rectToAjuct = inRightColumn
                        ? secondColumn : firstColumn;
                rectToAjuct.setTop(qMax(
                                       rectToAjuct.top(),
                                       frame.rect.bottom() + lineHeight()
                                       ));
            }
        }
        result = firstColumn.height() >= lineHeight()
                ? firstColumn : secondColumn;
    }
    return result;
}

bool PrintRenderer::isTwoColumnLayout() const
{
    ModelPtr parent = currentRoot_;
    while (parent) {
        if (parent->modelType() == DocBookModel::Article) {
            return true;
        }
        parent = parent->parent();
    }
    return false;
}

qreal PrintRenderer::columnWidth() const
{
    static const qreal ColumnSpacing = 5.0;
    if (isTwoColumnLayout()) {
        return (pageSize_.width() - ColumnSpacing) / 2.0;
    }
    else {
        return pageSize_.width();
    }
}

qreal PrintRenderer::secondColumnLeftPos() const
{
    return pageSize_.width() - columnWidth();
}

qreal PrintRenderer::lineHeight() const
{
    qreal fontSize = baseFont_.pointSizeF();
    return pointsToMillimeters(fontSize * 1.5);
}

qreal PrintRenderer::pointsToMillimeters(const qreal points)
{
    const qreal inches = points / 72.0;
    const qreal millimeters = inches * 25.4;
    return millimeters;
}

void PrintRenderer::finalizeFrame(Frame &frame) const
{
    QRectF actualRect;
    foreach (const FrameItem & item, frame.items) {
        QSizeF itemSize;
        if (item.type == TextFrameItem) {
            itemSize.setHeight(lineHeight());
            itemSize.setWidth(textWidth(item.text, item.font));
        }
        else if (item.type == ImageFrameItem) {
            itemSize = item.image.size();
        }
        else {
            // TODO tables
        }
        const QRectF itemRect(item.position, itemSize);
        actualRect = actualRect.intersected(itemRect);
    }
    frame.rect.setHeight(actualRect.height());
    if (frame.type == FullWidthTextFrame) {
        frame.rect.setWidth(actualRect.width());
    }
}

PrintRenderer::Page& PrintRenderer::newPage()
{
    pages_.append(Page());
    return pages_.last();
}

QString& PrintRenderer::addTextToFrame(
        Frame &frame,
        const QFont &font,
        QString &text) const
{
    return text;
}

qreal PrintRenderer::textWidth(const QString &text, const QFont &font)
{
    const QFontMetricsF fm(font);
    const qreal fontPointSize = font.pointSizeF();
    const qreal fontPixelSize = fm.height();
    const qreal metricsDotsPerPoint = fontPixelSize / fontPointSize;
    const qreal textPixelWidth = fm.width(text);
    const qreal textPointWidth = textPixelWidth / metricsDotsPerPoint;
    return pointsToMillimeters(textPointWidth);
}

void PrintRenderer::render(QPrinter *printer) const
{
    if (!printer)
        return;
    QPainter painter(printer);
    foreach (const Page& page, pages_) {
        renderPage(painter, page);
    }
}

void PrintRenderer::renderPage(QPainter &painter, const Page &page) const
{
    foreach (const Frame & frame, page) {
        renderFrame(painter, frame);
    }
}

void PrintRenderer::renderFrame(QPainter &painter, const Frame &frame) const
{
    painter.save();
    painter.translate(frame.rect.topLeft());
    foreach (const FrameItem & item, frame.items) {
        if (item.type == TextFrameItem) {
            renderTextFrameItem(painter, item);
        }
    }
    painter.restore();
}

void PrintRenderer::renderTextFrameItem(QPainter &painter,
                                        const FrameItem &item) const
{
    painter.save();
    painter.setFont(item.font);
    painter.drawText(item.position, item.text);
    painter.restore();
}

} // namespace DocBookViewer

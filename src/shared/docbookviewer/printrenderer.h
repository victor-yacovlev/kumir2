#ifndef DOCBOOKVIEWER_PRINTRENDERER_H
#define DOCBOOKVIEWER_PRINTRENDERER_H

#include "contentrenderer.h"
#include "docbookmodel.h"

#include <QtCore>
#include <QPrinter>
#include <QPoint>
#include <QPointF>
#include <QFont>


namespace DocBookViewer {

class PrintRenderer
        : public ContentRenderer
{
public:
    static PrintRenderer* self();
    bool isEmpty() const;
    void reset();
    void renderData(ModelPtr data);
    void setBaseFont(QFont font);
    int pagesCount() const;
    void render(QPrinter * printer) const;


private:

    enum FrameType {
        MainTextFrame,
        FloatBlockFrame,
        FullWidthTextFrame,
        HeaderTextFrame,
        FooterTextFrame
    };

    enum FrameItemType {
        TextFrameItem,
        ImageFrameItem,
        TableFrameItem
    };

    struct FrameItem {
        FrameItemType type;
        QString text;
        QPointF position;
        QFont font;
        QImage image;
    };

    struct Frame {
        QRectF rect;
        FrameType type;
        QList<FrameItem> items;
    };

    typedef QList<Frame> Page;

    struct Cursor {
        ModelPtr item;
        size_t textPosition;

        inline explicit Cursor(ModelPtr data)
            : item(data), textPosition(0) {}
        operator bool() const { return item; }
    };

    explicit PrintRenderer();
    void finalizeFrame(Frame & frame) const;
    Frame& newFrame(const FrameType type);
    Page& newPage();
    Cursor& fillFrameWithData(Frame & frame, Cursor & cursor) const;
    Cursor& consumeData(Cursor & cursor);
    QString& addTextToFrame(Frame& frame,
                            const QFont & font,
                            QString & text) const;
    QRectF freeContentArea(const Page & page) const;
    qreal lineHeight() const;
    static qreal textWidth(const QString & text, const QFont & font);
    static qreal pointsToMillimeters(const qreal points);
    qreal secondColumnLeftPos() const;
    qreal columnWidth() const;

    bool isTwoColumnLayout() const;

    void renderPage(QPainter &painter, const Page & page) const;
    void renderFrame(QPainter &painter, const Frame & frame) const;
    void renderTextFrameItem(QPainter &painter,
                             const FrameItem & item) const;
    void renderImageFrameItem(QPainter &painter,
                              const FrameItem & item) const;

    QFont baseFont_;
    ModelPtr currentRoot_;
    QSizeF pageSize_;

    QList<Page> pages_;

};

} // namespace DocBookViewer

#endif // DOCBOOKVIEWER_PRINTRENDERER_H

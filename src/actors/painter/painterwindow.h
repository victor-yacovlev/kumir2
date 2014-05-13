#ifndef PAINTERWINDOW_H
#define PAINTERWINDOW_H

#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif



namespace ActorPainter {

class PainterModule;

namespace Ui {
    class PainterWindow;
}

class PainterWindow : public QWidget
{
    Q_OBJECT

public:
    explicit PainterWindow(PainterModule * module, QWidget *parent = 0);
    void setCanvas(QImage * canvas, QMutex * locker);
    inline QSize minimumSizeHint() const { return QSize(360, 300); }
    QWidget * view();
    ~PainterWindow();
public slots:
    void newImage();
    void loadImage();
    void saveImageAs();
private slots:
    void handleColorTextModeChanged();
    void handleCursorMoved(int x, int y, const QColor &color);
    void saveImageToFile(const QString &fileName);
    void handleFullScreen();
    void handleScale();
    void reset();
    void changeZoom(int factor);
    void setZoom(qreal scale);

private:

    QString e_showColorMode; // values: "HTML", "RGB", "CMYK", "HSL", "HSV"
    Ui::PainterWindow *ui;
    QString s_fileName;
    QString s_templateName;
    PainterModule * m_module;
    class PainterNewImageDialog * m_newImageDialog;

};

}

#endif // PAINTERWINDOW_H

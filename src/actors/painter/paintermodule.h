/*
This file is generated, but you can safely change it
until you run "gen_actor_source.py" with "--project" flag.

Generated file is just a skeleton for module contents. 
You should change it corresponding to functionality.
*/

#ifndef PAINTERMODULE_H
#define PAINTERMODULE_H

#include <QtCore>
#include <QtGui>
#include "extensionsystem/kplugin.h"
#include "paintermodulebase.h"

namespace ActorPainter {

class PainterWindow;
class PainterNewImageDialog;

class PainterModule
    : public PainterModuleBase
{
    friend class PainterWindow;
    friend class PainterNewImageDialog;
    Q_OBJECT
public:
    // Constructor
    PainterModule(ExtensionSystem::KPlugin * parent);
public slots:
    // Reset actor state before program starts
    void reset();
    // Set animation flag
    void setAnimationEnabled(bool enabled);
    // Actor methods
    QString runCMYK(const int c, const int m, const int y, const int k);
    QString runCMYKA(const int c, const int m, const int y, const int k, const int a);
    QString runHSL(const int h, const int s, const int l);
    QString runHSLA(const int h, const int s, const int l, const int a);
    QString runHSV(const int h, const int s, const int v);
    QString runHSVA(const int h, const int s, const int v, const int a);
    QString runRGB(const int r, const int g, const int b);
    QString runRGBA(const int r, const int g, const int b, const int a);
    void runMoveTo(const int x, const int y);
    int runPageHeight();
    void runLoadPage(const QString& fileName);
    void runFill(const int x, const int y);
    void runSetBrush(const QString& color);
    void runLine(const int x0, const int y0, const int x, const int y);
    void runLineTo(const int x, const int y);
    void runPolygon(const int N, const QVector< int >& XX, const QVector< int >& YY);
    void runWrite(const int x, const int y, const QString& text);
    void runNewPage(const int width, const int height, const QString& backgroundColor);
    void runCircle(const int x, const int y, const int r);
    void runSetPen(const int width, const QString& color);
    void runPoint(const int x, const int y, const QString& color);
    void runDensity(const int value);
    void runRectangle(const int x0, const int y0, const int x, const int y);
    void runSplitColorToCMYK(const QString& color, int& c, int& m, int& y, int& k);
    void runSplitColorToHSL(const QString& color, int& h, int& s, int& l);
    void runSplitColorToHSV(const QString& color, int& h, int& s, int& v);
    void runSplitColorToRGB(const QString& color, int& r, int& g, int& b);
    void runSavePage(const QString& fileName);
    void runUnsetBrush();
    QString runPointSample(const int x, const int y);
    int runCenterX();
    int runCenterY();
    int runPageWidth();
    int runTextWidth(const QString& text);
    void runSetFont(const QString& family, const int size, const bool bold, const bool italic);
    void runEllipse(const int x, const int y, const int r1, const int r2);

public:
    // GUI access methods
    QWidget* mainWidget() const;

private:
    void drawPolygon(const QVector<QPoint> & points);
    class PainterWindow * m_window;
    QImage * canvas;
    QImage * originalCanvas;
    QMutex * canvasLock;
    QPoint point;
    QFont font;
    QBrush brush;
    QPen pen;
    bool transparent;
    QWidget * view;
    Qt::BrushStyle style;


}; // PainterModule
    
} // ActorPainter

#endif // PAINTERMODULE_H

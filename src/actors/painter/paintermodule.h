/*
This file is generated, but you can safely change it
until you run "gen_actor_source.py" with "--project" flag.

Generated file is just a skeleton for module contents. 
You should change it corresponding to functionality.
*/

#ifndef PAINTERMODULE_H
#define PAINTERMODULE_H

#include <QtCore>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include "extensionsystem/kplugin.h"
#include "paintermodulebase.h"

namespace ActorPainter {

class PainterWindow;
class PainterView;
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
    static QList<ExtensionSystem::CommandLineParameter> acceptableCommandLineParameters();
    inline void reloadSettings(ExtensionSystem::SettingsPtr, const QStringList & ) {}
    inline void changeGlobalState(ExtensionSystem::GlobalState, ExtensionSystem::GlobalState) {}
public slots:
    // Reset actor state before program starts
    void reset();
    inline void terminateEvaluation() {}
    // Set animation flag
    void setAnimationEnabled(bool enabled);
    // Actor methods
    Color runCMYK(const int c, const int m, const int y, const int k);
    Color runCMYKA(const int c, const int m, const int y, const int k, const int a);
    Color runHSL(const int h, const int s, const int l);
    Color runHSLA(const int h, const int s, const int l, const int a);
    Color runHSV(const int h, const int s, const int v);
    Color runHSVA(const int h, const int s, const int v, const int a);
    Color runRGB(const int r, const int g, const int b);
    Color runRGBA(const int r, const int g, const int b, const int a);
    void runMoveTo(const int x, const int y);
    int runPageHeight();
    void runLoadPage(const QString& fileName);
    void runFill(const int x, const int y);
    void runSetBrush(const Color& color);
    void runLine(const int x0, const int y0, const int x, const int y);
    void runLineTo(const int x, const int y);
    void runPolygon(const int N, const QVector< int >& XX, const QVector< int >& YY);
    void runWrite(const int x, const int y, const QString& text);
    void runNewPage(const int width, const int height, const Color& backgroundColor);
    void runCircle(const int x, const int y, const int r);
    void runSetPen(const int width, const Color& color);
    void runPoint(const int x, const int y, const Color& color);
    void runDensity(const int value);
    void runRectangle(const int x0, const int y0, const int x, const int y);
    void runSplitToCMYK(const Color& color, int& c, int& m, int& y, int& k);
    void runSplitToHSL(const Color& color, int& h, int& s, int& l);
    void runSplitToHSV(const Color& color, int& h, int& s, int& v);
    void runSplitToRGB(const Color& color, int& r, int& g, int& b);
    void runSavePage(const QString& fileName);
    void runUnsetBrush();
    Color runPointSample(const int x, const int y);
    int runCenterX();
    int runCenterY();
    int runPageWidth();
    int runTextWidth(const QString& text);
    void runSetFont(const QString& family, const int size, const bool bold, const bool italic);
    void runEllipse(const int x, const int y, const int r1, const int r2);

public:
    // GUI access methods
    QWidget* mainWidget() const;
    inline QWidget* pultWidget() const { return nullptr; }
    QString initialize(const QStringList &configurationParameters, const ExtensionSystem::CommandLine &runtimeParameters);

private:    
    void createGui();
    void timerEvent(QTimerEvent *);
    void markViewDirty();
    void drawPolygon(const QVector<QPoint> & points);
    PainterWindow * m_window;
    QScopedPointer<QImage> canvas;
    QScopedPointer<QImage> originalCanvas;
    QMutex * canvasLock;
    QPoint point;
    QFont font;
    QBrush brush;
    QPen pen;
    bool transparent;
    PainterView * view;
    Qt::BrushStyle style;
    bool dirty_;
    QMutex* dirtyLock_;


}; // PainterModule
    
} // ActorPainter

#endif // PAINTERMODULE_H

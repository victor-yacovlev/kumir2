#ifndef DEFAULT_START_PAGE_H
#define DEFAULT_START_PAGE_H

#include "interfaces/startpage_widget_interface.h"

#include <QtCore>
#if QT_VERSION >= 0x050000
#else
#   include <QtGui>
#endif
#   include <QtWidgets>


namespace CoreGUI {

namespace Ui {
class DefaultStartPage;
}

class Plugin;
class MainWindow;

class DefaultStartPage
        : public QWidget
        , public Shared::StartpageWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(Shared::StartpageWidgetInterface)
public:
    explicit DefaultStartPage(Plugin* plugin, MainWindow * mw, QWidget * parent = 0);
    QString startPageTabStyle() const;

protected /*methods*/:
    void createConnections();
    void reloadStyleSheet();
    void relayoutBlocks(const int width);

    void updateContents();
    void updateRecentFiles();
    void updateRecentCourses();
    void updateHelp();

    QString makeShortText(const QString &s, const int maxWidth, const QFontMetrics & fm);

    bool eventFilter(QObject *obj, QEvent *evt);

protected Q_SLOTS:
    void handleShowCoursesPressed();
    void handleShowHelpPressed();
    void handleRecentFileClicked();
    void handleRecentCourseClicked();
    void handleHelpItemClicked();

protected /*fields*/:
    Plugin* _plugin;
    MainWindow* _mainWindow;

    Ui::DefaultStartPage * ui;


    // StartpageWidgetInterface interface
public:
    inline QWidget *startPageWidget() { return this; }
    QString startPageTitle() const;
    inline void setStartPageTitleChangeHandler(const QObject *, const char *) {}
};



}

#endif

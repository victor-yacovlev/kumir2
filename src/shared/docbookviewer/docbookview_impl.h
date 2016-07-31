#ifndef DOCBOOKVIEW_IMPL_H
#define DOCBOOKVIEW_IMPL_H

#include "docbookview.h"
#include "document.h"

#include "extensionsystem/settings.h"

#include <QObject>
#include <QMap>
#include <QSharedPointer>
#include <QTreeWidget>
#include <QSplitter>
#include <QLabel>
#include <QStackedWidget>

namespace DocBookViewer {

class DocBookModel;
class SidePanel;
class ContentView;

class DocBookViewImpl
        : public QObject
{
    friend class DocBookView;
    Q_OBJECT
public /* methods */:
    explicit DocBookViewImpl(class DocBookView * pClass);
    QAction * viewerAction(const DocBookView::DocBookViewAction type) const;
    class Document addDocument(const QUrl & url, QString * error);

    class Document addDocuments(const QString & groupName, const QList<QUrl> & urls, QString * error = 0);

    void removeDocument(const Document & existingDocument);
    bool hasAlgorithm(const QString & name) const;
    void navigateToApiFunction(const QString & package, const QString & function);
    void navigateToIndex(const QString &index);
    void clearNavigationFilters();

    void updateSettings(ExtensionSystem::SettingsPtr settings, const QString & prefix);
    void saveState(ExtensionSystem::SettingsPtr settings, const QString & prefix);
    void restoreState(ExtensionSystem::SettingsPtr settings, const QString & prefix);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    bool isSideBarVisible() const;

    void createActions();

    void setRole(ModelType category, const QString & value);
    QString role(ModelType category) const;

signals:
    void itemSelected(ModelPtr model);

public slots:
    void showAnItem(ModelPtr model);
    void showPrintDialog();
    void hideSidePanel();
    void showSidePanel();
    void setInitialView();
    void setSize(const QSize & size);

    void switchToCompactMode(bool force=false);
    void switchToEnoughtSizeMode(bool force=false);

    QStringList booksList() const;
    void activateBookIndex(int index = -1);

private slots:
    void updateToggleSideBarButton();
    void handleShowSideBarButton(const QString & action);


private /* fields */:
    DocBookView* pClass_;
    ExtensionSystem::SettingsPtr settings_;
    QString settingsPrefix_;

    QSplitter * splitter_;
    SidePanel * sidePanel_;
    ContentView * content_;

    QAction * actionToggleNavigationBar_;
    QAction * actionShowPrintDialog_;

    QLabel * toggleSideBar_;
    QWidget * enoughtSizeContainer_;
    QWidget * smallSizeContainer_;
    QWidget * filler_;
    QWidget * background_;
    QStackedWidget * splitterRightWidget_;

    QMap<ModelType,QString> roleValues_;

    bool compactModeFlag_;


};

}

#endif

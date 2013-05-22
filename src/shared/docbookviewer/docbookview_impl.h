#ifndef DOCBOOKVIEW_IMPL_H
#define DOCBOOKVIEW_IMPL_H

#include "docbookview.h"
#include "document.h"

#include <QObject>
#include <QMap>
#include <QSharedPointer>
#include <QTreeWidget>
#include <QSplitter>

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
    class Document addDocument(
            const QUrl & url,
            QString * error,
            int index
            );

    void removeDocument(const Document & existingDocument);

    void updateSettings(QSettings * settings, const QString & prefix);
    void saveState(QSettings * settings, const QString & prefix);
    void restoreState(QSettings * settings, const QString & prefix);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    void createActions();

signals:
    void itemSelected(ModelPtr model);

public slots:
    void showAnItem(ModelPtr model);
    void showPrintDialog();

private /* fields */:
    DocBookView* pClass_;
    QSettings* settings_;
    QString settingsPrefix_;

    QSplitter * splitter_;
    SidePanel * sidePanel_;
    ContentView * content_;

    QAction* actionToggleNavigationBar_;
    QAction* actionShowPrintDialog_;


};

}

#endif

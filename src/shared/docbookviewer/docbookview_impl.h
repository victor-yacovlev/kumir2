#ifndef DOCBOOKVIEW_IMPL_H
#define DOCBOOKVIEW_IMPL_H

#include "docbookview.h"
#include "document.h"

#include <QObject>
#include <QMap>
#include <QSharedPointer>

class QSplitter;
class QTreeWidget;
class QTreeWidgetItem;
class QAction;

namespace DocBookViewer {

class DocBookModel;

class DocBookViewImpl
        : public QObject
{
    Q_OBJECT
public:
    explicit DocBookViewImpl(class DocBookView * pClass);
    QAction * viewerAction(const DocBookView::DocBookViewAction type) const;
    class Document addDocument(
            const QUrl & url,
            QString * error,
            int index
            );

    void removeDocument(const Document & existingDocument);
    void updateSettings(QSettings * settings, const QString & prefix);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    DocBookView* pClass_;
    QSettings* settings_;
    QString settingsPrefix_;

    /* Child widgets */
    QSplitter * splitter_;
    QTreeWidget * navigator_;
    class ContentView * content_;
    QList<Document> loadedDocuments_;
    QAction* actionToggleNavigationBar_;
    QAction* actionShowPrintDialog_;

    void createActions();


    void createNavigationItems(QTreeWidgetItem * item,
                               ModelPtr model);


    QMap<QTreeWidgetItem*, ModelPtr> modelsOfItems_;
    QMap<ModelPtr, QTreeWidgetItem*> itemsOfModels_;

public slots:
    void selectAnItem(QTreeWidgetItem * item);
    void loadAModelByPtr(quintptr dataPtr);
    void showPrintDialog();

};

}

#endif

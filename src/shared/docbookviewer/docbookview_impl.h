#ifndef DOCBOOKVIEW_IMPL_H
#define DOCBOOKVIEW_IMPL_H

#include "docbookview.h"
#include "document.h"

#include <QObject>
#include <QMap>

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

    DocBookView* pClass_;

    /* Child widgets */
    QSplitter * splitter_;
    QTreeWidget * navigator_;
    class ContentView * content_;
    QList<Document> loadedDocuments_;
    QAction* actionToggleNavigationBar_;
    QAction* actionShowPrintDialog_;

    void createActions();


    void createNavigationItems(QTreeWidgetItem * item,
                               const DocBookModel * model);


    QMap<QTreeWidgetItem*, const DocBookModel*> modelsOfItems_;
    QMap<const DocBookModel*, QTreeWidgetItem*> itemsOfModels_;

public slots:
    void selectAnItem(QTreeWidgetItem * item);
    void loadAModelByPtr(quintptr dataPtr);
    void showPrintDialog();

};

}

#endif

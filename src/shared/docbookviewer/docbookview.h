#ifndef DOCBOOKVIEW_H
#define DOCBOOKVIEW_H

// Self includes
#include "document.h"

// Qt includes
#include <QString>
#include <QWidget>
#include <QUrl>

#ifdef DOCBOOKVIEWER_LIBRARY
#define DOCBOOKVIEWER_EXPORT Q_DECL_EXPORT
#else
#define DOCBOOKVIEWER_EXPORT Q_DECL_IMPORT
#endif

namespace DocBookViewer {

class DOCBOOKVIEWER_EXPORT DocBookView
        : public QWidget
{
    Q_OBJECT
public:
    enum DocBookViewAction {
        NoAction,
        ToggleNavigationPane,
        ShowPrintDialog
    };

    explicit DocBookView(QWidget * parent = 0);
    QAction * viewerAction(const DocBookViewAction type) const;

    ~DocBookView();

    Document addDocument(
            const QUrl & url,
            QString * error = 0,
            int index = -1
            );

    void removeDocument(const Document & existingDocument);

private:    
    class DocBookViewImpl* pImpl_;
};

}

#endif

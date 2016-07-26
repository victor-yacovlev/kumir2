#ifndef DOCBOOKVIEW_H
#define DOCBOOKVIEW_H

// Self includes
#include "document.h"
#include "modeltype.h"

// Extension system includes
#include "extensionsystem/settings.h"

// Qt includes
#include <QString>
#include <QWidget>
#include <QUrl>
#include <QSettings>

#ifdef LIBRARY_BUILD
#define EXTERN Q_DECL_EXPORT
#else
#define EXTERN Q_DECL_IMPORT
#endif

namespace DocBookViewer {

class EXTERN DocBookView
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

    QStringList booksList() const;
    void activateBookIndex(int index);

    void updateSettings(ExtensionSystem::SettingsPtr settings, const QString & prefix);
    bool hasAlgorithm(const QString & name) const;
    void navigateToApiFunction(const QString & package, const QString & function);
    void navigateFromQuickReference(const int topicType, const QString &name);
    void clearNavigationFilters();
    void setInitialView();

    void setRole(ModelType category, const QString & value);
    QString role(ModelType category) const;

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    ~DocBookView();

    Document addDocument(const QUrl & url, QString * error = 0);
    Document addDocuments(const QString & groupName, const QList<QUrl> & urls, QString * error = 0);

    void removeDocument(const Document & existingDocument);

protected:
    void showEvent(QShowEvent *);
    void closeEvent(QCloseEvent *);
    void hideEvent(QHideEvent *);
    void resizeEvent(QResizeEvent *);

private:    
    class DocBookViewImpl* pImpl_;
};

}

#endif

#ifndef DOCBOOKVIEWER_SIDEPANEL_H
#define DOCBOOKVIEWER_SIDEPANEL_H

#include "document.h"

#include "extensionsystem/settings.h"

#include <QWidget>
#include <QTreeWidget>
#include <QSettings>
#include <QSet>

namespace DocBookViewer {

namespace Ui {
class SidePanel;
}

class SidePanel : public QWidget
{
    Q_OBJECT
    
public:
    explicit SidePanel(QWidget *parent = 0);

    void addDocument(Document document, bool bookSetItemsAsTopLevel);
    QList<ModelPtr> loadedDocuments() const;
    void saveState(ExtensionSystem::SettingsPtr  settings, const QString & prefix);
    void restoreState(ExtensionSystem::SettingsPtr  settings, const QString & prefix);

    /** Find function in any package by name */
    ModelPtr findApiFunction(const QString & name) const;

    /** Find function in specified package by name */
    ModelPtr findApiFunction(const QString & package, const QString & function) const;


    ~SidePanel();

public slots:
    void selectItem(ModelPtr model);
    void selectItem(ModelPtr model, const QString & searchText);
    void clearNavigationFilters();

signals:
    void itemPicked(ModelPtr model);
    
private:
    void createNavigationItems(QTreeWidgetItem * item, ModelPtr model);
    void createListOfExamples(ModelPtr root);
    void createListOfTables(ModelPtr root);
    void createListOfAlgorithms(ModelPtr root);

    static QSet<QTreeWidgetItem*> findFilteredItems(
            const QString & text,
            QTreeWidget * tree,
            QTreeWidgetItem * root
            );

    typedef QPair<QString,QString> FunctionName; // <Package,Function>

    Ui::SidePanel *ui;
    QString settingsPrefix_;
    QMap<QTreeWidgetItem*, ModelPtr> modelsOfItems_;
    QMap<ModelPtr, QTreeWidgetItem*> itemsOfModels_;
    QMap<FunctionName, ModelPtr> functionsIndex_;
    QList<Document> loadedDocuments_;
    QList<ModelPtr> topLevelItems_;

private slots:
    void hadleButtonPressed();
    void selectTreeWidgetItem(QTreeWidgetItem * item);
    void doFilter(const QString & text);


};


} // namespace DocBookViewer
#endif // DOCBOOKVIEWER_SIDEPANEL_H

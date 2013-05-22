#ifndef DOCBOOKVIEWER_SIDEPANEL_H
#define DOCBOOKVIEWER_SIDEPANEL_H

#include "document.h"

#include <QWidget>
#include <QTreeWidget>
#include <QSettings>

namespace DocBookViewer {

namespace Ui {
class SidePanel;
}

class SidePanel : public QWidget
{
    Q_OBJECT
    
public:
    explicit SidePanel(QWidget *parent = 0);

    void addDocument(Document document);    
    void saveState(QSettings * settings, const QString & prefix);
    void restoreState(QSettings * settings, const QString & prefix);

    ~SidePanel();

public slots:
    void selectItem(ModelPtr model);

signals:
    void itemPicked(ModelPtr model);
    
private:
    void createNavigationItems(QTreeWidgetItem * item, ModelPtr model);
    void createListOfExamples(ModelPtr root);
    void createListOfTables(ModelPtr root);



    Ui::SidePanel *ui;
    QString settingsPrefix_;
    QMap<QTreeWidgetItem*, ModelPtr> modelsOfItems_;
    QMap<ModelPtr, QTreeWidgetItem*> itemsOfModels_;
    QList<Document> loadedDocuments_;

private slots:
    void hadleButtonPressed();
    void selectTreeWidgetItem(QTreeWidgetItem * item);

};


} // namespace DocBookViewer
#endif // DOCBOOKVIEWER_SIDEPANEL_H

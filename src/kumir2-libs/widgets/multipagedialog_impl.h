#ifndef WIDGETS_MULTIPAGEDIALOG_IMPL_H
#define WIDGETS_MULTIPAGEDIALOG_IMPL_H

#include <QObject>
#include <QStackedWidget>
#include <QTreeWidget>
#include <QScrollArea>
#include <QDialogButtonBox>

namespace Widgets {

class MultiPageDialogImpl : public QObject
{
    friend class MultiPageDialog;
    Q_OBJECT
private /*methods*/:
    explicit MultiPageDialogImpl(class MultiPageDialog *parent);
    void addPage(const QString & groupTitle, QWidget * page);
    void setupUi();
    void updateListWidth();
    
private slots:
    void handleGroupSelected(QTreeWidgetItem * current, QTreeWidgetItem * previous);
    void acceptAllPages();
    void resetAllPages();
    void init();

private /*fields*/:
    class MultiPageDialog * pClass_;
    QList<QWidget*> pages_;
    QStackedWidget * stack_;
    QTreeWidget * list_;
    QDialogButtonBox * buttonBox_;
    
};

} // namespace Widgets

#endif // WIDGETS_MULTIPAGEDIALOG_IMPL_H

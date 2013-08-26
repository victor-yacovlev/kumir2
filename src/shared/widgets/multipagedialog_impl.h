#ifndef WIDGETS_MULTIPAGEDIALOG_IMPL_H
#define WIDGETS_MULTIPAGEDIALOG_IMPL_H

#include <QObject>
#include <QStackedWidget>
#include <QListWidget>
#include <QScrollArea>

namespace Widgets {

class MultiPageDialogImpl : public QObject
{
    friend class MultiPageDialog;
    Q_OBJECT
private /*methods*/:
    explicit MultiPageDialogImpl(class MultiPageDialog *parent);
    void addPage(QWidget * page);
    void setupUi();
    void updateListWidth();
    
private slots:
    void handleGroupSelected(int index);
    void acceptAllPages();
    void init();

private /*fields*/:
    class MultiPageDialog * pClass_;
    QList<QWidget*> pages_;
    QStackedWidget * stack_;
    QListWidget * list_;
    
};

} // namespace Widgets

#endif // WIDGETS_MULTIPAGEDIALOG_IMPL_H

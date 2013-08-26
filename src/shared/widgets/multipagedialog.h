#ifndef MULTIPAGEDIALOG_H
#define MULTIPAGEDIALOG_H

#include <QtGlobal>
#include <QDialog>

#ifdef WIDGETS_LIBRARY
#define WIDGETS_EXPORT Q_DECL_EXPORT
#else
#define WIDGETS_EXPORT Q_DECL_IMPORT
#endif

namespace Widgets {

class WIDGETS_EXPORT MultiPageDialog
        : public QDialog
{
    Q_OBJECT

public:
    explicit MultiPageDialog(QWidget *parent = 0);
    void addPage(QWidget* page);

public slots:
    int exec();
    void accept();

private:
    class MultiPageDialogImpl * pImpl_;
};


}
#endif

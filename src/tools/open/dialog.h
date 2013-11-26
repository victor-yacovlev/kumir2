#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "appmanager.h"

namespace Ui {
class OpenDialog;
}

class OpenDialog
        : public QDialog
{
    Q_OBJECT
public:
    explicit OpenDialog(const QList<Application> & apps);
    quintptr selectedApplicationId() const;
private:
    Ui::OpenDialog * ui_;
};


#endif

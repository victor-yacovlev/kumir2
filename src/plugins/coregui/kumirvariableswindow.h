#ifndef COREGUI_KUMIRVARIABLESWINDOW_H
#define COREGUI_KUMIRVARIABLESWINDOW_H

#include <QtCore>
#include <QtGui>

#include "ui_kumirvariableswindow.h"

namespace CoreGUI {

class KumirVariablesWindow
        : public QWidget
        , Ui::KumirVariablesWindow

{
public:
    KumirVariablesWindow();
};

} // namespace CoreGUI

#endif // COREGUI_KUMIRVARIABLESWINDOW_H

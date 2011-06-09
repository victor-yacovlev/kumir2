#ifndef BROWSER_COMPONENT_H
#define BROWSER_COMPONENT_H

#include <QWidget>

namespace Browser {

namespace Ui {
    class Component;
}

class Component : public QWidget
{
    Q_OBJECT

public:
    explicit Component(QWidget *parent = 0);
    ~Component();

private:
    Ui::Component *ui;
};


} // namespace Browser
#endif // BROWSER_COMPONENT_H

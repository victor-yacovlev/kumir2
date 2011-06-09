#include "component.h"
#include "ui_component.h"

namespace Browser {

Component::Component(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Component)
{
    ui->setupUi(this);
}

Component::~Component()
{
    delete ui;
}

} // namespace Browser

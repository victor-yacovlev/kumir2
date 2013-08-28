#include "declarativesettingspage_impl.h"

namespace Widgets {

DeclarativeSettingsPageImpl::DeclarativeSettingsPageImpl(DeclarativeSettingsPage * parent)
    : QObject(parent)
    , pClass_(parent)
{
}

void DeclarativeSettingsPageImpl::init()
{
    if (!settings_) return;
    foreach (const QString & key, entries_.keys()) {
        const DeclarativeSettingsPage::Entry entry = entries_[key];
        if (entry.type==DeclarativeSettingsPage::Integer) {
            int value = settings_->value(key, entry.defaultValue).toInt();
            QSpinBox * control = 0;
            if (widgets_.contains(key) && QString(widgets_[key]->metaObject()->className())=="QSpinBox")
                control = qobject_cast<QSpinBox*>(widgets_[key]);
            if (control)
                control->setValue(value);
        }
        else if (entry.type==DeclarativeSettingsPage::Color) {
            QColor value = settings_->value(key, entry.defaultValue).toString();
            QToolButton * control = 0;
            if (widgets_.contains(key) && QString(widgets_[key]->metaObject()->className())=="QToolButton")
                control = qobject_cast<QToolButton*>(widgets_[key]);
            if (control)
                setButtonColor(control, value);
        }
        else {
            qFatal("Not implemented");
        }
    }
}

void DeclarativeSettingsPageImpl::resetToDefaults()
{
    foreach (const QString & key, entries_.keys()) {
        const DeclarativeSettingsPage::Entry entry = entries_[key];
        if (entry.type==DeclarativeSettingsPage::Integer) {
            int value = entry.defaultValue.toInt();
            QSpinBox * control = 0;
            if (widgets_.contains(key) && QString(widgets_[key]->metaObject()->className())=="QSpinBox")
                control = qobject_cast<QSpinBox*>(widgets_[key]);
            if (control)
                control->setValue(value);
        }
        else if (entry.type==DeclarativeSettingsPage::Color) {
            QColor value = entry.defaultValue.toString();
            QToolButton * control = 0;
            if (widgets_.contains(key) && QString(widgets_[key]->metaObject()->className())=="QToolButton")
                control = qobject_cast<QToolButton*>(widgets_[key]);
            if (control)
                setButtonColor(control, value);
        }
        else {
            qFatal("Not implemented");
        }
    }
}

void DeclarativeSettingsPageImpl::accept()
{
    if (!settings_) return;
    foreach (const QString & key, entries_.keys()) {
        const DeclarativeSettingsPage::Entry entry = entries_[key];
        if (entry.type==DeclarativeSettingsPage::Integer) {
            QSpinBox * control = 0;
            if (widgets_.contains(key) && QString(widgets_[key]->metaObject()->className())=="QSpinBox")
                control = qobject_cast<QSpinBox*>(widgets_[key]);
            if (control)
                settings_->setValue(key, control->value());
        }
        else if (entry.type==DeclarativeSettingsPage::Color) {
            QToolButton * control = 0;
            if (widgets_.contains(key) && QString(widgets_[key]->metaObject()->className())=="QToolButton")
                control = qobject_cast<QToolButton*>(widgets_[key]);
            if (control)
                settings_->setValue(key, buttonColor(control).name());
        }
        else {
            qFatal("Not implemented");
        }
    }
    emit pClass_->settingsChanged();
}

void DeclarativeSettingsPageImpl::addIntegerField(const QString &key, const DeclarativeSettingsPage::Entry &entry)
{
    QSpinBox * control = new QSpinBox(pClass_);
    if (entry.defaultValue.isValid())
        control->setValue(entry.defaultValue.toInt());
    if (entry.minimumValue.isValid())
        control->setMinimum(entry.minimumValue.toInt());
    if (entry.maximumValue.isValid())
        control->setMaximum(entry.maximumValue.toInt());
    widgets_[key] = control;
    addField(entry.title, control);
}

void DeclarativeSettingsPageImpl::addColorField(const QString &key, const DeclarativeSettingsPage::Entry &entry)
{
    QToolButton * control = new QToolButton(pClass_);
    QColor defColor(Qt::black);
    if (entry.defaultValue.isValid()) {
        defColor = QColor(entry.defaultValue.toString());
    }
    setButtonColor(control, defColor);
    connect(control, SIGNAL(clicked()), this, SLOT(showColorDialog()));
    widgets_[key] = control;
    addField(entry.title, control);
}

void DeclarativeSettingsPageImpl::addField(const QString &labelText, QWidget *controlWidget)
{
    QWidget * c = new QWidget(pClass_);
    QHBoxLayout * cl = new QHBoxLayout(c);
    c->setLayout(cl);
    QLabel * lbl = new QLabel(labelText+":");
    cl->addWidget(lbl);
    cl->addStretch();
    cl->addWidget(controlWidget);
    pClass_->layout()->addWidget(c);
}

void DeclarativeSettingsPageImpl::showColorDialog()
{
    QToolButton * b = qobject_cast<QToolButton*>(sender());
    if (!b)
        return;
    QColor c = QColorDialog::getColor(
                buttonColor(b),
                pClass_,
                tr("Choose color")
                );
    if (c.isValid()) {
        setButtonColor(b, c);
    }
}

void DeclarativeSettingsPageImpl::setButtonColor(QToolButton *btn, const QColor &color)
{
    QString style = QString("QToolButton { border: 1px solid ") + color.darker().name() + QString("; ") +
            QString("              border-radius: 4px; ")+
            QString("              background-color: ") + color.name() +
            QString("            }")+
            QString("QToolButton::pressed { background-color: ") + color.darker(50).name() + QString(" }");

    btn->setStyleSheet(style);
}

QColor DeclarativeSettingsPageImpl::buttonColor(const QToolButton *b)
{
    QString style = b->styleSheet();
    QRegExp rxBgColor("background-color:\\s*(\\S+)");
    if ( rxBgColor.indexIn(style) != -1 ) {
        QString color = rxBgColor.cap(1);
        return QColor(color);
    }
    else {
        return QColor(Qt::black);
    }
}


} // namespace Widgets

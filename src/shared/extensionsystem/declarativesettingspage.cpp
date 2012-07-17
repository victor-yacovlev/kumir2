#include "declarativesettingspage.h"

namespace ExtensionSystem {

DeclarativeSettingsPage::DeclarativeSettingsPage(
        const QString &pluginName,
        const QString &title,
        QSettings *settings,
        const QMap<QString, Entry> entries)
{
    setMaximumWidth(400);
    s_pluginName = pluginName;
    setWindowTitle(title);
    m_entries = entries;
    QVBoxLayout * layout = new QVBoxLayout();
    setLayout(layout);
    foreach (const QString &key, entries.keys()) {
        const Entry entry = entries[key];
        if (entry.type==Integer) {
            addIntegerField(key, entry);
        }
        else if (entry.type==Color) {
            addColorField(key, entry);
        }
        else {
            qFatal("Not implemented");
        }
    }
    layout->addStretch();
    setSettingsObject(settings);
}

void DeclarativeSettingsPage::setSettingsObject(QSettings *settings)
{
    m_settings = settings;
    const QString className = QString::fromAscii(settings->metaObject()->className());
    Q_ASSERT(className=="QSettings");
    init();
}

void DeclarativeSettingsPage::init()
{
    if (!m_settings) return;
    foreach (const QString & key, m_entries.keys()) {
        const Entry entry = m_entries[key];
        if (entry.type==Integer) {
            int value = m_settings->value(key, entry.defaultValue).toInt();
            QSpinBox * control = 0;
            if (m_widgets.contains(key) && QString(m_widgets[key]->metaObject()->className())=="QSpinBox")
                control = qobject_cast<QSpinBox*>(m_widgets[key]);
            if (control)
                control->setValue(value);
        }
        else if (entry.type==Color) {
            QColor value = m_settings->value(key, entry.defaultValue).toString();
            QToolButton * control = 0;
            if (m_widgets.contains(key) && QString(m_widgets[key]->metaObject()->className())=="QToolButton")
                control = qobject_cast<QToolButton*>(m_widgets[key]);
            if (control)
                setButtonColor(control, value);
        }
        else {
            qFatal("Not implemented");
        }
    }
}

void DeclarativeSettingsPage::accept()
{
    if (!m_settings) return;
    foreach (const QString & key, m_entries.keys()) {
        const Entry entry = m_entries[key];
        if (entry.type==Integer) {
            QSpinBox * control = 0;
            if (m_widgets.contains(key) && QString(m_widgets[key]->metaObject()->className())=="QSpinBox")
                control = qobject_cast<QSpinBox*>(m_widgets[key]);
            if (control)
                m_settings->setValue(key, control->value());
        }
        else if (entry.type==Color) {
            QToolButton * control = 0;
            if (m_widgets.contains(key) && QString(m_widgets[key]->metaObject()->className())=="QToolButton")
                control = qobject_cast<QToolButton*>(m_widgets[key]);
            if (control)
                m_settings->setValue(key, buttonColor(control).name());
        }
        else {
            qFatal("Not implemented");
        }
    }
    emit settingsChanged();
}

void DeclarativeSettingsPage::addIntegerField(const QString &key, const Entry &entry)
{
    QSpinBox * control = new QSpinBox(this);
    if (entry.defaultValue.isValid())
        control->setValue(entry.defaultValue.toInt());
    if (entry.minimumValue.isValid())
        control->setMinimum(entry.minimumValue.toInt());
    if (entry.maximumValue.isValid())
        control->setMaximum(entry.maximumValue.toInt());
    m_widgets[key] = control;
    addField(entry.title, control);
}

void DeclarativeSettingsPage::addColorField(const QString &key, const Entry &entry)
{
    QToolButton * control = new QToolButton(this);
    QColor defColor(Qt::black);
    if (entry.defaultValue.isValid()) {
        defColor = QColor(entry.defaultValue.toString());
    }
    setButtonColor(control, defColor);
    connect(control, SIGNAL(clicked()), this, SLOT(showColorDialog()));
    m_widgets[key] = control;
    addField(entry.title, control);
}

void DeclarativeSettingsPage::addField(const QString &labelText, QWidget *controlWidget)
{
    QWidget * c = new QWidget(this);
    QHBoxLayout * cl = new QHBoxLayout(c);
    c->setLayout(cl);
    QLabel * lbl = new QLabel(labelText+":");
    cl->addWidget(lbl);
    cl->addStretch();
    cl->addWidget(controlWidget);
    this->layout()->addWidget(c);
}

void DeclarativeSettingsPage::showColorDialog()
{
    QToolButton * b = qobject_cast<QToolButton*>(sender());
    if (!b)
        return;
    QColor c = QColorDialog::getColor(
                buttonColor(b),
                this,
                tr("Choose color")
                );
    if (c.isValid()) {
        setButtonColor(b, c);
    }
}

void DeclarativeSettingsPage::setButtonColor(QToolButton *btn, const QColor &color)
{
    QString style = QString("QToolButton { border: 1px solid ") + color.darker().name() + QString("; ") +
            QString("              border-radius: 4px; ")+
            QString("              background-color: ") + color.name() +
            QString("            }")+
            QString("QToolButton::pressed { background-color: ") + color.darker(50).name() + QString(" }");

    btn->setStyleSheet(style);
}

QColor DeclarativeSettingsPage::buttonColor(const QToolButton *b)
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

}

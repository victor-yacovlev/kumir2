#include "guisettingspage.h"
#include "ui_guisettingspage.h"

#include "mainwindow.h"
#include "plugin.h"
#include "toolbarcontextmenu.h"
#include "extensionsystem/pluginmanager.h"
#include "kumirprogram.h"
#include "interfaces/actorinterface.h"

#include "widgets/iconprovider.h"

namespace CoreGUI {

const QString GUISettingsPage::LayoutKey = "MainWindowLayout";
const QString GUISettingsPage::RowsFirstValue = "RowsFirst";
const QString GUISettingsPage::ColumnsFirstValue = "ColumnsFirst";

GUISettingsPage::GUISettingsPage(ExtensionSystem::SettingsPtr settings, QWidget *parent)
    : settings_(settings)
    , QWidget(parent)
    , ui(new Ui::GUISettingsPage)
{
    ui->setupUi(this);
    ui->lblRowsFirst->setPixmap(QPixmap(":/coregui/layout-rows-first.png"));
    ui->lblColumnsFirst->setPixmap(QPixmap(":/coregui/layout-columns-first.png"));
    ui->gridLayout_2->setAlignment(ui->lblColumnsFirst, Qt::AlignHCenter|Qt::AlignBottom);
    ui->gridLayout_2->setAlignment(ui->lblRowsFirst, Qt::AlignHCenter|Qt::AlignBottom);
    ui->gridLayout_2->setAlignment(ui->btnColumnsFirst, Qt::AlignHCenter|Qt::AlignTop);
    ui->gridLayout_2->setAlignment(ui->btnRowsFirst, Qt::AlignHCenter|Qt::AlignTop);
    createVisibleIconsGrid();
    init();
}

void GUISettingsPage::createVisibleIconsGrid()
{
//    static const QString qtcreatorIconsPath = ExtensionSystem::PluginManager::instance()->sharePath()
//            + "/icons/from_qtcreator/";

    static const QString iconsRoot = ExtensionSystem::PluginManager::instance()->sharePath()
            + "/icons/";

    Widgets::IconProvider* icons = Widgets::IconProvider::self();

    static const int MaxColumns = 2;

    int row = 0;
    int col = 0;
    QFrame * line = 0;

    QWidget * c = ui->visibleIconsGrid;
    QGridLayout * l = new QGridLayout(c);
    c->setLayout(l);

    toolbarVisibleItems_ << new QCheckBox(MainWindow::tr("New program"), this);
    toolbarVisibleItems_.last()->setObjectName("file-new");
    toolbarVisibleItems_.last()->setIcon(icons->iconForName(toolbarVisibleItems_.last()->objectName()));
    l->addWidget(toolbarVisibleItems_.last(), row, col, 1, 1, Qt::AlignLeft|Qt::AlignVCenter);
    if (++col >= MaxColumns) {row++; col = 0;}

    toolbarVisibleItems_ << new QCheckBox(MainWindow::tr("Open..."), this);
    toolbarVisibleItems_.last()->setObjectName("file-open");
    toolbarVisibleItems_.last()->setIcon(icons->iconForName(toolbarVisibleItems_.last()->objectName()));
    l->addWidget(toolbarVisibleItems_.last(), row, col, 1, 1, Qt::AlignLeft|Qt::AlignVCenter);
    if (++col >= MaxColumns) {row++; col = 0;}

    toolbarVisibleItems_ << new QCheckBox(MainWindow::tr("Save"), this);
    toolbarVisibleItems_.last()->setObjectName("file-save");
    toolbarVisibleItems_.last()->setIcon(icons->iconForName(toolbarVisibleItems_.last()->objectName()));
    l->addWidget(toolbarVisibleItems_.last(), row, col, 1, 1, Qt::AlignLeft|Qt::AlignVCenter);
    if (++col >= MaxColumns) {row++; col = 0;}

    line = new QFrame(this);
    line->setObjectName(QString::fromUtf8("line"));
    line->setFixedHeight(3);
    line->setMinimumWidth(480);
    line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    l->addWidget(line, ++row, 0, 1, MaxColumns, Qt::AlignCenter);
    row ++; col = 0;

    toolbarVisibleItems_ << new QCheckBox(tr("Cut selection to clipboard"), this);
    toolbarVisibleItems_.last()->setObjectName("edit-cut");
    toolbarVisibleItems_.last()->setIcon(icons->iconForName(toolbarVisibleItems_.last()->objectName()));
    l->addWidget(toolbarVisibleItems_.last(), row, col, 1, 1, Qt::AlignLeft|Qt::AlignVCenter);
    if (++col >= MaxColumns) {row++; col = 0;}

    toolbarVisibleItems_ << new QCheckBox(tr("Copy selection to clipboard"), this);
    toolbarVisibleItems_.last()->setObjectName("edit-copy");
    toolbarVisibleItems_.last()->setIcon(icons->iconForName(toolbarVisibleItems_.last()->objectName()));
    l->addWidget(toolbarVisibleItems_.last(), row, col, 1, 1, Qt::AlignLeft|Qt::AlignVCenter);
    if (++col >= MaxColumns) {row++; col = 0;}

    toolbarVisibleItems_ << new QCheckBox(tr("Paste from clipboard"), this);
    toolbarVisibleItems_.last()->setObjectName("edit-paste");
    toolbarVisibleItems_.last()->setIcon(icons->iconForName(toolbarVisibleItems_.last()->objectName()));
    l->addWidget(toolbarVisibleItems_.last(), row, col, 1, 1, Qt::AlignLeft|Qt::AlignVCenter);
    if (++col >= MaxColumns) {row++; col = 0;}

    line = new QFrame(this);
    line->setObjectName(QString::fromUtf8("line"));
    line->setFixedHeight(3);
    line->setMinimumWidth(480);
    line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    l->addWidget(line, ++row, 0, 1, MaxColumns, Qt::AlignCenter);
    row ++; col = 0;

    toolbarVisibleItems_ << new QCheckBox(tr("Undo last action"), this);
    toolbarVisibleItems_.last()->setObjectName("edit-undo");
    toolbarVisibleItems_.last()->setIcon(icons->iconForName(toolbarVisibleItems_.last()->objectName()));
    l->addWidget(toolbarVisibleItems_.last(), row, col, 1, 1, Qt::AlignLeft|Qt::AlignVCenter);
    if (++col >= MaxColumns) {row++; col = 0;}

    toolbarVisibleItems_ << new QCheckBox(tr("Redo last undoed action"), this);
    toolbarVisibleItems_.last()->setObjectName("edit-redo");
    toolbarVisibleItems_.last()->setIcon(icons->iconForName(toolbarVisibleItems_.last()->objectName()));
    l->addWidget(toolbarVisibleItems_.last(), row, col, 1, 1, Qt::AlignLeft|Qt::AlignVCenter);
    if (++col >= MaxColumns) {row++; col = 0;}

    line = new QFrame(this);
    line->setObjectName(QString::fromUtf8("line"));
    line->setFixedHeight(3);
    line->setMinimumWidth(480);
    line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    l->addWidget(line, ++row, 0, 1, MaxColumns, Qt::AlignCenter);
    row ++; col = 0;

    toolbarVisibleItems_ << new QCheckBox(KumirProgram::tr("Blind run"), this);
    toolbarVisibleItems_.last()->setObjectName("run-blind");
    toolbarVisibleItems_.last()->setIcon(icons->iconForName(toolbarVisibleItems_.last()->objectName()));
    l->addWidget(toolbarVisibleItems_.last(), row, col, 1, 1, Qt::AlignLeft|Qt::AlignVCenter);
    if (++col >= MaxColumns) {row++; col = 0;}

    toolbarVisibleItems_ << new QCheckBox(KumirProgram::tr("Regular run"), this);
    toolbarVisibleItems_.last()->setObjectName("run-regular");
    toolbarVisibleItems_.last()->setIcon(icons->iconForName(toolbarVisibleItems_.last()->objectName()));
    l->addWidget(toolbarVisibleItems_.last(), row, col, 1, 1, Qt::AlignLeft|Qt::AlignVCenter);
    if (++col >= MaxColumns) {row++; col = 0;}

    toolbarVisibleItems_ << new QCheckBox(KumirProgram::tr("Stop"), this);
    toolbarVisibleItems_.last()->setObjectName("run-stop");
    toolbarVisibleItems_.last()->setIcon(icons->iconForName(toolbarVisibleItems_.last()->objectName()));
    l->addWidget(toolbarVisibleItems_.last(), row, col, 1, 1, Qt::AlignLeft|Qt::AlignVCenter);
    if (++col >= MaxColumns) {row++; col = 0;}

    line = new QFrame(this);
    line->setObjectName(QString::fromUtf8("line"));
    line->setFixedHeight(3);
    line->setMinimumWidth(480);
    line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    l->addWidget(line, ++row, 0, 1, MaxColumns, Qt::AlignCenter);
    row ++; col = 0;

    toolbarVisibleItems_ << new QCheckBox(KumirProgram::tr("Step over"), this);
    toolbarVisibleItems_.last()->setObjectName("run-step-over");
    toolbarVisibleItems_.last()->setIcon(icons->iconForName(toolbarVisibleItems_.last()->objectName()));
    l->addWidget(toolbarVisibleItems_.last(), row, col, 1, 1, Qt::AlignLeft|Qt::AlignVCenter);
    if (++col >= MaxColumns) {row++; col = 0;}

    toolbarVisibleItems_ << new QCheckBox(KumirProgram::tr("Step in"), this);
    toolbarVisibleItems_.last()->setObjectName("run-step-in");
    toolbarVisibleItems_.last()->setIcon(icons->iconForName(toolbarVisibleItems_.last()->objectName()));
    l->addWidget(toolbarVisibleItems_.last(), row, col, 1, 1, Qt::AlignLeft|Qt::AlignVCenter);
    if (++col >= MaxColumns) {row++; col = 0;}

    toolbarVisibleItems_ << new QCheckBox(KumirProgram::tr("Step to end"), this);    
    toolbarVisibleItems_.last()->setObjectName("run-step-out");
    toolbarVisibleItems_.last()->setIcon(icons->iconForName(toolbarVisibleItems_.last()->objectName()));
    l->addWidget(toolbarVisibleItems_.last(), row, col, 1, 1, Qt::AlignLeft|Qt::AlignVCenter);
    if (++col >= MaxColumns) {row++; col = 0;}

    line = new QFrame(this);
    line->setObjectName(QString::fromUtf8("line"));
    line->setFixedHeight(3);
    line->setMinimumWidth(480);
    line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    l->addWidget(line, ++row, 0, 1, MaxColumns, Qt::AlignCenter);
    row ++; col = 0;

    toolbarVisibleItems_ << new QCheckBox(Plugin::tr("Courses"), this);
    toolbarVisibleItems_.last()->setObjectName("window-courses");
    toolbarVisibleItems_.last()->setIcon(icons->iconForName("window-cources"));
    l->addWidget(toolbarVisibleItems_.last(), row, col, 1, 1, Qt::AlignLeft|Qt::AlignVCenter);
    if (++col >= MaxColumns) {row++; col = 0;}

    QList<Shared::ActorInterface*> actors =
            ExtensionSystem::PluginManager::instance()
            ->findPlugins<Shared::ActorInterface>();

    Q_FOREACH(Shared::ActorInterface* actor, actors) {
        const QString actorName = Shared::actorCanonicalName(actor->localizedModuleName(QLocale::Russian));
        const QString actorObjectname = Shared::actorCanonicalName(actor->asciiModuleName()).replace(" ", "-").toLower();
        QString mainIconFileName = iconsRoot+"actors/"+actor->mainIconName()+".png";
        if (!QFile(mainIconFileName).exists())
            mainIconFileName = iconsRoot+"actors/"+actor->mainIconName()+"_22x22.png";
        QString rcIconFileName = iconsRoot+"actors/"+actor->pultIconName()+".png";
        if (!QFile(rcIconFileName).exists())
            rcIconFileName = iconsRoot+"actors/"+actor->pultIconName()+"_22x22.png";
        if (actor->mainWidget()) {
            const QString title = actorName;
            const QString objectName = "window-actor-" + actorObjectname;
            toolbarVisibleItems_ << new QCheckBox(title, this);
            toolbarVisibleItems_.last()->setIcon(QIcon(mainIconFileName));
            toolbarVisibleItems_.last()->setObjectName(objectName);
            const bool tristate = "window-actor-robot"!=objectName && "window-actor-draw"!=objectName;
            toolbarVisibleItems_.last()->setTristate(tristate);
            l->addWidget(toolbarVisibleItems_.last(), row, col, 1, 1, Qt::AlignLeft|Qt::AlignVCenter);
            if (++col >= MaxColumns) {row++; col = 0;}
        }
        if (actor->pultWidget()) {
            const QString title = actorName + " - " + Plugin::tr("Remote Control");
            const QString objectName = "window-control-" + actorObjectname;
            toolbarVisibleItems_ << new QCheckBox(title, this);
            toolbarVisibleItems_.last()->setIcon(QIcon(rcIconFileName));
            toolbarVisibleItems_.last()->setObjectName(objectName);
            const bool tristate = "window-control-robot"!=objectName;
            toolbarVisibleItems_.last()->setTristate(tristate);
            l->addWidget(toolbarVisibleItems_.last(), row, col, 1, 1, Qt::AlignLeft|Qt::AlignVCenter);
            if (++col >= MaxColumns) {row++; col = 0;}
        }
    }

}

void GUISettingsPage::init()
{
    const QString layoutValue = settings_->value(LayoutKey, ColumnsFirstValue).toString();
    if (layoutValue == ColumnsFirstValue) {
        ui->btnColumnsFirst->setChecked(true);
    }
    else {
        ui->btnRowsFirst->setChecked(true);
    }

    // Visible toolbar icons
    for (int i=0; i<toolbarVisibleItems_.size(); i++) {
        QCheckBox* const item = toolbarVisibleItems_[i];
        const QString key = "MainToolBar/" + (
                    item->objectName().isEmpty()
                    ? QString("unknown") : item->objectName()
                      );
        const Qt::CheckState state = ToolbarContextMenu::f2cs(
                    settings_->value(key, ToolbarContextMenu::cs2f(
                                         ToolbarContextMenu::defaultVisible(item->objectName())
                                         )).toFloat()
                    );
        item->setCheckState(state);
    }
}

void GUISettingsPage::accept()
{
    const QString layoutValue = settings_->value(LayoutKey, ColumnsFirstValue).toString();
    if (ui->btnColumnsFirst->isChecked()) {
        settings_->setValue(LayoutKey, ColumnsFirstValue);
    }
    else {
        settings_->setValue(LayoutKey, RowsFirstValue);
    }

    QStringList keys;

    if (layoutValue != settings_->value(LayoutKey, RowsFirstValue).toString()) {
        keys << LayoutKey;
    }

    for (int i=0; i<toolbarVisibleItems_.size(); i++) {
        QCheckBox* const item = toolbarVisibleItems_[i];
        const QString key = "MainToolBar/" + (
                    item->objectName().isEmpty()
                    ? QString("unknown") : item->objectName()
                      );
        const double val = ToolbarContextMenu::cs2f(item->checkState());
        settings_->setValue(key, val);
        keys << key;
    }

    emit settingsChanged(keys);
}

void GUISettingsPage::resetToDefaults()
{
    const QString layoutValue = settings_->value(LayoutKey, ColumnsFirstValue).toString();
    ui->btnRowsFirst->setChecked(true);
    settings_->setValue(LayoutKey, RowsFirstValue);
    QStringList keys;
    if (layoutValue != settings_->value(LayoutKey, RowsFirstValue).toString()) {
        keys << LayoutKey;
    }

    for (int i=0; i<toolbarVisibleItems_.size(); i++) {
        QCheckBox* const item = toolbarVisibleItems_[i];
        const QString key = "MainToolBar/" + (
                    item->objectName().isEmpty()
                    ? QString("unknown") : item->objectName()
                      );
        Qt::CheckState state = ToolbarContextMenu::defaultVisible(item->objectName());
        item->setCheckState(state);
        const double val = ToolbarContextMenu::cs2f(state);
        settings_->setValue(key, val);
        keys << key;
    }

    emit settingsChanged(keys);
}

GUISettingsPage::~GUISettingsPage()
{
    delete ui;
}

} // namespace CoreGUI

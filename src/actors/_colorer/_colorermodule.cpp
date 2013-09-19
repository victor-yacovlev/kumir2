/*
This file is generated, but you can safely change it
until you run "gen_actor_source.py" with "--project" flag.

Generated file is just a skeleton for module contents.
You should change it corresponding to functionality.
*/

// Self include
#include "_colorermodule.h"

// Kumir includes
#include "extensionsystem/kplugin.h"

// Qt includes
#include <QtCore>
#include <QtGui>

namespace Actor_Colorer {

_ColorerModule::_ColorerModule(ExtensionSystem::KPlugin * parent)
    : _ColorerModuleBase(parent)
{
    // Module constructor, called once on plugin load
    // TODO implement me
}

/* public static */ QList<ExtensionSystem::CommandLineParameter> _ColorerModule::acceptableCommandLineParameters()
{
    // See "src/shared/extensionsystem/commandlineparameter.h" for constructor details
    return QList<ExtensionSystem::CommandLineParameter>();
}

/* public slot */ void _ColorerModule::changeGlobalState(ExtensionSystem::GlobalState old, ExtensionSystem::GlobalState current)
{
    // Called when changed kumir state. The states are defined as enum ExtensionSystem::GlobalState:
    /*
    namespace ExtensionSystem {
        enum GlobalState {
            GS_Unlocked, // Edit mode
            GS_Observe, // Observe mode
            GS_Running, // Running mode
            GS_Input,  // User input required
            GS_Pause  // Running paused
        };
    }
    */
    // TODO implement me
    using namespace ExtensionSystem;  // not to write "ExtensionSystem::" each time in this method scope
    Q_UNUSED(old);  // Remove this line on implementation
    Q_UNUSED(current);  // Remove this line on implementation
}

/* public slot */ void _ColorerModule::loadActorData(QIODevice * source)
{
    // Set actor specific data (like environment)
    // The source should be ready-to-read QIODevice like QBuffer or QFile
    Q_UNUSED(source);  // By default do nothing

}





/* public slot */ void _ColorerModule::reloadSettings(ExtensionSystem::SettingsPtr settings, const QStringList & keys)
{
    // Updates setting on module load, workspace change or appliyng settings dialog.
    // If @param keys is empty -- should reload all settings, otherwise load only setting specified by @param keys
    // TODO implement me
    Q_UNUSED(settings);  // Remove this line on implementation
    Q_UNUSED(keys);  // Remove this line on implementation
}

/* public slot */ void _ColorerModule::reset()
{
    // Resets module to initial state before program execution
    // TODO implement me
}


static QColor parseColor(const QString &css)
{
    QColor result = QColor::Invalid;
    QString normCSS = css.toLower().remove(QRegExp("\\s"));
    QRegExp rxCSS("(rgb|rgba|cmyk|cmyka|hsl|hsla|hsv|hsva)\\((.*)\\)");
    QMap<QString,QString> ruNames;
    ruNames[QString::fromUtf8("белый")] = "white";
    ruNames[QString::fromUtf8("черный")] = "black";
    ruNames[QString::fromUtf8("чёрный")] = "black";
    ruNames[QString::fromUtf8("серый")] = "gray";
    ruNames[QString::fromUtf8("фиолетовый")] = "magenta";
    ruNames[QString::fromUtf8("синий")] = "blue";
    ruNames[QString::fromUtf8("голубой")] = "lightblue";
    ruNames[QString::fromUtf8("зелёный")] = "lime";
    ruNames[QString::fromUtf8("зеленый")] = "lime";
    ruNames[QString::fromUtf8("жёлтый")] = "yellow";
    ruNames[QString::fromUtf8("желтый")] = "yellow";
    ruNames[QString::fromUtf8("оранжевый")] = "orange";
    ruNames[QString::fromUtf8("красный")] = "red";

    if (normCSS.startsWith("#")) {
        result = QColor(css); // Qt supports HTML-colors
    }
    else if (normCSS.isEmpty() ||
             normCSS==QString::fromUtf8("прозрачный") ||
             normCSS==QString::fromUtf8("transparent")
             ) {
        result = QColor(0,0,0,0);
    }
    else if (rxCSS.exactMatch(normCSS)) {
        const QString model = rxCSS.cap(1);
        const QStringList components = rxCSS.cap(2).split(",");
        bool ok = true;
        foreach (const QString &comp, components) {
            if (ok) {
                comp.toInt(&ok);
            }
        }
        if (ok) {
            if (model.length()!=components.size()) {
                result = QColor::Invalid;
            }
            else if (model=="rgb") {
                result.setRgb(components[0].toInt(), components[1].toInt(), components[2].toInt());
            }
            else if (model=="rgba") {
                result.setRgb(components[0].toInt(), components[1].toInt(), components[2].toInt(), components[3].toInt());
            }
            else if (model=="hsl") {
                result.setHsl(components[0].toInt(), components[1].toInt(), components[2].toInt());
            }
            else if (model=="hsla") {
                result.setHsl(components[0].toInt(), components[1].toInt(), components[2].toInt(), components[3].toInt());
            }
            else if (model=="hsv") {
                result.setHsv(components[0].toInt(), components[1].toInt(), components[2].toInt());
            }
            else if (model=="hsva") {
                result.setHsv(components[0].toInt(), components[1].toInt(), components[2].toInt(), components[3].toInt());
            }
            else if (model=="cmyk") {
                result.setCmyk(components[0].toInt(), components[1].toInt(), components[2].toInt(), components[3].toInt());
            }
            else if (model=="cmyka") {
                result.setCmyk(components[0].toInt(), components[1].toInt(), components[2].toInt(), components[3].toInt(), components[4].toInt());
            }
        }
        else {
            result = QColor::Invalid;
        }
    }
    else if (ruNames.contains(normCSS)) {
        result = QColor(ruNames[normCSS]);
    }
    else {
        result = QColor(normCSS);
    }
    return result;
}


/* public slot */ Color _ColorerModule::runOperatorINPUT(const QString& x, bool& ok)
{
    /* алг цвет ввод(лит x, рез лог ok) */
    QStringList validValues;
    validValues << QString::fromUtf8("прозрачный");
    validValues << QString::fromUtf8("белый");
    validValues << QString::fromUtf8("черный");
    validValues << QString::fromUtf8("чёрный");
    validValues << QString::fromUtf8("серый");
    validValues << QString::fromUtf8("фиолетовый");
    validValues << QString::fromUtf8("синий");
    validValues << QString::fromUtf8("голубой");
    validValues << QString::fromUtf8("зелёный");
    validValues << QString::fromUtf8("зеленый");
    validValues << QString::fromUtf8("жёлтый");
    validValues << QString::fromUtf8("желтый");
    validValues << QString::fromUtf8("оранжевый");
    validValues << QString::fromUtf8("красный");
    ok = validValues.contains(x.trimmed());
    Color result;
    if (ok) {
        QColor c = parseColor(x.trimmed());
        result.cssValue = c.name();
    }
    return result;
    
}

/* public slot */ QString _ColorerModule::runOperatorOUTPUT(const Color& x)
{
    /* алг лит вывод(цвет x) */
    QString result;
    QMap<QString,QString> standardColorNames;
    standardColorNames[parseColor(QString::fromUtf8("прозрачный")).name()] = (QString::fromUtf8("прозрачный"));
    standardColorNames[parseColor(QString::fromUtf8("белый")).name()] = (QString::fromUtf8("белый"));
    standardColorNames[parseColor(QString::fromUtf8("чёрный")).name()] = (QString::fromUtf8("чёрный"));
    standardColorNames[parseColor(QString::fromUtf8("серый")).name()] = (QString::fromUtf8("серый"));
    standardColorNames[parseColor(QString::fromUtf8("фиолетовый")).name()] = (QString::fromUtf8("фиолетовый"));
    standardColorNames[parseColor(QString::fromUtf8("синий")).name()] = (QString::fromUtf8("синий"));
    standardColorNames[parseColor(QString::fromUtf8("голубой")).name()] = (QString::fromUtf8("голубой"));
    standardColorNames[parseColor(QString::fromUtf8("зелёный")).name()] = (QString::fromUtf8("зелёный"));
    standardColorNames[parseColor(QString::fromUtf8("жёлтый")).name()] = (QString::fromUtf8("жёлтый"));
    standardColorNames[parseColor(QString::fromUtf8("оранжевый")).name()] = (QString::fromUtf8("оранжевый"));
    standardColorNames[parseColor(QString::fromUtf8("красный")).name()] = (QString::fromUtf8("красный"));
    if (standardColorNames.contains(x.cssValue)) {
        result = standardColorNames[x.cssValue];
    }
    else {
        result = x.cssValue;
    }
    return result;
    
}



} // namespace Actor_Colorer

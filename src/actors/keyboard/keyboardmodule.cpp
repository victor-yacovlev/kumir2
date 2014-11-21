/*
This file is generated, but you can safely change it
until you run "gen_actor_source.py" with "--project" flag.

Generated file is just a skeleton for module contents.
You should change it corresponding to functionality.
*/

// Self include
#include "keyboardmodule.h"

// Kumir includes
#include "extensionsystem/kplugin.h"
#include "interfaces/plugininterface.h"

// Qt includes
#include <QtCore>
#include <QtGui>

namespace ActorKeyboard {

KeyboardModule::KeyboardModule(ExtensionSystem::KPlugin * parent)
    : KeyboardModuleBase(parent)    
{
    // Module constructor, called once on plugin load
    // TODO implement me
}

/* public static */ QList<ExtensionSystem::CommandLineParameter> KeyboardModule::acceptableCommandLineParameters()
{
    // See "src/shared/extensionsystem/commandlineparameter.h" for constructor details
    return QList<ExtensionSystem::CommandLineParameter>();
}

/* public slot */ void KeyboardModule::changeGlobalState(ExtensionSystem::GlobalState old, ExtensionSystem::GlobalState current)
{
    // Called when changed kumir state. The states are defined as enum ExtensionSystem::GlobalState:

    if (Shared::PluginInterface::GS_Running == old) {
        if (Shared::PluginInterface::GS_Observe == current || Shared::PluginInterface::GS_Unlocked == current) {
            finalizeRun();
        }
    }
}

/* public slot */ void KeyboardModule::loadActorData(QIODevice * source)
{
    // Set actor specific data (like environment)
    // The source should be ready-to-read QIODevice like QBuffer or QFile
    Q_UNUSED(source);  // By default do nothing

}

/* public */ QWidget* KeyboardModule::mainWidget() const
{
    // Returns module main view widget, or nullptr if there is no any views
    // NOTE: the method is const and might be called at any time,
    //       so DO NOT create widget here, just return!
    // TODO implement me
    return nullptr;
}

/* public */ QWidget* KeyboardModule::pultWidget() const
{
    // Returns module control view widget, or nullptr if there is no control view
    // NOTE: the method is const and might be called at any time,
    //       so DO NOT create widget here, just return!
    // TODO implement me
    return nullptr;
}

/* public slot */ void KeyboardModule::reloadSettings(ExtensionSystem::SettingsPtr settings, const QStringList & keys)
{
    // Updates setting on module load, workspace change or appliyng settings dialog.
    // If @param keys is empty -- should reload all settings, otherwise load only setting specified by @param keys
    // TODO implement me
    Q_UNUSED(settings);  // Remove this line on implementation
    Q_UNUSED(keys);  // Remove this line on implementation
}

/* public slot */ void KeyboardModule::reset()
{
    runClearKeyBuffer();
    // Register event filter
    QWidgetList widgets = QApplication::allWidgets();
    Q_FOREACH(QWidget * w, widgets) {
        w->installEventFilter(this);
    }
}

void KeyboardModule::finalizeRun()
{
    // Deregister event filter
    QWidgetList widgets = QApplication::allWidgets();
    Q_FOREACH(QWidget * w, widgets) {
        w->removeEventFilter(this);
    }
}

/* public slot */ void KeyboardModule::setAnimationEnabled(bool enabled)
{
    // Sets GUI animation flag on run
    // NOTE this method just setups a flag and might be called anytime, even module not needed
    // TODO implement me
    Q_UNUSED(enabled);  // Remove this line on implementation
}

/* public slot */ bool KeyboardModule::runKeyHit()
{
    /* алг лог клавиша нажата */
    bufferLock_.lock();
    const bool result = ! buffer_.isEmpty();
    bufferLock_.unlock();
    return result;
    
}

/* public slot */ int KeyboardModule::runKeyCode()
{
    /* алг цел код клавиши */
    bufferLock_.lock();
    const int result = buffer_.isEmpty() ? 0 : buffer_.dequeue().kumirCode;
    qDebug() << "Got key code : " << result << "; keys left in buffer: " << buffer_.size();
    bufferLock_.unlock();
    return result;
    
}

/* public slot */ void KeyboardModule::runClearKeyBuffer()
{
    /* алг очистить клавиши */

    bufferLock_.lock();
    lastPressed_ = KeyEvent();
    buffer_.clear();
    bufferLock_.unlock();
    
}

/* public slot */ int KeyboardModule::runKEY_UP()
{
    /* алг цел КЛ_ВВЕРХ */
    return 38;
    
}

/* public slot */ int KeyboardModule::runKEY_DOWN()
{
    /* алг цел КЛ_ВНИЗ */
    return 40;
    
}

/* public slot */ int KeyboardModule::runKEY_LEFT()
{
    /* алг цел КЛ_ВЛЕВО */
    return 37;
    
}

/* public slot */ int KeyboardModule::runKEY_RIGHT()
{
    /* алг цел КЛ_ВПРАВО */
    return 39;
    
}

/* public slot */ Keycode KeyboardModule::runOperatorINPUT(const QString& s, bool& ok)
{
    /* алг _клав ввод(лит s, рез лог ok) */
    const QString normalized = s.trimmed().toUpper();
    Keycode result;
    result.code = 0;
    ok = false;
    if (normalized.length() < 4 || !normalized.startsWith(QString::fromUtf8("КЛ_"))) {
        return result;
    }
#define C(x, y) QPair<QString,int>(QString::fromUtf8(x),y)
#define D(x, y) QPair<QString,int>(QString::fromUtf8(x), static_cast<int>(QString::fromLatin1(y).at(0).toLatin1()))
    static const QPair<QString,int> CODES[] = {
        C("НАЗАД", 8),
        C("BACKSPACE", 8),
        C("TAB", 9),
        C("ВВОД", 13),
        C("ENTER", 13),
        C("RETURN", 13),
        C("SPACE", 32),
        C("ПРОБЕЛ", 32),
        C("PAGEUP", 33),
        C("PGUP", 33),
        C("PAGEDOWN", 34),
        C("PGDOWN", 34),
        C("END", 35),
        C("HOME", 36),
        C("ВЛЕВО", 37),
        C("ВВЕРХ", 38),
        C("ВПРАВО", 39),
        C("ВНИЗ", 40),
        C("INSERT", 45),
        C("DELETE", 46),
        C("F1", 112), C("F2", 113), C("F3", 114), C("F4", 115), C("F5", 116), C("F6", 117),
        C("F7", 118), C("F8", 119), C("F9", 120), C("F10", 121), C("F11", 122), C("F12", 123),
        D("Й", "Q"), D("Ц", "W"), D("У", "E"), D("К", "R"), D("Е", "T"), D("Н", "Y"), D("Г", "U"), D("Ш", "I"), D("Щ", "O"), D("З", "P"),
        D("Ф", "A"), D("Ы", "S"), D("В", "D"), D("А", "F"), D("П", "G"), D("Р", "H"), D("О", "J"), D("Л", "K"), D("Д", "L"),
        D("Я", "Z"), D("Ч", "X"), D("С", "C"), D("М", "V"), D("И", "B"), D("Т", "N"), D("Ь", "M"),
        C("", 0)
    };
#undef C
#undef D
    size_t index = 0;
    const QString keyName = normalized.mid(3);
    while (0 != CODES[index].second) {
        const QString & match = CODES[index].first;
        if (match == keyName) {
            ok = true;
            result.code = CODES[index].second;
            break;
        }
        ++index;
    }
    if (!ok && keyName.length()==1) {
        const QChar aKey = keyName.at(0);
        if ('\0' != aKey.toLatin1()) {
            ok = true;
            result.code = aKey.toLatin1();
        }
    }
    return result;
    
}

QString KeyboardModule::runOperatorOUTPUT(const Keycode &x)
{
    return QString::number(x.code);
}

/* public slot */ Keycode KeyboardModule::runOperatorASSIGN(const int x)
{
    /* алг _клав :=(цел x) */
    Keycode result;
    result.code = x;
    return result;
}

int KeyboardModule::runOperatorASSIGN(const Keycode &x)
{
    return x.code;
}

bool KeyboardModule::runOperatorEQUAL(const Keycode &self, const Keycode &other)
{
    return self.code == other.code;
}

bool KeyboardModule::runOperatorEQUAL(const Keycode &self, int other)
{
    return self.code == other;
}

bool KeyboardModule::runOperatorEQUAL(int other, const Keycode &self)
{
    return self.code == other;
}

bool KeyboardModule::runOperatorNOTEQUAL(const Keycode &self, const Keycode &other)
{
    return self.code != other.code;
}

bool KeyboardModule::runOperatorNOTEQUAL(const Keycode &self, int other)
{
    return self.code != other;
}

bool KeyboardModule::runOperatorNOTEQUAL(int other, const Keycode &self)
{
    return self.code != other;
}

bool KeyboardModule::runOperatorLESS(const Keycode &self, const Keycode &other)
{
    return self.code < other.code;
}

bool KeyboardModule::runOperatorLESS(const Keycode &self, int other)
{
    return self.code < other;
}

bool KeyboardModule::runOperatorLESS(int self, const Keycode &other)
{
    return self < other.code;
}

bool KeyboardModule::runOperatorGREATER(const Keycode &self, const Keycode &other)
{
    return self.code > other.code;
}

bool KeyboardModule::runOperatorGREATER(const Keycode &self, int other)
{
    return self.code > other;
}

bool KeyboardModule::runOperatorGREATER(int self, const Keycode &other)
{
    return self > other.code;
}

int KeyboardModule::runOperatorPLUS(const Keycode &self, const Keycode &other)
{
    return self.code + other.code;
}

int KeyboardModule::runOperatorPLUS(const Keycode &self, int other)
{
    return self.code + other;
}

int KeyboardModule::runOperatorPLUS(int self, const Keycode &other)
{
    return self + other.code;
}

int KeyboardModule::runOperatorMINUS(const Keycode &self, const Keycode &other)
{
    return self.code - other.code;
}

int KeyboardModule::runOperatorMINUS(const Keycode &self, int other)
{
    return self.code - other;
}

int KeyboardModule::runOperatorMINUS(int self, const Keycode &other)
{
    return self - other.code;
}

int KeyboardModule::runOperatorASTERISK(const Keycode &self, const Keycode &other)
{
    return self.code * other.code;
}

int KeyboardModule::runOperatorASTERISK(const Keycode &self, int other)
{
    return self.code * other;
}

int KeyboardModule::runOperatorASTERISK(int self, const Keycode &other)
{
    return self * other.code;
}

bool KeyboardModule::eventFilter(QObject *obj, QEvent *event)
{
    if (QEvent::KeyPress == event->type()) {
        qDebug() << "From " << obj->metaObject()->className();
        QKeyEvent * keyEvent = static_cast<QKeyEvent*>(event);
        qDebug() << "Catched Qt code: " << keyEvent->key();
        const int code = polyakovCodeOfKey(keyEvent->key(), keyEvent->text());
        if (code) {
            qDebug() << "Catched code: " << code;
            bufferLock_.lock();
            const KeyEvent keyEvent(code);
            bool addToBuffer = true;
            if (lastPressed_.kumirCode) {
                qint64 delta = keyEvent.timestamp - lastPressed_.timestamp;
                if (delta < KeyEvent::MAX_DELTA) {
                    addToBuffer = false;
                }
            }
            lastPressed_ = keyEvent;
            if (addToBuffer) {
                buffer_.enqueue(KeyEvent(code));
            }
            bufferLock_.unlock();
        }
    }
    return QObject::eventFilter(obj, event);
}

int KeyboardModule::polyakovCodeOfKey(int qtCode, const QString & text)
{
    if (1 == text.length()) {
#define D(x, y) QPair<QString,int>(QString::fromUtf8(x), static_cast<int>(QString::fromLatin1(y).at(0).toLatin1()))
        const QString normalized = text.toUpper();
        static const QPair<QString,int> CODES[] = {
            D("Й", "Q"), D("Ц", "W"), D("У", "E"), D("К", "R"), D("Е", "T"), D("Н", "Y"), D("Г", "U"), D("Ш", "I"), D("Щ", "O"), D("З", "P"),
            D("Ф", "A"), D("Ы", "S"), D("В", "D"), D("А", "F"), D("П", "G"), D("Р", "H"), D("О", "J"), D("Л", "K"), D("Д", "L"),
            D("Я", "Z"), D("Ч", "X"), D("С", "C"), D("М", "V"), D("И", "B"), D("Т", "N"), D("Ь", "M"),
            QPair<QString,int>("", 0)
        };
        size_t index = 0;
        if (0 != normalized.at(0).toLatin1()) {
            return normalized.at(0).toLatin1();
        }
        while (0 != CODES[index].second) {
            if (normalized == CODES[index].first)
                return CODES[index].second;
            ++index;
        }
        return 0;
#undef D
    }

    switch (qtCode) {
    case Qt::Key_Up:
        return 38;
    case Qt::Key_Down:
        return 40;
    case Qt::Key_Left:
        return 37;
    case Qt::Key_Right:
        return 39;
    case Qt::Key_Backspace:
        return 8;
    case Qt::Key_Tab:
        return 9;
    case Qt::Key_Enter: case Qt::Key_Return:
        return 13;
    case Qt::Key_Space:
        return 32;
    case Qt::Key_PageUp:
        return 33;
    case Qt::Key_PageDown:
        return 34;
    case Qt::Key_End:
        return 35;
    case Qt::Key_Home:
        return 36;
    case Qt::Key_Insert:
        return 45;
    case Qt::Key_Delete:
        return 46;
    case Qt::Key_F1:
        return 112;
    case Qt::Key_F2:
        return 113;
    case Qt::Key_F3:
        return 114;
    case Qt::Key_F4:
        return 115;
    case Qt::Key_F5:
        return 116;
    case Qt::Key_F6:
        return 117;
    case Qt::Key_F7:
        return 118;
    case Qt::Key_F8:
        return 119;
    case Qt::Key_F9:
        return 120;
    case Qt::Key_F10:
        return 121;
    case Qt::Key_F11:
        return 122;
    case Qt::Key_F12:
        return 123;
    case Qt::Key_0:
        return 48;
    case Qt::Key_1:
        return 49;
    case Qt::Key_2:
        return 50;
    case Qt::Key_3:
        return 51;
    case Qt::Key_4:
        return 52;
    case Qt::Key_5:
        return 53;
    case Qt::Key_6:
        return 54;
    case Qt::Key_7:
        return 55;
    case Qt::Key_8:
        return 56;
    case Qt::Key_9:
        return 57;
    case Qt::Key_Q:
        return 81;
    case Qt::Key_W:
        return 87;
    case Qt::Key_E:
        return 69;
    default:
        return 0;
    }
}




} // namespace ActorKeyboard

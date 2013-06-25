#ifndef EDITOR_H
#define EDITOR_H

#include <QtGui>
#include "extensionsystem/settings.h"
#include "interfaces/analizerinterface.h"
#include "dataformats/kumfile.h"

namespace Editor {

class Editor
        : public QWidget
{
    Q_OBJECT
public:
    friend class EditorPrivate;
    explicit Editor(
            bool initiallyNotSaved = false,
            ExtensionSystem::SettingsPtr settings = ExtensionSystem::SettingsPtr(),
            Shared::AnalizerInterface * analizer = 0,
            int documentId = 0,
            QWidget *parent = 0
            );
    ~Editor();
    void setTeacherMode(bool v);
    bool isTeacherMode() const;
    QList<QAction*> toolbarActions();
    void setSettings(ExtensionSystem::SettingsPtr s);
    QList<QMenu*> menuActions();
    QList<QWidget*> statusbarWidgets();
    KumFile::Data toKumFile() const;
    void setKumFile(const KumFile::Data & data);
    void setPlainText(const QString & data);
    void setDocumentId(int id);

    const class TextCursor * cursor() const;
    const class TextDocument * document() const;
    const Shared::AnalizerInterface * analizer() const;
    Shared::AnalizerInterface * analizer();
    class TextCursor * cursor();
    class TextDocument * document();
    bool isModified() const;
    void appendMarginText(int lineNo, const QString & text);
    void setMarginText(int lineNo, const QString & text, const QColor &fgColor);
    void clearMarginText();
    void clearMarginText(uint fromLine, uint toLine);
    void setNotModified();
    void checkForClean();
    void lock();
    void unlock();
    void setLineHighlighted(int lineNo, const QColor & color, quint32 colStart, quint32 colEnd);
    void ensureAnalized();
    QByteArray saveState() const;
    void restoreState(const QByteArray &data);
    void unsetAnalizer();
    bool forceNotSavedFlag() const;
    void setForceNotSavedFlag(bool v);
public slots:
    void undo();
    void redo();
signals:
    void urlsDragAndDropped(const QList<QUrl> &);
    void documentCleanChanged(bool v);
    void cursorPositionChanged(uint row, uint column);
    void keyboardLayoutChanged(QLocale::Language lang, bool capslock, bool shift, bool alt);

private:

    void focusInEvent(QFocusEvent *e);
    class EditorPrivate * d;



};

QDataStream & operator<< (QDataStream & stream, const Editor & editor);
QDataStream & operator>> (QDataStream & stream, Editor & editor);

} // namespace Editor

#endif // EDITOR_H

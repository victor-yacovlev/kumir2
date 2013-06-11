#ifndef EDITOR_SUGGESTIONSWINDOW_H
#define EDITOR_SUGGESTIONSWINDOW_H

#include "extensionsystem/settings.h"

#include <QtGui>

#include "interfaces/analizerinterface.h"

namespace Editor {

namespace Ui {
class SuggestionsWindow;
}

class SuggestionsWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit SuggestionsWindow(QWidget *editorWidget);
    void init(const QString & before, const QList<Shared::Suggestion> & suggestions);
    ~SuggestionsWindow();
    void updateSettings(const ExtensionSystem::SettingsPtr settings);
signals:
    void acceptedSuggestion(const QString & text);
    void hidden();
protected:
    void focusInEvent(QFocusEvent * event);
    void keyPressEvent(QKeyEvent * event);
    void keyReleaseEvent(QKeyEvent *event);
    void hideEvent(QHideEvent * event);
    bool eventFilter(QObject * obj, QEvent * event);
protected slots:
    void handleCurrentItemChanged(int currentRow);
    void handleItemActivated(QListWidgetItem * item);
    void acceptItem();
private:
    void createIcons(const ExtensionSystem::SettingsPtr settings);
    Ui::SuggestionsWindow *ui;
    QList<Shared::Suggestion> suggestions_;
    bool keyPressedFlag_;
    QIcon icon_local_;
    QIcon icon_global_;
    QIcon icon_algorithm_;
    QIcon icon_keyword_;
    QIcon icon_module_;
    QIcon icon_kumfile_;
    QIcon icon_other_;
    QWidget * editorWidget_;
};


} // namespace Editor
#endif // EDITOR_SUGGESTIONSWINDOW_H

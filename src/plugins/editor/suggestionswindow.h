#ifndef EDITOR_SUGGESTIONSWINDOW_H
#define EDITOR_SUGGESTIONSWINDOW_H

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
    void updateSettings(const QSettings * settings);
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
    void createIcons(const QSettings * settings);
    Ui::SuggestionsWindow *ui;
    QList<Shared::Suggestion> l_suggestions;
    bool b_keyPressed = false;
    QIcon icon_local;
    QIcon icon_global;
    QIcon icon_algorithm;
    QIcon icon_keyword;
    QIcon icon_module;
    QIcon icon_kumfile;
    QIcon icon_other;
    QWidget * editorWidget = nullptr;
};


} // namespace Editor
#endif // EDITOR_SUGGESTIONSWINDOW_H

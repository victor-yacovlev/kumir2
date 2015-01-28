#ifndef EDITOR_SUGGESTIONSWINDOW_H
#define EDITOR_SUGGESTIONSWINDOW_H

#include "extensionsystem/settings.h"
#include "docbookviewer/docbookview.h"

#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "interfaces/analizerinterface.h"
#include "interfaces/editorinterface.h"

namespace Editor {


namespace Ui {
class SuggestionsWindow;
}

class SuggestionItem
        : public QStandardItem
{
public:
    explicit SuggestionItem(const Shared::Analizer::Suggestion & suggestion,
                            class SuggestionsWindow * factory,
                            Shared::EditorInterface * editorPlugin,
                            DocBookViewer::DocBookView * helpViewer);
    inline bool hasHelpEntry() const { return hasHelpEntry_; }
private:
    bool hasHelpEntry_;
};

class SuggestionItemDelegate
        : public QStyledItemDelegate
{
public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

class SuggestionsWindow : public QWidget
{
    friend class SuggestionItem;
    Q_OBJECT
public:
    explicit SuggestionsWindow(QWidget *editorWidget);
    void init(const QString & before,
              const QList<Shared::Analizer::Suggestion> & suggestions,
              Shared::EditorInterface * editorPlugin,
              DocBookViewer::DocBookView * helpViewer
              );
    ~SuggestionsWindow();
    void updateSettings(const ExtensionSystem::SettingsPtr settings);
signals:
    void requestHelpForAlgorithm(const QString & package, const QString & function);
    void acceptedSuggestion(const QString & text);
    void hidden();
protected:
    void focusInEvent(QFocusEvent * event);
    void keyPressEvent(QKeyEvent * event);
    void keyReleaseEvent(QKeyEvent *event);
    void hideEvent(QHideEvent * event);
    bool eventFilter(QObject * obj, QEvent * event);
protected slots:
    void handleItemActivated(const QModelIndex & index);
    void acceptItem();
private:
    void createIcons(const ExtensionSystem::SettingsPtr settings);
    Ui::SuggestionsWindow *ui;
    QStandardItemModel * itemModel_;
    bool keyPressedFlag_;
    QIcon icon_local_;
    QIcon icon_global_;
    QIcon icon_algorithm_;
    QIcon icon_keyword_;
    QIcon icon_module_;
    QIcon icon_kumfile_;
    QIcon icon_other_;
    QWidget * editorWidget_;
    Shared::EditorInterface * editorPlugin_;

};


} // namespace Editor
#endif // EDITOR_SUGGESTIONSWINDOW_H

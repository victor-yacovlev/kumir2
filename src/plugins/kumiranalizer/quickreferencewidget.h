#ifndef KUMIRANALIZER_QUICKREFERENCEWIDGET_H
#define KUMIRANALIZER_QUICKREFERENCEWIDGET_H

#include <QWidget>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QEvent>
#include <QScrollArea>

#include <kumir2/actorinterface.h>


namespace KumirAnalizer {

namespace Ui {
class QuickReferenceWidget;
}

class KumirAnalizerPlugin;

class QuickReferenceWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QuickReferenceWidget(KumirAnalizerPlugin * plugin);
    static const int StatementTopic = 0;
    static const int TypeTopic = 1;
    static const int FunctionTopic = 2;

Q_SIGNALS:
    void openTopicInDocumentation(const int topic, const QString &name);

private Q_SLOTS:
    void handleButtonPressed();

private:
    void relayoutBlocks(int width);
    bool eventFilter(QObject *obj, QEvent *evt);
    void reloadStyleSheet();

    void createKeywordsList();
    void createTypesList();
    void createFunctionsList();

    KumirAnalizerPlugin* _plugin;

    Ui::QuickReferenceWidget * ui;

    QList<Shared::ActorInterface*> _builtInModules;

};

} // namespace KumirAnalizer

#endif // KUMIRANALIZER_QUICKREFERENCEWIDGET_H

#ifndef DECLARATIVE_SETTINGS_PAGE
#define DECLARATIVE_SETTINGS_PAGE

#include <QtCore>
#include <QtGui>

namespace ExtensionSystem {

class DeclarativeSettingsPage:
        public QWidget
{
    Q_OBJECT
public:
    enum Type { Integer, Double, String, Char, Bool, Color, Font };

    struct Entry {
        Type type;
        QString title;
        QVariant defaultValue;
        QVariant minimumValue;
        QVariant maximumValue;

    };

    DeclarativeSettingsPage(
            const QString &pluginName,
            const QString &title,
            QSettings * settings,
            const QMap<QString,Entry> entries
            );

    void setSettingsObject(QSettings * settings);
    inline QSettings * settingsObject() const { return m_settings; }

public slots:
    void init();
    void accept();

signals:
    void settingsChanged();

private slots:
    void showColorDialog();

private:
    static QColor buttonColor(const QToolButton *b);
    static void setButtonColor(QToolButton * b, const QColor &color);

    void addIntegerField(const QString & key, const Entry & entry);
    void addColorField(const QString & key, const Entry & entry);
    void addField(const QString & labelText, QWidget * controlWidget);
    QString s_pluginName;
    QSettings * m_settings;
    QMap<QString,Entry> m_entries;
    QMap<QString,QWidget*> m_widgets;
};

}

#endif

#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include <QtCore>

namespace Editor {

struct ClipboardData {
    enum Type { Invalid, Text, Block } type;
    QString text;
    QString html;
    QStringList block;
};

class Clipboard : public QObject
{
    Q_OBJECT
public:
    explicit Clipboard(QObject *parent = 0);
    static QString BlockMimeType;
public slots:
    void push(const ClipboardData & data);
    void select(int index);
    bool hasContent() const;
    ClipboardData content() const;
    void clear();
    int entriesCount() const;
signals:
    void bufferEntriesCountChanged(int c);
private slots:
    void checkForChanged();
private:
    QList<ClipboardData> m_data;
    int i_selection;

};

} // namespace Editor

#endif // CLIPBOARD_H

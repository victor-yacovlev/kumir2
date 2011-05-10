#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include <QtCore>

namespace Editor {

class Clipboard : public QObject
{
    Q_OBJECT
public:
    explicit Clipboard(QObject *parent = 0);
public slots:
    void push(const QString & text);
    void select(int index);
    bool hasContent() const;
    QString content() const;
    void clear();
private:
    QList<QString> m_data;
    int i_selection;

};

} // namespace Editor

#endif // CLIPBOARD_H

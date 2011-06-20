#ifndef BROWSER_DIR_H
#define BROWSER_DIR_H

#include <QtCore>

namespace Browser {

class Dir : public QObject
{
    Q_OBJECT
public:
    explicit Dir(QObject *parent = 0);

signals:

public slots:
    QStringList files(const QString & filter);
    QStringList dirs();
    void open(const QString & path);
    QString resolveUrl(const QString & entry);
private:
    QDir m_dir;

};

} // namespace Browser

#endif // BROWSER_DIR_H

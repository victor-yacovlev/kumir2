#ifndef APPMANAGER_H
#define APPMANAGER_H

#include <QtGlobal>
#include <QString>
#include <QIcon>
#include <QUrl>

struct Application {
    quintptr id;
    QString name;
    QIcon icon;
};

class ApplicationManager
{
public:
    static ApplicationManager & get();
    QList<Application> applications() const;
    inline void open(const Application & application,
                     const QUrl & url) { open(application.id, url); }
    void open(quintptr applicationId, const QUrl & url);

private:
    explicit ApplicationManager();
    void scanForApplications(const QString & appsDir, const QString & iconsDir);

    struct ApplicationPrivate {
        QString name;
        QString executableFileName;
        QIcon icon;
        bool mdiInterface;
    };
    QList<ApplicationPrivate> applications_;

};

#endif

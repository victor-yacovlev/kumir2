#include "sch_game.h"
#include "fp.h"
#include <QtScript>

namespace Schema {



extern QString normalizeString(const QString &t)
{
    return t.simplified();
}

extern bool parceJSON(const QScriptValue &value, Game &game)
{
    if (!value.isObject()) {
        return false;
    }
    if (!value.property("title").isString()) {
        return false;
    }
    game.title = value.property("title").toString();
    if (value.property("authors").isString()) {
        QString auth = value.property("authors").toString();
        game.authors = MAP(normalizeString, auth.split(","));
    }
    else if (value.property("authors").isArray()) {
        int len = value.property("authors").property("length").toInteger();
        for (int i=0; i<len; i++) {
            QScriptValue v = value.property("authors").property(i);
            if (v.isString()) {
                game.authors += v.toString().simplified();
            }
        }
    }
    if (value.property("license").isString()) {
        game.license = value.property("license").toString();
    }
    if (value.property("copyright").isString()) {
        game.copyright = value.property("copyright").toString();
    }
    if (value.property("homepage").isString()) {
        game.homepage = value.property("homepage").toString();
    }
    if (!value.property("tasks").isArray()) {
        return false;
    }
    int len = value.property("tasks").property("length").toInteger();
    for (int i=0; i<len; i++) {
        QScriptValue v = value.property("tasks").property(i);
        Task task;
        if (parceJSON(v, task)) {
            game.tasks << task;
        }
    }
    game.index = 0;
    if (value.property("index").isNumber()) {
        game.index = value.property("index").toInteger();
    }
    return game.tasks.size()>0 && game.index>=0 && game.index<game.tasks.size();
}


extern QString addLeadingTab(const QString &t);
extern QString addTwoLeadingTabs(const QString &t) { return "\t\t"+t; }
extern QString addBoundingQuotes(const QString &t) { return "\""+t+"\""; }

extern QString generateJSON(const Game &game)
{
    const QString template0 = "{\n\t\"title\": %1,\n\t\"homepage\": %2,\n\t\"authors\": %3,\n\t\"copyright\": %4,\n\t\"license\": %5,\n\t\"tasks\": %6,\n\t\"index\": %7\n}\n";
    QString title = addBoundingQuotes(game.title);
    QString homepage = game.homepage.isEmpty()? "null" : addBoundingQuotes(game.homepage);
    QString copyright = game.copyright.isEmpty()? "null" : addBoundingQuotes(game.copyright);
    QString license = game.license.isEmpty()? "null" : addBoundingQuotes(game.license);
    QString authors;
    if (game.authors.size()==0) {
        authors = "null";
    }
    else if (game.authors.size()==1) {
        authors = addBoundingQuotes(game.authors[0]);
    }
    else {
        authors = "[ "+QStringList(MAP(addBoundingQuotes, game.authors)).join(", ")+" ]";
    }
    QStringList tasksList;
    for (int i=0; i<game.tasks.size(); i++) {
        QStringList taskLines = generateJSON(game.tasks[i]).split("\n");
        for (int j=0; j<taskLines.size(); j++) {
            taskLines[j] = "\t"+taskLines[j];
        }
        tasksList << taskLines.join("\n").trimmed();
    }
    QString tasks = "[ "+ tasksList.join(", ")+" ] ";
    const QString index = QString::number(game.index);
    const QString data = template0
            .arg(title)
            .arg(homepage)
            .arg(authors)
            .arg(copyright)
            .arg(license)
            .arg(tasks)
            .arg(index);
    return data;
}

extern bool isEqual(const Game &a, const Game &b)
{
    if (a.title.simplified()!=b.title.simplified()) {
        return false;
    }
    if (a.authors.size()!=b.authors.size()) {
        return false;
    }
    else {
        bool authorsDiff = false;
        for (int i=0; i<a.authors.size(); i++) {
            if (a.authors[i].simplified()!=b.authors[i].simplified()) {
                authorsDiff = true;
            }
        }
    }
    if (a.license.simplified()!=b.license.simplified()) {
        return false;
    }
    if (a.homepage.simplified()!=b.homepage.simplified()) {
        return false;
    }
    if (a.copyright.simplified()!=b.copyright.simplified()) {
        return false;
    }
    if (a.tasks.size()!=b.tasks.size())
        return false;
    for (int i=0; i<a.tasks.size(); i++) {
        if ( !isEqual(a.tasks[i], b.tasks[i]) )
            return false;
    }
    return true;
}


} // namespace Schema

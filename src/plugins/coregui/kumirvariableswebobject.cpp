#include "kumirvariableswebobject.h"
#include "abstractsyntaxtree/ast_algorhitm.h"

namespace CoreGUI {

KumirVariablesWebObject::KumirVariablesWebObject(QObject *parent) :
    QObject(parent)
{

}

void KumirVariablesWebObject::setProgram(const AST::Data *ast)
{
    l_userModules.clear();
    l_userModulesIds.clear();
    for (int i=0; i<ast->modules.size(); i++) {
        if (ast->modules[i]->header.type==AST::ModTypeUser) {
            l_userModules << ast->modules[i];
            l_userModulesIds << i;
        }
    }
    refreshRoot();
}

void KumirVariablesWebObject::reset(const Shared::RunInterface * runner)
{
    if (m_runner!=runner) {
        m_runner = runner;
        refreshRoot();
    }
    else {
        m_runner = runner;
    }
    checkUnusedWindows();
}

void KumirVariablesWebObject::refreshRoot()
{
    emit jsRequest("updateRoot", QVariantList());
}

QString KumirVariablesWebObject::rootPage() const
{
    QString result;
    for (int i=0; i<l_userModules.size(); i++) {
        result += modulePage(i);
    }
    return result;
}

int KumirVariablesWebObject::modulesCount() const
{
    return l_userModules.size();
}

QString KumirVariablesWebObject::makeArr(const QVariant & value)
{
    QString result;
    if (value.type()==QVariant::List) {
        result = "{ ";
        for (int i=0; i<value.toList().size(); i++) {
            result += makeArr(value.toList().at(i));
            if (i<value.toList().size()-1)
                result += ", ";
        }
        result += "}";
    }
    else {
        if (value.isValid())
            result = value.toString();
        else
            result = QString::fromAscii(
                        "<span class=\"undefined_value\">"
                        "%1"
                        "</span>"
                        )
                    .arg("?");
    }

    return result;
}

void KumirVariablesWebObject::test(const QString & aaa)
{
    qDebug() << "TEST: " << aaa;
}

QString KumirVariablesWebObject::makeTableValue(
    int moduleId, int algorhitmId, int variableId, // These used for hyperlink
    int dimension, const QVariant value // These used for screen value
    )
{
    QString screenValue = makeArr(value);
//    if (screenValue.length()>200)
//        screenValue = screenValue.left(200);
    const QString hyperlink = QString::fromAscii(
                "<a href=\"#\" onclick=\"openTableWindow(%1, %2, %3, %4)\">"
                "%5"
                "</a>"
                )
            .arg(dimension)
            .arg(moduleId)
            .arg(algorhitmId)
            .arg(variableId)
            .arg(screenValue);
    return hyperlink;
}

QString KumirVariablesWebObject::modulePage(int index) const
{
    QString result;
    if (index>=0 && index<l_userModules.size()) {
        AST::Module * mod = l_userModules[index];

        QString moduleName = mod->header.name;
        if (moduleName.isEmpty())
            moduleName = tr("User algorhitms and variables");
        QString moduleBody;
        if (!mod->impl.globals.isEmpty()) {
            moduleBody = QString::fromAscii(
                        "<div class=\"globals\">\n"
                        "  <div class=\"globalstitle\">%1</div>\n"
                        "%2"
                        "</div>\n")
                    .arg(tr("Global variables"))
                    .arg(valuesTable(l_userModulesIds[index], -1, mod->impl.globals));
        }
        for (int i=0; i<mod->impl.algorhitms.size(); i++) {
            if (!mod->impl.algorhitms[i]->impl.locals.isEmpty()) {
                QString algorhitmTitle = mod->impl.algorhitms[i]->header.name;
                if (algorhitmTitle.isEmpty())
                    algorhitmTitle = tr("Main algorhitm");
                else
                    algorhitmTitle = tr("Algorhitm \"%1\"").arg(algorhitmTitle);
                moduleBody += QString::fromAscii(
                            "<div class=\"algorhitm\">\n"
                            "  <div class=\"algorhitmtitle\">%1</div>\n"
                            "%2"
                            "</div>\n"
                            )
                        .arg(algorhitmTitle)
                        .arg(valuesTable(l_userModulesIds[index], i, mod->impl.algorhitms[i]->impl.locals));
            }
        }
        result  = QString::fromAscii(
                    "<div class=\"module\" id=\"module-%1\">\n"
                    "  <div class=\"modulename\">%2</div>\n"
                    "  <div class=\"modulebody\">\n"
                    "%3"
                    "  </div>\n"
                    "</div>\n"
                    )
                .arg(index)
                .arg(moduleName)
                .arg(moduleBody);
    }
    return result;
}

QString KumirVariablesWebObject::valuesTable(int moduleId, int algId, const QList<AST::Variable *> values) const
{
    QString body;
    for (int i=0; i<values.size(); i++) {
        AST::Variable * var = values[i];
        QVariant value; // TODO implement me
        if (m_runner) {
            value = m_runner->value(moduleId, algId, i);
        }
        QString type;
        if (var->baseType==AST::TypeInteger)
            type = tr("integer");
        else if (var->baseType==AST::TypeReal)
            type = tr("real");
        else if (var->baseType==AST::TypeCharect)
            type = tr("charect");
        else if (var->baseType==AST::TypeString)
            type = tr("string");
        else if (var->baseType==AST::TypeBoolean)
            type = tr("boolean");
        if (var->dimension>0) {
            type += " "+tr("table");
        }
        QString screenValue;
        if (!value.isValid()) {
            screenValue = tr("undefined");
        }
        else if (var->dimension>0) {
            screenValue = makeTableValue(moduleId, algId, i, var->dimension, value);
        }
        else {
            screenValue = value.toString();
        }
        body += QString::fromAscii(
                    "<tr class=\"%1\">" // odd or even row
                    "<td class=\"varname_header\">%2</td>" // name
                    "<td class=\"vartype_header\">%3</td>" // type
                    "<td class=\"varvalue_header %4\">%5</td>" // %4 - defined or not, %5 - value
                    "</tr>\n"
                    )
                .arg(i%2==0? "even" : "odd")
                .arg(var->name)
                .arg(type)
                .arg(value.isValid()? "defined_value" : "undefined_value")
                .arg(screenValue)
                ;
    }
    return QString::fromAscii(
                "<table class=\"values\">\n"
                "<tr class=\"valuesheader\"><td>%1</td><td>%2</td><td>%3</td></tr>\n"
                "%4"
                "</table>\n"
                )
            .arg(tr("Name"))
            .arg(tr("Type"))
            .arg(tr("Value"))
            .arg(body);
}

void KumirVariablesWebObject::openTableWindow(int dimension, int moduleId, int algorhitmId, int variableId)
{

    const QString key = QString::fromAscii(
                "%1:%2:%3").arg(moduleId).arg(algorhitmId).arg(variableId);

    if (l_openedChildWindows.contains(key)) {
        emit windowRaiseRequest(l_openedChildWindows[key].widget);
    }
    else {
        TableMetadata metadata;
        AST::Module * mod = l_userModules[l_userModulesIds.indexOf(moduleId)];
        AST::Variable * var = 0;
        metadata.moduleName = mod->header.name;
        if (algorhitmId!=-1) {
            metadata.algorhitmName = mod->impl.algorhitms[algorhitmId]->header.name;
            metadata.global = false;
            var = mod->impl.algorhitms[algorhitmId]->impl.locals[variableId];
        }
        else {
            metadata.global = true;
            var = mod->impl.globals[variableId];
        }
        metadata.variableName = var->name;
        const QString qualifedName = metadata.moduleName+"::"+metadata.algorhitmName+"::"+metadata.variableName;
        metadata.hash =  qHash(qualifedName);
        l_openedChildData[key] = metadata;
        const QString url = QString::fromAscii(
                    "%2/table%1.html"
                    )
                .arg(dimension)
                .arg(QCoreApplication::instance()->property("sharePath").toString()+"/coregui/variableswindow_kumir")
                ;
        QMap<QString,QObject*> objs;
        QUrl uurl = QUrl::fromLocalFile(url);
        uurl.addQueryItem("hash", QString::number(metadata.hash));
        objs["variablesObject"] = this;
        Shared::BrowserComponent bc = m_browser->createBrowser(uurl, objs);
        l_openedChildWindows[key] = bc;

        emit newWindowCreated(bc);
    }
}

void KumirVariablesWebObject::checkUnusedWindows()
{
    Q_ASSERT(l_openedChildWindows.size()==l_openedChildData.size());
    Q_ASSERT(l_openedChildWindows.keys()==l_openedChildData.keys());
    const QStringList keys = l_openedChildData.keys();
    QStringList keysToRemove;
    QMap<QString,Shared::BrowserComponent> openedChildWindows;
    QMap<QString,TableMetadata> openedChildData;
    for (int i=0; i<keys.size(); i++) {
        TableMetadata metadata = l_openedChildData[keys[i]];
        int modId, algId, varId;
        const AST::Variable * var = findVariable(metadata.moduleName,
                                           metadata.algorhitmName,
                                           metadata.variableName,
                                           metadata.global,
                                           modId,
                                           algId,
                                           varId);
        if (var && var->dimension>0) {
            // If found -- update url
            const QString url = QString::fromAscii(
                        "%2/table%1.html"
                        )
                    .arg(var->dimension)
                    .arg(QCoreApplication::instance()->property("sharePath").toString()+"/coregui/variableswindow_kumir");
            QUrl uurl = QUrl::fromLocalFile(url);
            uurl.addQueryItem("hash", QString::number(metadata.hash));

            const QMetaObject * m = l_openedChildWindows[keys[i]].widget->metaObject();
            if (var->dimension!=metadata.dimension) {
                m->invokeMethod(l_openedChildWindows[keys[i]].widget,
                               "go",
                               Qt::QueuedConnection,
                               Q_ARG(QUrl, uurl));
            }
            m->invokeMethod(l_openedChildWindows[keys[i]].widget,
                            "evaluateCommand",
                            Qt::QueuedConnection,
                            Q_ARG(QString, QString::fromAscii("updateIDs")),
                            Q_ARG(QVariantList, QVariantList() << QVariant(modId) << QVariant(algId) << QVariant(varId)));
            const QString newKey = QString::fromAscii(
                        "%1:%2:%3").arg(modId).arg(algId).arg(varId);
            openedChildWindows[newKey] = l_openedChildWindows[keys[i]];
            openedChildData[newKey] = l_openedChildData[keys[i]];
        }
        else {
            // Not found -- remove opened window
            keysToRemove << keys[i];
        }
    }
    for (int i=0; i<keysToRemove.size(); i++) {
        emit windowCloseRequest(l_openedChildWindows[keysToRemove[i]].widget);
    }
    l_openedChildWindows = openedChildWindows;
    l_openedChildData = openedChildData;
}

const AST::Variable * KumirVariablesWebObject::findVariable(
    const QString &moduleName,
    const QString &algName,
    const QString &varName,
    bool global,
    int &modId, int &algId, int &varId) const
{
    for (int m=0; m<l_userModules.size(); m++) {
        AST::Module * mm = l_userModules[m];
        modId = l_userModulesIds[m];
        if (mm->header.name==moduleName) {
            if (global) {
                algId = -1;
                for (varId=0; varId<mm->impl.globals.size(); varId++) {
                    if (mm->impl.globals[varId]->name==varName) {
                        return mm->impl.globals[varId];
                    }
                }
            }
            else {
                for (algId=0; algId<mm->impl.algorhitms.size(); algId++) {
                    AST::Algorhitm * a = mm->impl.algorhitms[algId];
                    if (a->header.name==algName) {
                        for (varId=0; varId<a->impl.locals.size(); varId++) {
                            if (a->impl.locals[varId]->name==varName) {
                                return a->impl.locals[varId];
                            }
                        }
                    }
                }
            }
        }
    }
    modId = algId = varId = -1;
    return 0;
}

QString KumirVariablesWebObject::tableTitle(const QString &hashValue) const
{
    uint hash = hashValue.toUInt();
    QString index;
    foreach (const QString &k, l_openedChildData.keys()) {
        if (l_openedChildData[k].hash==hash) {
            index = k;
            break;
        }
    }

    QString result;
    if (!index.isEmpty()) {
        if (l_openedChildData[index].global) {
            result = tr("Global table \"%1\"").arg(l_openedChildData[index].variableName);
        }
        else if (l_openedChildData[index].algorhitmName.isEmpty()) {
            result = tr("Table \"%1\" from main algorhitm").arg(l_openedChildData[index].variableName);
        }
        else {
            result = tr("Table \"%1\" from algorhitm \"%2\"").arg(l_openedChildData[index].variableName).arg(l_openedChildData[index].algorhitmName);
        }
        if (!l_openedChildData[index].moduleName.isEmpty()) {
            result += tr(" (module \"%1\")").arg(l_openedChildData[index].moduleName);
        }
    }
    else {
        result = tr("Unknown table");
    }
    return result;
}

} // namespace CoreGUI

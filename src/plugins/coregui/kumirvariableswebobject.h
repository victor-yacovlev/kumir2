#ifndef COREGUI_KUMIRVARIABLESWEBOBJECT_H
#define COREGUI_KUMIRVARIABLESWEBOBJECT_H

#include <QtCore>
#include "abstractsyntaxtree/ast.h"
#include "abstractsyntaxtree/ast_variable.h"
#include "interfaces/runinterface.h"
#include "interfaces/browserinterface.h"

namespace CoreGUI {

class KumirVariablesWebObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString rootPage READ rootPage)
public:
    explicit KumirVariablesWebObject(QObject *parent = 0);
    inline void setBrowserPlugin(Shared::BrowserInterface * browser)
    {
        m_browser = browser;
    }


signals:
    void jsRequest(const QString& method, const QVariantList & args);
    void newWindowCreated(const Shared::BrowserComponent & bc);
    void windowRaiseRequest(QWidget * w);
    void windowCloseRequest(QWidget * w);

public slots:
    QString rootPage() const;
    QString modulePage(int index) const;
    QString tableTitle(const QString & hashValue) const;
    int modulesCount() const;
    void reset(const Shared::RunInterface * runner);
    void setProgram(const AST::Data * ast);
    void refreshRoot();
    void test(const QString & aaa);
    void openTableWindow(int dimension, int moduleId, int algorhitmId, int variableId);
    void checkUnusedWindows();
private:
    QString valuesTable(int moduleId, int algId, const QList<AST::Variable*> values) const;
    const AST::Variable * findVariable(
        const QString & moduleName,
        const QString & algName,
        const QString & varName,
        bool global,
        int &modId, int &algId, int &varId
        ) const;
    static QString makeTableValue(int moduleId, int algorhitmId, int variableId, int dimension, const QVariant value);
    static QString makeArr(const QVariant & value);

    QList<AST::Module*> l_userModules;
    QList<int> l_userModulesIds;
    const Shared::RunInterface * m_runner;
    Shared::BrowserInterface * m_browser;
    QMap<QString,Shared::BrowserComponent> l_openedChildWindows;
    struct TableMetadata {
        QString moduleName;
        QString algorhitmName;
        bool global;
        QString variableName;
        int dimension;
        uint hash;
    };
    QMap<QString,TableMetadata> l_openedChildData;
};

} // namespace CoreGUI

#endif // COREGUI_KUMIRVARIABLESWEBOBJECT_H

#include "nameprovider.h"

namespace KumirCppGenerator {

NameProvider::NameProvider(QObject *parent) :
    QObject(parent)
{
}

QString NameProvider::baseTypeName(AST::VariableBaseType type)
{
    switch (type) {
    case AST::TypeNone: return "void";
    case AST::TypeInteger: return "int";
    case AST::TypeReal: return "double";
    case AST::TypeCharect: return "wchar_t";
    case AST::TypeString: return "wchar_t*";
    case AST::TypeBoolean: return "unsigned char";
    default: {
        quint32 index = quint32(type) & quint32(AST::TypeUser);
        return "struct user_"+QString::number(index);
    }
    }
}

QString NameProvider::operatorName(AST::ExpressionOperator op)
{
    switch (op) {
    case AST::OpSumm: return "+";
    case AST::OpSubstract: return "-";
    case AST::OpMultiply: return "*";
    case AST::OpDivision: return "/";
    case AST::OpNot: return "!";
    case AST::OpAnd: return "&&";
    case AST::OpOr: return "||";
    case AST::OpEqual: return "==";
    case AST::OpNotEqual: return "!=";
    case AST::OpLess: return "<";
    case AST::OpLessOrEqual: return "<=";
    case AST::OpGreater: return ">";
    case AST::OpGreaterOrEqual: return ">=";
    default: return "";
    }
}

QString NameProvider::findVariable(
    const QString &modName,
    const QString &algName,
    const QString &varName) const
{
    if (!algName.isEmpty()) {
        const QString key = modName+"::"+algName;
        if (m_database.contains(key) && m_database[key].contains(varName))
            return m_database[key][varName];
    }
    return m_database[modName][varName];
}

QString NameProvider::addName(const QString &name, const QString &ns)
{
    static const QSet<QString> reservedNames
            = QSet<QString>::fromList(QStringList()
                                      << "void"
                                      << "int"
                                      << "unsigned"
                                      << "long"
                                      << "float"
                                      << "double"
                                      << "char"
                                      << "wchar_t"
                                      << "for"
                                      << "if"
                                      << "while"
                                      << "do"
                                      << "case"
                                      << "switch"
                                      << "namespace"
                                      << "main"
                                      << "struct"
                                      << "enum"
                                      << "typedef"
                                      << "static"
                                      << "extern"
                                      << "sizeof"
                                      );
    if (!m_database.contains(ns)) {
        m_database[ns] = QMap<QString,QString>();
    }
    const QString cNameBase = suggestCName(name);
    int index = 1;
    QString cName = cNameBase;
    QMap<QString,QString> nsDb = m_database[ns];
    if (cName.startsWith("__")) {
        cName += "_1";
    }
    while (nsDb.contains(cName) || reservedNames.contains(cName)) {
        cName = cNameBase+"_"+QString::number(index);
        index++;
    }
    m_database[ns][name] = cName;
    return cName;
}

QString NameProvider::suggestCName(const QString &name)
{
    QString result;
    foreach (QChar c, name) {
        QString cc;
        if (c.toLower()==QString::fromUtf8("а")[0]) cc = "a";
        else if (c.toLower()==QString::fromUtf8("б")[0]) cc = "b";
        else if (c.toLower()==QString::fromUtf8("в")[0]) cc = "v";
        else if (c.toLower()==QString::fromUtf8("г")[0]) cc = "g";
        else if (c.toLower()==QString::fromUtf8("д")[0]) cc = "d";
        else if (c.toLower()==QString::fromUtf8("е")[0]) cc = "e";
        else if (c.toLower()==QString::fromUtf8("ё")[0]) cc = "jo";
        else if (c.toLower()==QString::fromUtf8("ж")[0]) cc = "zh";
        else if (c.toLower()==QString::fromUtf8("з")[0]) cc = "z";
        else if (c.toLower()==QString::fromUtf8("и")[0]) cc = "i";
        else if (c.toLower()==QString::fromUtf8("й")[0]) cc = "j";
        else if (c.toLower()==QString::fromUtf8("к")[0]) cc = "k";
        else if (c.toLower()==QString::fromUtf8("л")[0]) cc = "l";
        else if (c.toLower()==QString::fromUtf8("м")[0]) cc = "m";
        else if (c.toLower()==QString::fromUtf8("н")[0]) cc = "n";
        else if (c.toLower()==QString::fromUtf8("о")[0]) cc = "o";
        else if (c.toLower()==QString::fromUtf8("п")[0]) cc = "p";
        else if (c.toLower()==QString::fromUtf8("р")[0]) cc = "r";
        else if (c.toLower()==QString::fromUtf8("с")[0]) cc = "s";
        else if (c.toLower()==QString::fromUtf8("т")[0]) cc = "t";
        else if (c.toLower()==QString::fromUtf8("у")[0]) cc = "u";
        else if (c.toLower()==QString::fromUtf8("ф")[0]) cc = "f";
        else if (c.toLower()==QString::fromUtf8("х")[0]) cc = "h";
        else if (c.toLower()==QString::fromUtf8("ц")[0]) cc = "ts";
        else if (c.toLower()==QString::fromUtf8("ч")[0]) cc = "ch";
        else if (c.toLower()==QString::fromUtf8("ш")[0]) cc = "sh";
        else if (c.toLower()==QString::fromUtf8("щ")[0]) cc = "sch";
        else if (c.toLower()==QString::fromUtf8("э")[0]) cc = "e";
        else if (c.toLower()==QString::fromUtf8("ю")[0]) cc = "ju";
        else if (c.toLower()==QString::fromUtf8("я")[0]) cc = "ja";
        else if (c.toLower()==QString::fromUtf8("ы")[0]) cc = "y";
        else if (c.toLower()==QString::fromUtf8("ь")[0]) cc = "";
        else if (c.toLower()==QString::fromUtf8("ъ")[0]) cc = "";
        else if (c.toLower()==QString::fromUtf8(" ")[0]) cc = "_";
        else cc = c;
        result += cc;
    }
    if (result.isEmpty()) {
        result = "__blank__";
    }
    return result;
}

} // namespace KumirCppGenerator

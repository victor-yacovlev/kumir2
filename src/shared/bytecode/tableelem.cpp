#include "tableelem.h"

namespace Bytecode {

static QString typeToString(ElemType t)
{
    if (t==EL_LOCAL)
        return ".local";
    else if (t==EL_GLOBAL)
        return ".global";
    else if (t==EL_CONST)
        return ".constant";
    else if (t==EL_FUNCTION)
        return ".function";
    else if (t==EL_EXTERN)
        return ".extern";
    else if (t==EL_INIT)
        return ".init";
    else if (t==EL_MAIN)
        return ".main";
    else if (t==EL_TESTING)
        return ".testing";
    else  {
        return "";
    }
}

static ElemType typeFromString(const QString & s)
{
    if (s.toLower()==".local")
        return EL_LOCAL;
    else if (s.toLower()==".global")
        return EL_GLOBAL;
    else if (s.toLower()==".constant")
        return EL_CONST;
    else if (s.toLower()==".function")
        return EL_FUNCTION;
    else if (s.toLower()==".extern")
        return EL_EXTERN;
    else if (s.toLower()==".init")
        return EL_INIT;
    else if (s.toLower()==".main")
        return EL_MAIN;
    else if (s.toLower()==".testing")
        return EL_TESTING;
    else {
        return ElemType(0);
    }
}



static QString vtypeToString(ValueType t)
{
    if (t==VT_int)
        return "int";
    else if (t==VT_float)
        return "float";
    else if (t==VT_char)
        return "char";
    else if (t==VT_string)
        return "string";
    else if (t==VT_bool)
        return "bool";
    else
        return "";
}

static ValueType vtypeFromString(const QString &s)
{
    if (s.toLower()=="int")
        return VT_int;
    else if (s.toLower()=="float")
        return VT_float;
    else if (s.toLower()=="char")
        return VT_char;
    else if (s.toLower()=="string")
        return VT_string;
    else if (s.toLower()=="bool")
        return VT_bool;
    else
        return VT_void;
}

static QString kindToString(ValueKind k)
{
    if (k==VK_Plain)
        return "var";
    else if (k==VK_In)
        return "in";
    else if (k==VK_InOut)
        return "inout";
    else if (k==VK_Out)
        return "out";
    else  {
        qFatal("Unknown type");
        return "";
    }
}

static ValueKind kindFromString(const QString &s)
{
    if (s.toLower()=="in")
        return VK_In;
    else if (s.toLower()=="inout")
        return VK_InOut;
    else if (s.toLower()=="out")
        return VK_Out;
    else
        return VK_Plain;
}

static QString screenString(QString s)
{
    s.replace("\n", "\\n");
    s.replace("\"", "\\\"");
    s.replace(" ", "\\s");
    s.replace("\t", "\\t");
    s.replace("\\", "\\\\");
    return s;
}

static QString unscreenString(QString s)
{
    s.replace("\\n", "\n");
    s.replace("\\\"", "\"");
    s.replace("\\s", " ");
    s.replace("\\t", "\t");
    s.replace("\\\\", "\\");
    return s;
}

extern QTextStream& operator<<(QTextStream& ts, const TableElem& e)
{
    QString s;
    s += typeToString(e.type) + " ";
    if (e.type==EL_LOCAL || e.type==EL_GLOBAL || e.type==EL_CONST) {
        s += vtypeToString(e.vtype) + " ";
        s += QString::number(e.dimension)+" ";
    }
    if (e.type==EL_LOCAL || e.type==EL_GLOBAL)
        s += kindToString(e.refvalue)+" ";

    if (e.type==EL_LOCAL || e.type==EL_GLOBAL || e.type==EL_EXTERN || e.type==EL_FUNCTION || e.type==EL_MAIN || e.type==EL_INIT || e.type==EL_TESTING) {
        s += QString::number(e.module)+" ";
    }
    if (e.type==EL_LOCAL) {
        s += QString::number(e.algId)+" ";
    }
    if (e.type!=EL_INIT)
        s += QString::number(e.id)+" ";
    if (e.type==EL_EXTERN)
        s += e.moduleName;
    else if (e.type==EL_CONST) {
        if (e.constantValue.type()==QVariant::Int)
            s += QString::number(e.constantValue.toInt());
        else if (e.constantValue.type()==QVariant::Double)
            s += QString::number(e.constantValue.toDouble());
        else if (e.constantValue.type()==QVariant::Bool)
            s += e.constantValue.toBool()? "true" : "false";
        else
            s += "\""+screenString(e.constantValue.toString())+"\"";
    }
    else if (e.type==EL_LOCAL||e.type==EL_GLOBAL||e.type==EL_GLOBAL||e.type==EL_FUNCTION||e.type==EL_MAIN||e.type==EL_FUNCTION) {
        s += "\""+screenString(e.name)+"\" ";
    }
    if (e.type==EL_FUNCTION || e.type==EL_MAIN || e.type==EL_TESTING || e.type==EL_INIT) {
        s += QString::number(e.instructions.size());
        for (int i=0; i<e.instructions.size(); i++) {
            s += "\n";
            s += QString::number(i) + ":\t";
            s += instructionToString(e.instructions[i]);
        }
    }
    s += "\n";
    ts << s;
    return ts;
}

extern QTextStream& operator>>(QTextStream& ts, TableElem& e)
{
    e.type = EL_NONE;
    QString header;
    while (!ts.atEnd()) {
        QString line = ts.readLine().trimmed();
        if (!line.isEmpty() && !line.startsWith("#")) {
            header = line;
            break;
        }
    }
    if (header=="") {
        return ts;
    }
    QStringList lexems = header.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    e.type = typeFromString(lexems.takeFirst());
    if (e.type==EL_LOCAL || e.type==EL_GLOBAL || e.type==EL_CONST) {
        e.vtype = vtypeFromString(lexems.takeFirst());
        e.dimension = quint8(lexems.takeFirst().toUShort());
    }
    if (e.type==EL_LOCAL || e.type==EL_GLOBAL)
        e.refvalue = kindFromString(lexems.takeFirst());

    if (e.type==EL_LOCAL || e.type==EL_GLOBAL || e.type==EL_EXTERN || e.type==EL_FUNCTION || e.type==EL_MAIN || e.type==EL_INIT || e.type==EL_TESTING) {
        e.module = quint8(lexems.takeFirst().toUShort());
    }
    if (e.type==EL_LOCAL) {
        e.algId = quint16(lexems.takeFirst().toUInt());
    }
    if (e.type!=EL_INIT)
        e.id = quint16(lexems.takeFirst().toUInt());
    if (e.type==EL_EXTERN)
        e.moduleName = lexems.takeFirst();
    else if (e.type==EL_CONST) {
        QString c = lexems.takeFirst();
        if (e.vtype==VT_int)
            e.constantValue = c.toInt();
        else if (e.vtype==VT_float)
            e.constantValue = c.toDouble();
        else if (e.vtype==VT_bool)
            e.constantValue = bool(c.toLower()=="true"? true : false);
        else if (e.vtype==VT_char)
            e.constantValue = QChar(unscreenString(QString(c.mid(1,c.length()-2)))[0]);
        else if (e.vtype==VT_string)
            e.constantValue = unscreenString(c.mid(1,c.length()-2));
    }
    else if (e.type==EL_LOCAL||e.type==EL_GLOBAL||e.type==EL_GLOBAL||e.type==EL_FUNCTION||e.type==EL_MAIN||e.type==EL_TESTING) {
        QString n = lexems.takeFirst();
        e.name = unscreenString(n.mid(1,n.length()-2));
    }
    if (e.type==EL_FUNCTION || e.type==EL_MAIN || e.type==EL_TESTING || e.type==EL_INIT) {
        int size = lexems.takeFirst().toInt();
        e.instructions = QVector<Instruction>(size);
        int index = 0;
        while (index<size && !ts.atEnd()) {
            QString line = ts.readLine().trimmed();
            if (line.isEmpty() || line.startsWith("#"))
                continue;
            int colonPos = line.indexOf(":");
            line = line.mid(colonPos+1);
            e.instructions[index] = instructionFromString(line);
            index++;
        }
    }
    return ts;
}

extern QDataStream& operator<<(QDataStream & ds, const TableElem &e)
{
    ds << quint8(e.type);
    ds << quint8(e.vtype);
    ds << quint8(e.dimension);
    ds << quint8(e.refvalue);
    ds << quint8(e.module);
    ds << quint16(e.algId);
    ds << quint16(e.id);
    ds << e.name.toUtf8().data();
    if (e.type==EL_EXTERN)
        ds << e.moduleName.toAscii().data();
    else if (e.type==EL_CONST) {
        if (e.constantValue.type()==QVariant::Int)
            ds << qint32(e.constantValue.toInt());
        else if (e.constantValue.type()==QVariant::Double)
            ds << e.constantValue.toDouble();
        else if (e.constantValue.type()==QVariant::Bool)
            ds << quint8(e.constantValue.toBool()? 1 : 0);
        else
            ds << e.constantValue.toString().toUtf8().data();
    }
    else if (e.type==EL_FUNCTION || e.type==EL_MAIN || e.type==EL_TESTING || e.type==EL_INIT) {
        ds << quint16(e.instructions.size());
        for (int i=0; i<e.instructions.size(); i++) {
            ds << toUint32(e.instructions[i]);
        }
    }
    return ds;
}

extern QDataStream& operator>>(QDataStream & ds, TableElem &e)
{
    quint8 t;
    quint8 v;
    quint8 d;
    quint8 r;
    quint8 m;
    quint16 a;
    quint16 id;
    char * s;
    ds >> t >> v >> d >> r >> m >> a >> id >> s;
    e.type = ElemType(t);
    e.vtype = ValueType(v);
    e.dimension = d;
    e.refvalue = ValueKind(r);
    e.module = m;
    e.algId = a;
    e.id = id;
    e.name = QString::fromUtf8(s);
    delete s;
    if (e.type==EL_EXTERN) {
        ds >> s;
        e.moduleName = QString::fromAscii(s);
        delete s;
    }
    else if (e.type==EL_CONST) {
        if (e.vtype==VT_int) {
            qint32 iv;
            ds >> iv;
            e.constantValue = iv;
        }
        else if (e.vtype==VT_float) {
            double fv;
            ds >> fv;
            e.constantValue = fv;
        }
        else if (e.vtype==VT_bool) {
            quint8 bv;
            ds >> bv;
            e.constantValue = bool(bv>0? true : false);
        }
        else {
            ds >> s;
            e.constantValue = QString::fromUtf8(s);
            delete s;
        }
    }
    else if (e.type==EL_FUNCTION || e.type==EL_MAIN || e.type==EL_TESTING || e.type==EL_INIT) {
        quint16 sz;
        ds >> sz;
        e.instructions = QVector<Instruction>(sz);
        for (quint16 i=0; i<sz; i++) {
            quint32 instr;
            ds >> instr;
            e.instructions[i] = fromUint32(instr);
        }
    }
    return ds;
}

} // namespace Bytecode

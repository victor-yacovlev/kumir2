#include "simplepascalsyntaxanalizer.h"

namespace FpcAnalizer {

SimplePascalSyntaxAnalizer* SimplePascalSyntaxAnalizer::create(QObject *parent)
{
    static const QStringList kwds = QStringList()
            << "and"
            << "array"
            << "asm"
            << "begin"
            << "break"
            << "case"
            << "const"
            << "constructor"
            << "continue"
            << "destructor"
            << "div"
            << "do"
            << "downto"
            << "else"
            << "end"
            << "false"
            << "file"
            << "for"
            << "function"
            << "goto"
            << "if"
            << "implementation"
            << "in"
            << "inline"
            << "interface"
            << "label"
            << "mod"
            << "nil"
            << "not"
            << "object"
            << "of"
            << "on"
            << "operator"
            << "or"
            << "packed"
            << "procedure"
            << "program"
            << "record"
            << "repeat"
            << "set"
            << "shl"
            << "shr"
            << "then"
            << "to"
            << "true"
            << "type"
            << "unit"
            << "until"
            << "uses"
            << "var"
            << "while"
            << "with"
            << "xor"
            << "as"
            << "class"
            << "dispose"
            << "except"
            << "exit"
            << "exports"
            << "finalization"
            << "finally"
            << "inherited"
            << "initialization"
            << "is"
            << "library"
            << "new"
            << "on"
            << "out"
            << "property"
            << "raise"
            << "self"
            << "threadvar"
            << "try"
            << "absolute"
            << "abstract"
            << "alias"
            << "assembler"
            << "cdecl"
            << "cppdecl"
            << "default"
            << "export"
            << "external"
            << "forward"
            << "index"
            << "local"
            << "name"
            << "nostackframe"
            << "oldfpccall"
            << "override"
            << "pascal"
            << "private"
            << "protected"
            << "public"
            << "published"
            << "register"
            << "reintroduce"
            << "safecall"
            << "softfloat"
            << "stdcall"
            << "virtual"
    ;
    static const QStringList ops = QStringList()
            << "*" << "/" << "+" << "-" << ":="
            << "(" << ")" << "[" << "]" << "," << ";"
            << "." << "^" << "<" << ">" << "=";
    static const QStringList tps = QStringList()
            << "integer" << "real" << "char" << "string" << "pchar"
            << "boolean" << "word" << "longint";
    static const QString pattern = "\\b" + kwds.join("\\b|\\b") + "\\b" +
            "|\\*|//|/|\\+|\\-|:=|\\(|\\)|\\[|\\]|,|;|\\.|\\^|<|>|=|'|\\{|\\}|\\s+";
    QRegExp rx(pattern);
    rx.setMinimal(false);
    SimplePascalSyntaxAnalizer* self =
            new SimplePascalSyntaxAnalizer(parent, kwds, ops, tps, rx);
    return self;
}



SimplePascalSyntaxAnalizer::SimplePascalSyntaxAnalizer
(QObject *parent, const QStringList& kwds, const QStringList& ops,
 const QStringList & tps, const QRegExp & rx)
    : QObject(parent)
    , Keywords(kwds)
    , Operators(ops)
    , StandardTypes(tps)
    , Delimeters(rx)
{


}

void SimplePascalSyntaxAnalizer::reset()
{
    unitNames_.clear();
    functionNames_.clear();
    lineStartStates_.clear();
    typeNames_.clear();
}

void SimplePascalSyntaxAnalizer::processSyntaxAnalysis(
        const QStringList &lines
        , const QStringList & unitNames
        , const QStringList & functionNames
        , const QStringList & typeNames
        , QList<LineProp> &lineProps
        , QList<QPoint> &lineRanks
        )
{
    reset();
    unitNames_ = unitNames;
    functionNames_ = functionNames;
    typeNames_ = typeNames;
    State currentState = Program;
    for (int i=0; i<lines.size(); i++) {
        const QString line = lines[i].toLower();
        LineProp & lineProp = lineProps[i];
        QPoint lineRank(0,0);
        State nextState;
        processLine(line, currentState, lineProp, lineRank, nextState);
        lineStartStates_.append(currentState);
        lineRanks[i] = lineRank;
        currentState = nextState;
    }
    lineStartStates_.append(currentState);
}

void SimplePascalSyntaxAnalizer::processLineProp(const QString& line,
                     const QStringList & unitNames,
                     const QStringList & functionNames,
                     const QStringList & typeNames,
                     int lineNo,
                     LineProp & lineProp
                     )
{
    State initialState = lineNo < lineStartStates_.size() ? lineStartStates_.at(lineNo) : Program;
    QPoint dummy;
    State dummyState;
    processLine(line, initialState, lineProp, dummy, dummyState);
}

SimplePascalSyntaxAnalizer::Lexem
SimplePascalSyntaxAnalizer::takeLexem
(const QString &line, int startPos, State startState) const
{
    Lexem result;
    if (LineComment == startState) {
        result.start = startPos;
        result.length = line.length() - startPos;
        result.stateAfter = Program;
    }
    else if (BlockComment == startState) {
        int end = line.indexOf('}', startPos);
        if (-1 == end) {
            result.start = startPos;
            result.length = line.length() - startPos;
            result.stateAfter = BlockComment;
        }
        else {
            result.start = startPos;
            result.length = end - startPos;
            result.stateAfter = Program;
        }
    }
    else if (String == startState) {
        int end = line.indexOf('\'', startPos+1);
        if (-1 == end)
            end = line.length();
        result.start = startPos;
        result.length = end - startPos;
        result.stateAfter = Program;
    }
    else if (Program == startState) {
        int end = Delimeters.indexIn(line, startPos+1);
        QString next;
        if (-1 == end)
            end = line.length();
        else
            next = Delimeters.cap();
        result.start = startPos;
        while (result.start < end && line.at(result.start).isSpace()) {
            result.start ++;
        }
        result.length = end - result.start;
        if ("//" == next)
            result.stateAfter = LineComment;
        else if ("{" == next)
            result.stateAfter = BlockComment;
        else if ("'" == next)
            result.stateAfter = String;
        else
            result.stateAfter = Program;
    }
    else {
        qFatal("Unknown state");
    }
    return result;
}

void SimplePascalSyntaxAnalizer::processLine
(const QString &line, const State initialState,
 LineProp &lineProp, QPoint &rank, State &endState) const
{
    State currentState = initialState;
    int currentPos = 0;
    Q_FOREVER {
        Lexem lx = takeLexem(line, currentPos, currentState);
        QString lxText = line.mid(lx.start, lx.length).trimmed();
        LexemType tp = LxTypeName;
        if (String == currentState) {
            tp = LxConstLiteral;
        }
        else if (LineComment == currentState || BlockComment == currentState) {
            tp = LxTypeComment;
        }
        else {
            if (Operators.contains(lxText)) {
                tp = LxTypeOperator;
            }
            else if (Keywords.contains(lxText)) {
                tp = LxTypePrimaryKwd;
                rank += keywordRank(lxText);
            }
            else if (StandardTypes.contains(lxText) || typeNames_.contains(lxText)) {
                tp = LxNameClass;
            }
            else if (functionNames_.contains(lxText)) {
                tp = LxNameAlg;
            }
            else if (unitNames_.contains(lxText)) {
                tp = LxNameModule;
            }
        }
        for (int i=lx.start; i<lx.start+lx.length; i++)
            lineProp[i] = tp;
        currentPos = lx.start + lx.length;
        currentState = lx.stateAfter;
        if (currentPos >= line.length())
            break;
    }
    endState = currentState;
}

QPoint SimplePascalSyntaxAnalizer::keywordRank(const QString &keyword)
{
    if ("procedure" == keyword || "function" == keyword) {
        return QPoint(-1, 1);
    }
    else if ("var" == keyword) {
        return QPoint(-1, 1);
    }
    else if ("type" == keyword) {
        return QPoint(-1, 1);
    }
    else if ("begin" == keyword) {
        return QPoint(-1, 1);
    }
    else if ("do" == keyword) {
        return QPoint(0, 1);
    }
    else if ("then" == keyword || "else" == keyword) {
        return QPoint(0, 1);
    }
    else if ("end" == keyword) {
        return QPoint(-1, 0);
    }
    else {
        return QPoint(0, 0);
    }
}

QString SimplePascalSyntaxAnalizer::correctCapitalization
(const QString &name, LexemType lxType) const
{
    QString result;
    if (lxType == LxTypePrimaryKwd || lxType == LxTypeSecondaryKwd) {
        result = name.toLower();
    }
    else if (lxType == LxNameClass) {
        if ("longint" == name.toLower()) {
            result = "LongInt";
        }
        else if ((name.toLower().startsWith("t") || name.toLower().startsWith("p"))&& name.length() > 1) {
            result.append(name.at(0).toUpper());
            result.append(name.at(1).toUpper());
            result.append(name.mid(2));
        }
        else {
            result.append(name.at(0).toUpper());
            result.append(name.mid(1).toLower());
        }
    }
    else if (lxType == LxNameAlg) {
        if (name.toLower() == "readln") result = "ReadLn";
        else if (name.toLower() == "writeln") result = "WriteLn";
        else if (name.toLower().startsWith("get") && name.length() > 3) {
            result = "Get" + QString(name.at(3).toUpper()) + name.mid(4);
        }
        else if (name.toLower().startsWith("set") && name.length() > 3) {
            result = "Set" + QString(name.at(3).toUpper()) + name.mid(4);
        }
        else if (name.toLower().startsWith("is") && name.length() > 2) {
            result = "Is" + QString(name.at(2).toUpper()) + name.mid(3);
        }
        else if (name.toLower().startsWith("do") && name.length() > 2) {
            result = "Do" + QString(name.at(2).toUpper()) + name.mid(3);
        }
        else {
            result = name.at(0).toUpper() + name.mid(1);
        }
    }
    else if (lxType == LxNameModule) {
        result.append(name.at(0).toUpper());
        result.append(name.mid(1));
    }
    else {
        result = name;
    }
    return result;
}

} // namespace FpcAnalizer

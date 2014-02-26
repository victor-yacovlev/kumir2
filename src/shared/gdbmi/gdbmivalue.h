/****************************************************************************
**
** Some parts of code taken from QtCreator 2.8.1 by Digia Plc
**
** Licensed under the terms of GNU Lesser GPL Licensee 2.1
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
****************************************************************************/

#ifndef GDBMIVALUE_H
#define GDBMIVALUE_H

#include <QByteArray>
#include <QList>
#include <QString>
#include <QVariant>


namespace GdbMi {

class GdbMiValue
{
public:
    GdbMiValue() : m_type(Invalid) {}

    QByteArray m_name;
    QByteArray m_data;
    QList<GdbMiValue> m_children;

    enum Type {
        Invalid,
        Const,
        Tuple,
        List
    };

    Type m_type;

    inline Type type() const { return m_type; }
    inline QByteArray name() const { return m_name; }
    inline bool hasName(const char *name) const { return m_name == name; }

    inline bool isValid() const { return m_type != Invalid; }
    inline bool isConst() const { return m_type == Const; }
    inline bool isTuple() const { return m_type == Tuple; }
    inline bool isList() const { return m_type == List; }


    inline QByteArray data() const { return m_data; }
    inline const QList<GdbMiValue> &children() const { return m_children; }
    inline int childCount() const { return m_children.size(); }

    const GdbMiValue &childAt(int index) const { return m_children[index]; }
    GdbMiValue &childAt(int index) { return m_children[index]; }
    GdbMiValue operator[](const char *name) const;

    QByteArray toString(bool multiline = false, int indent = 0) const;
    qulonglong toAddress() const;
    int toInt() const { return m_data.toInt(); }
    QString toUtf8() const { return QString::fromUtf8(m_data); }
    QString toLatin1() const { return QString::fromLatin1(m_data); }
    void fromString(const QByteArray &str);
    void fromStringMultiple(const QByteArray &str);

private:
    friend class MiDebuggerPrivate;

    static QByteArray parseCString(const char *&from, const char *to);
    static QByteArray escapeCString(const QByteArray &ba);
    void parseResultOrValue(const char *&from, const char *to);
    void parseValue(const char *&from, const char *to);
    void parseTuple(const char *&from, const char *to);
    void parseTuple_helper(const char *&from, const char *to);
    void parseList(const char *&from, const char *to);

    void dumpChildren(QByteArray *str, bool multiline, int indent) const;
};

}


#endif // GDBMIVALUE_H

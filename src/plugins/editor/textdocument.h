#ifndef TEXTBLOCKDATA_H
#define TEXTBLOCKDATA_H

#include <QtCore>

#include "interfaces/lexemtype.h"

namespace Editor {

struct TextLine
{
    int indentStart;
    int indentEnd;
    QList<Shared::LexemType> highlight;
    QString text;
};

class TextDocument:
        public QList<TextLine>
{
public:
    inline int indentAt(int lineNo) const
    {
        int result = 0;
        for (int i=0; i<qMin(lineNo, size()); i++) {
            result += at(i).indentStart + at(i).indentEnd;
        }
        if (lineNo < size()) {
            result += at(lineNo).indentStart;
        }
        return qMax(result, 0);
    }
    inline void setPlainText(const QString &text)
    {
        QStringList lines = text.split("\n");
        foreach (const QString &line, lines) {
            TextLine textLine;
            textLine.text = line;
            textLine.indentStart = 0;
            textLine.indentEnd = 0;
            append(textLine);
        }
    }
};


}

#endif // TEXTBLOCKDATA_H

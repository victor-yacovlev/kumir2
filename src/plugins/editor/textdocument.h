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
    QList<bool> selected;
    bool lineEndSelected;
    QString text;
    QStringList errors;
};

class TextDocument:
        public QList<TextLine>
{
public:
    int documentId;
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
            textLine.lineEndSelected = false;
            for (int j=0; j<line.length(); j++) {
                textLine.highlight << Shared::LxTypeEmpty;
                textLine.selected << false;
            }
            append(textLine);
        }
    }
};


}

#endif // TEXTBLOCKDATA_H

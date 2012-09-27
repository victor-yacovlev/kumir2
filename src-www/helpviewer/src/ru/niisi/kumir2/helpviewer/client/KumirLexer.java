package ru.niisi.kumir2.helpviewer.client;

import com.google.gwt.regexp.shared.MatchResult;
import com.google.gwt.regexp.shared.RegExp;

import java.util.LinkedList;
import java.util.List;


public class KumirLexer {
    static class Lexem {
        String data = "";
        Type type = Type.Plain;
        Lexem(final String data, final Type type) {
            this.data = data; this.type = type;
        }
        enum Type {
            Plain,
            Operator,
            Keyword,
            TypeName,
            LiteralConstant,
            Constant,
            AlgName,
            ModuleName,
            Comment,
            CommentFirstSymbol,
            DocString
            };
    }
    private static final String[] keywords = {
            "исп", "кон_исп", "пауза", "стоп", "использовать", "знач", "алг", "нач", "кон",
            "дано", "надо", "утв", "нц", "кц", "при", "кц_при", "для", "пока", "раз", "от", "до",
            "шаг", "выход", "если", "то", "иначе", "все", "выбор", "ввод", "вывод",
            "нс", "и", "или", "арг", "рез", "аргрез"
    };
    private static final RegExp rxKeywords = RegExp.compile("\\b"+join(keywords, "\\b|\\b")+"\\b");
    private static final String[] constants = {
            "да", "нет"
    };
    private static final RegExp rxConstants = RegExp.compile("\\b"+join(constants, "\\b|\\b")+"\\b|[0-9]+(.[0-9])*");
    private static final String[] typenames = {
            "цел", "вещ", "лог", "лит", "сим",
            "цел\\s*таб", "вещ\\s*таб","лит\\s*таб","сим\\s*таб","лог\\s*таб"
    };
    private static final RegExp rxTypenames = RegExp.compile("\\b"+join(typenames, "\\b|\\b")+"\\b");
    private static final String[] operators = {
            "\\+", "-", "\\*\\*", "\\(", "\\)", "\\[", "\\]", ",", ":",
            "\\*", ">=", "<=", "<>", "<", ">", "/", ";", "#", ":=", "\"", "'", "\\|"
    };
    private static final RegExp rxOperators = RegExp.compile(join(operators, "|"));
    private static final RegExp rxDelimiters = RegExp.compile(
            rxKeywords.getSource()+"|"+
            rxConstants.getSource()+"|"+
            rxTypenames.getSource()+"|"+
            rxOperators.getSource()+"|\\s+"
    );

    private static String join(final String[] strings, String delimiter) {
        StringBuilder builder;
        builder = new StringBuilder();
        for (int i=0; i<strings.length; i++) {
            if (i>0)
                builder.append(delimiter);
            builder.append(strings[i]);
        }
        return builder.toString();
    }
    private List<Lexem> splitLineIntoLexems(final String line) {
        List<Lexem> lexems = new LinkedList<Lexem>();
        boolean inLit = false;
        boolean inComment = false;
        char litSymb = '\0';
        if (line.trim().isEmpty())
            return lexems;
        final List<String> chunks = splitLineIntoChunks(line);
        // 1. Make correct "split" taking into consideration literal constants and comments
        for (int i=0; i<chunks.size(); i++) {
            final String chunk = chunks.get(i);
            if (inComment) {
                lexems.get(lexems.size()-1).data += chunk;
            }
            else if (inLit) {
                if (chunk.equals("\"") || chunk.equals("'") && chunk.charAt(0)==litSymb) {
                    inLit = false;
                }
                else {
                    lexems.get(lexems.size()-1).data += chunk;
                }
            }
            else {
                if (chunk.equals("|")) {
                    Lexem lx = new Lexem(chunk, Lexem.Type.CommentFirstSymbol);
                    lexems.add(lx);
                    lx = new Lexem("", Lexem.Type.Comment);
                    lexems.add(lx);
                    inComment = true;
                }
                else if (chunk.equals("#")) {
                    Lexem lx = new Lexem(chunk, Lexem.Type.DocString);
                    lexems.add(lx);
                    inComment = true;
                }
                else if (chunk.equals("\"") || chunk.equals("'")) {
                    litSymb = chunk.charAt(0);
                    Lexem lx = new Lexem(chunk, Lexem.Type.LiteralConstant);
                    lexems.add(lx);
                    inLit = true;
                }
                else {
                    Lexem lx = new Lexem(chunk, Lexem.Type.Plain);
                    lexems.add(lx);
                }
            }
        }
        // 2. Check for ordinal lexem types
        boolean firstIsAlg = false;
        boolean firstIsUse = false;
        boolean firstIsIsp = false;
        for ( int i=0; i<lexems.size(); i++ ) {
            Lexem lx = lexems.get(i);
            if (lx.type== Lexem.Type.Plain) {
                if (rxOperators.test(lx.data)) {
                    lx.type = Lexem.Type.Operator;
                }
                else if (test(lx.data, typenames)) {
                    lx.type = Lexem.Type.TypeName;
                }
                else if (test(lx.data, keywords)) {
                    lx.type = Lexem.Type.Keyword;
                }
                else if (test(lx.data, constants)) {
                    lx.type = Lexem.Type.Constant;
                }
            }
            if (i==0 && lx.type== Lexem.Type.Keyword) {
                if (lx.data.equals("алг"))
                    firstIsAlg = true;
                else if (lx.data.equals("исп"))
                    firstIsIsp = true;
                else if (lx.data.equals("использовать"))
                    firstIsUse = true;
            }
            else if (lx.type== Lexem.Type.Plain) {
                if (firstIsAlg)
                    lx.type = Lexem.Type.AlgName;
                else if (firstIsIsp || firstIsUse)
                    lx.type = Lexem.Type.ModuleName;
            }
            else if (lx.type== Lexem.Type.Operator) {
                firstIsAlg = firstIsIsp = firstIsUse = false;
            }
        }
        return lexems;
    }

    static boolean test(final String text, final String[] items) {
        for (final String item : items) {
            if (text.equals(item))
                return true;
        }
        return false;
    }

    public String formatKumirProgram(final String programText) {
        final String[] lines = programText.split("\n");
        StringBuilder builder = new StringBuilder();
        for (int i=0; i<lines.length; i++) {
            if (i>0) builder.append("\n");
            List<Lexem> lexems = splitLineIntoLexems(lines[i].trim());
            for (final Lexem lx : lexems) {
                if (lx.type== Lexem.Type.Plain)
                    builder.append(lx.data);
                else {
                    final String cssClass = lx.type.name().toLowerCase();
                    builder.append("<span class=\""+cssClass+"\">");
                    builder.append(lx.data);
                    builder.append("</span>");
                }
            }
        }
        return builder.toString();
    }

    private static List<String> splitLineIntoChunks(String line) {
        List<String> result = new LinkedList<String>();
        rxDelimiters.setLastIndex(0);
        for (;;) {
            MatchResult match = rxDelimiters.exec(line);
            if (match==null) {
                final String last = line;
                result.add(last);
                break;
            }
            else {
                int curPos = match.getIndex();
                final String prev = line.substring(0, curPos);
                if (!prev.isEmpty())
                    result.add(prev);
                final String cur = match.getGroup(0);
                result.add(cur);
                line = line.substring(prev.length()+cur.length());
            }
        }
        return result;
    }

}

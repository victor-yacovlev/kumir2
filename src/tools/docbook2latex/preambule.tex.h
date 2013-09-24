#ifndef PREAMBULE_TEX_H
#define PREAMBULE_TEX_H

namespace docbook2latex {

const char * PREAMBULE = ""
        "\\documentclass[12pt,%%paper%%]{%%class%%}\n"
        "\\usepackage[%%paper%%]{geometry}\n"
        "\\geometry{verbose,tmargin=2.5cm,bmargin=2.5cm,lmargin=3.5cm,rmargin=2.5cm}\n"
        "\\usepackage[koi8-r]{inputenc}\n"
        "\\usepackage[T1,T2A]{fontenc}\n"
        "\\usepackage[russian]{babel}\n"
        "\\usepackage{indentfirst}\n"
        "\\usepackage[pdftex,bookmarks=true,bookmarksnumbered=true,bookmarksopen=true]{hyperref}\n\n"
        "\\usepackage[pdftex]{color}\n"
        "\\usepackage{titlesec}\n"
        "\\usepackage{textcase}\n"
        "\\usepackage{fancyhdr}\n"
        "\\usepackage{listings}\n"
        "\\usepackage{xparse}\n"
        "\\usepackage{framed}\n"
        "\\usepackage{float}\n"
        "\\pagestyle{fancy}\n"
        "\\renewcommand{\\headrulewidth}{0pt}\n"
        "\\renewcommand\\lq{<<}\n"
        "\\renewcommand\\rq{>>}\n"
        "\\titleformat{\\chapter}{\\normalfont\\bfseries\\centering}{\\hspace{1.5em}\\MakeUppercase{\\chaptertitlename}\\ \\thechapter.}{-5.5em}{\\newline\\filcenter\\MakeUppercase}\n"
        "\\titleformat{\\section}{\\normalfont\\bfseries\\centering}{\\thesection.}{0.25em}{\\filcenter\\MakeUppercase}\n"
        "\\titleformat{\\subsection}{\\normalfont\\bfseries}{\\thesubsection.}{0.25em}{}\n"
        "\\lstdefinelanguage{kumir}{"
        "   morekeywords={алг,нач,кон,дано,надо,если,то,иначе,все,выбор,при,исп,кон_исп,нц,кц,кц_при,для,от,до,шаг,раз,пока,ввод,вывод,пауза,утв,арг,рез,аргрез,и,или,не,цел,вещ,лог,лит,сим,целтаб,вещтаб,логтаб,литтаб,симтаб,таб,нс,знач,да,нет,выход},"
        "   sensitive=true,"
        "   morecomment=[l]{\\|},"
        "   morestring=[b]\","
        "   morestring=[b]'"
        "}\n"
        "\\newcounter{example}\n"
        "\\setcounter{example}{1}\n"
        "\\NewDocumentEnvironment{example}{m}{"
        "\\begin{samepage}"
        "\\begin{framed}"
        "}{"
        "\\end{framed}"
        "\\vspace{-12pt}\\noindent\\begin{center}\\bfseries{Пример \\thechapter.\\theexample. }\\normalfont#1\\end{center}"
        "\\end{samepage}"
        "\\stepcounter{example}"
//        "\\vfill"
        "}\n"
        "";

const char * DOCUMENT_BEGIN = ""
        "\\titlespacing{\\chapter}{0pt}{0pt}{72pt}\n"
        "\\titlespacing{\\section}{0pt}{36pt}{18pt}\n"
        "\\titlespacing{\\subsection}{0pt}{36pt}{18pt}\n"
        "\\selectlanguage{russian}\n"
        "\\lhead{}\n"
        "\\chead{--\\ \\thepage\\ --}\n"
        "\\rhead{}\n"
        "\\lfoot{}\n"
        "\\cfoot{}\n"
        "\\rfoot{}\n"

        "";

}

#endif // PREAMBULE_TEX_H

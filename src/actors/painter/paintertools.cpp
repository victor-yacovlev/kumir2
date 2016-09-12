#include "paintertools.h"

namespace ActorPainter {

extern QColor parseColor(const QString &css)
{
    QColor result = QColor::Invalid;
    QString normCSS = css.toLower().remove(QRegExp("\\s"));
    QRegExp rxCSS("(rgb|rgba|cmyk|cmyka|hsl|hsla|hsv|hsva)\\((.*)\\)");
    QMap<QString,QString> ruNames;
    ruNames[QString::fromUtf8("белый")] = "white";
    ruNames[QString::fromUtf8("черный")] = "black";
    ruNames[QString::fromUtf8("чёрный")] = "black";
    ruNames[QString::fromUtf8("серый")] = "gray";
    ruNames[QString::fromUtf8("фиолетовый")] = "magenta";
    ruNames[QString::fromUtf8("синий")] = "blue";
    ruNames[QString::fromUtf8("голубой")] = "lightblue";
    ruNames[QString::fromUtf8("зелёный")] = "lime";
    ruNames[QString::fromUtf8("зеленый")] = "lime";
    ruNames[QString::fromUtf8("жёлтый")] = "yellow";
    ruNames[QString::fromUtf8("желтый")] = "yellow";
    ruNames[QString::fromUtf8("оранжевый")] = "orange";
    ruNames[QString::fromUtf8("красный")] = "red";

    if (normCSS.startsWith("#")) {
        result = QColor(css); // Qt supports HTML-colors
    }
    else if (normCSS.isEmpty() ||
             normCSS==QString::fromUtf8("прозрачный") ||
             normCSS==QString::fromUtf8("transparent")
             ) {
        result = QColor(0,0,0,0);
    }
    else if (rxCSS.exactMatch(normCSS)) {
        const QString model = rxCSS.cap(1);
        const QStringList components = rxCSS.cap(2).split(",");
        bool ok = true;
        foreach (const QString &comp, components) {
            if (ok) {
                comp.toInt(&ok);
            }
        }
        if (ok) {
            if (model.length()!=components.size()) {
                result = QColor::Invalid;
            }
            else if (model=="rgb") {
                result.setRgb(components[0].toInt(), components[1].toInt(), components[2].toInt());
            }
            else if (model=="rgba") {
                result.setRgb(components[0].toInt(), components[1].toInt(), components[2].toInt(), components[3].toInt());
            }
            else if (model=="hsl") {
                result.setHsl(components[0].toInt(), components[1].toInt(), components[2].toInt());
            }
            else if (model=="hsla") {
                result.setHsl(components[0].toInt(), components[1].toInt(), components[2].toInt(), components[3].toInt());
            }
            else if (model=="hsv") {
                result.setHsv(components[0].toInt(), components[1].toInt(), components[2].toInt());
            }
            else if (model=="hsva") {
                result.setHsv(components[0].toInt(), components[1].toInt(), components[2].toInt(), components[3].toInt());
            }
            else if (model=="cmyk") {
                result.setCmyk(components[0].toInt(), components[1].toInt(), components[2].toInt(), components[3].toInt());
            }
            else if (model=="cmyka") {
                result.setCmyk(components[0].toInt(), components[1].toInt(), components[2].toInt(), components[3].toInt(), components[4].toInt());
            }
        }
        else {
            result = QColor::Invalid;
        }
    }
    else if (ruNames.contains(normCSS)) {
        result = QColor(ruNames[normCSS]);
    }
    else {
        result = QColor(normCSS);
    }
    return result;
}

extern QString CSS_RGB_toString(const QString &cssRgb)
{
    QMap<QString,QString> rgb_name;
    rgb_name["rgb(240,248,255)"] = "aliceblue";
    rgb_name["rgb(250,235,215)"] = "antiquewhite";
    rgb_name["rgb(0,255,255)"] = "aqua";
    rgb_name["rgb(127,255,212)"] = "aquamarine";
    rgb_name["rgb(240,255,255)"] = "azure";
    rgb_name["rgb(245,245,220)"] = "beige";
    rgb_name["rgb(255,228,196)"] = "bisque";
    rgb_name["rgb(0,0,0)"] = "black";
    rgb_name["rgb(255,235,205)"] = "blanchedalmond";
    rgb_name["rgb(0,0,255)"] = "blue";
    rgb_name["rgb(138,43,226)"] = "blueviolet";
    rgb_name["rgb(165,42,42)"] = "brown";
    rgb_name["rgb(222,184,135)"] = "burlywood";
    rgb_name["rgb(95,158,160)"] = "cadetblue";
    rgb_name["rgb(127,255,0)"] = "chartreuse";
    rgb_name["rgb(210,105,30)"] = "chocolate";
    rgb_name["rgb(255,127,80)"] = "coral";
    rgb_name["rgb(100,149,237)"] = "cornflowerblue";
    rgb_name["rgb(255,248,220)"] = "cornsilk";
    rgb_name["rgb(220,20,60)"] = "crimson";
    rgb_name["rgb(0,255,255)"] = "cyan";
    rgb_name["rgb(0,0,139)"] = "darkblue";
    rgb_name["rgb(0,139,139)"] = "darkcyan";
    rgb_name["rgb(184,134,11)"] = "darkgoldenrod";
    rgb_name["rgb(169,169,169)"] = "darkgray";
    rgb_name["rgb(0,100,0)"] = "darkgreen";
    rgb_name["rgb(169,169,169)"] = "darkgrey";
    rgb_name["rgb(189,183,107)"] = "darkkhaki";
    rgb_name["rgb(139,0,139)"] = "darkmagenta";
    rgb_name["rgb(85,107,47)"] = "darkolivegreen";
    rgb_name["rgb(255,140,0)"] = "darkorange";
    rgb_name["rgb(153,50,204)"] = "darkorchid";
    rgb_name["rgb(139,0,0)"] = "darkred";
    rgb_name["rgb(233,150,122)"] = "darksalmon";
    rgb_name["rgb(143,188,143)"] = "darkseagreen";
    rgb_name["rgb(72,61,139)"] = "darkslateblue";
    rgb_name["rgb(47,79,79)"] = "darkslategray";
    rgb_name["rgb(47,79,79)"] = "darkslategrey";
    rgb_name["rgb(0,206,209)"] = "darkturquoise";
    rgb_name["rgb(148,0,211)"] = "darkviolet";
    rgb_name["rgb(255,20,147)"] = "deeppink";
    rgb_name["rgb(0,191,255)"] = "deepskyblue";
    rgb_name["rgb(105,105,105)"] = "dimgray";
    rgb_name["rgb(105,105,105)"] = "dimgrey";
    rgb_name["rgb(30,144,255)"] = "dodgerblue";
    rgb_name["rgb(178,34,34)"] = "firebrick";
    rgb_name["rgb(255,250,240)"] = "floralwhite";
    rgb_name["rgb(34,139,34)"] = "forestgreen";
    rgb_name["rgb(255,0,255)"] = "fuchsia";
    rgb_name["rgb(220,220,220)"] = "gainsboro";
    rgb_name["rgb(248,248,255)"] = "ghostwhite";
    rgb_name["rgb(255,215,0)"] = "gold";
    rgb_name["rgb(218,165,32)"] = "goldenrod";
    rgb_name["rgb(128,128,128)"] = "gray";
    rgb_name["rgb(128,128,128)"] = "grey";
    rgb_name["rgb(0,128,0)"] = "green";
    rgb_name["rgb(173,255,47)"] = "greenyellow";
    rgb_name["rgb(240,255,240)"] = "honeydew";
    rgb_name["rgb(255,105,180)"] = "hotpink";
    rgb_name["rgb(205,92,92)"] = "indianred";
    rgb_name["rgb(75,0,130)"] = "indigo";
    rgb_name["rgb(255,255,240)"] = "ivory";
    rgb_name["rgb(240,230,140)"] = "khaki";
    rgb_name["rgb(230,230,250)"] = "lavender";
    rgb_name["rgb(255,240,245)"] = "lavenderblush";
    rgb_name["rgb(124,252,0)"] = "lawngreen";
    rgb_name["rgb(255,250,205)"] = "lemonchiffon";
    rgb_name["rgb(173,216,230)"] = "lightblue";
    rgb_name["rgb(240,128,128)"] = "lightcoral";
    rgb_name["rgb(224,255,255)"] = "lightcyan";
    rgb_name["rgb(250,250,210)"] = "lightgoldenrodyellow";
    rgb_name["rgb(211,211,211)"] = "lightgray";
    rgb_name["rgb(144,238,144)"] = "lightgreen";
    rgb_name["rgb(211,211,211)"] = "lightgrey";
    rgb_name["rgb(255,182,193)"] = "lightpink";
    rgb_name["rgb(255,160,122)"] = "lightsalmon";
    rgb_name["rgb(32,178,170)"] = "lightseagreen";
    rgb_name["rgb(135,206,250)"] = "lightskyblue";
    rgb_name["rgb(119,136,153)"] = "lightslategray";
    rgb_name["rgb(119,136,153)"] = "lightslategrey";
    rgb_name["rgb(176,196,222)"] = "lightsteelblue";
    rgb_name["rgb(255,255,224)"] = "lightyellow";
    rgb_name["rgb(0,255,0)"] = "lime";
    rgb_name["rgb(50,205,50)"] = "limegreen";
    rgb_name["rgb(250,240,230)"] = "linen";
    rgb_name["rgb(255,0,255)"] = "magenta";
    rgb_name["rgb(128,0,0)"] = "maroon";
    rgb_name["rgb(102,205,170)"] = "mediumaquamarine";
    rgb_name["rgb(0,0,205)"] = "mediumblue";
    rgb_name["rgb(186,85,211)"] = "mediumorchid";
    rgb_name["rgb(147,112,219)"] = "mediumpurple";
    rgb_name["rgb(60,179,113)"] = "mediumseagreen";
    rgb_name["rgb(123,104,238)"] = "mediumslateblue";
    rgb_name["rgb(0,250,154)"] = "mediumspringgreen";
    rgb_name["rgb(72,209,204)"] = "mediumturquoise";
    rgb_name["rgb(199,21,133)"] = "mediumvioletred";
    rgb_name["rgb(25,25,112)"] = "midnightblue";
    rgb_name["rgb(245,255,250)"] = "mintcream";
    rgb_name["rgb(255,228,225)"] = "mistyrose";
    rgb_name["rgb(255,228,181)"] = "moccasin";
    rgb_name["rgb(255,222,173)"] = "navajowhite";
    rgb_name["rgb(0,0,128)"] = "navy";
    rgb_name["rgb(253,245,230)"] = "oldlace";
    rgb_name["rgb(128,128,0)"] = "olive";
    rgb_name["rgb(107,142,35)"] = "olivedrab";
    rgb_name["rgb(255,165,0)"] = "orange";
    rgb_name["rgb(255,69,0)"] = "orangered";
    rgb_name["rgb(218,112,214)"] = "orchid";
    rgb_name["rgb(238,232,170)"] = "palegoldenrod";
    rgb_name["rgb(152,251,152)"] = "palegreen";
    rgb_name["rgb(175,238,238)"] = "paleturquoise";
    rgb_name["rgb(219,112,147)"] = "palevioletred";
    rgb_name["rgb(255,239,213)"] = "papayawhip";
    rgb_name["rgb(255,218,185)"] = "peachpuff";
    rgb_name["rgb(205,133,63)"] = "peru";
    rgb_name["rgb(255,192,203)"] = "pink";
    rgb_name["rgb(221,160,221)"] = "plum";
    rgb_name["rgb(176,224,230)"] = "powderblue";
    rgb_name["rgb(128,0,128)"] = "purple";
    rgb_name["rgb(255,0,0)"] = "red";
    rgb_name["rgb(188,143,143)"] = "rosybrown";
    rgb_name["rgb(65,105,225)"] = "royalblue";
    rgb_name["rgb(139,69,19)"] = "saddlebrown";
    rgb_name["rgb(250,128,114)"] = "salmon";
    rgb_name["rgb(244,164,96)"] = "sandybrown";
    rgb_name["rgb(46,139,87)"] = "seagreen";
    rgb_name["rgb(255,245,238)"] = "seashell";
    rgb_name["rgb(160,82,45)"] = "sienna";
    rgb_name["rgb(192,192,192)"] = "silver";
    rgb_name["rgb(135,206,235)"] = "skyblue";
    rgb_name["rgb(106,90,205)"] = "slateblue";
    rgb_name["rgb(112,128,144)"] = "slategray";
    rgb_name["rgb(112,128,144)"] = "slategrey";
    rgb_name["rgb(255,250,250)"] = "snow";
    rgb_name["rgb(0,255,127)"] = "springgreen";
    rgb_name["rgb(70,130,180)"] = "steelblue";
    rgb_name["rgb(210,180,140)"] = "tan";
    rgb_name["rgb(0,128,128)"] = "teal";
    rgb_name["rgb(216,191,216)"] = "thistle";
    rgb_name["rgb(255,99,71)"] = "tomato";
    rgb_name["rgb(64,224,208)"] = "turquoise";
    rgb_name["rgb(238,130,238)"] = "violet";
    rgb_name["rgb(245,222,179)"] = "wheat";
    rgb_name["rgb(255,255,255)"] = "white";
    rgb_name["rgb(245,245,245)"] = "whitesmoke";
    rgb_name["rgb(255,255,0)"] = "yellow";
    rgb_name["rgb(154,205,50)"] = "yellowgreen";


    QMap<QString, QString> ruNames;
    ruNames[QString::fromUtf8("white")] = QString::fromUtf8("белый");
    ruNames[QString::fromUtf8("black")] = QString::fromUtf8("чёрный");
    ruNames[QString::fromUtf8("gray")] = QString::fromUtf8("серый");
    ruNames[QString::fromUtf8("magenta")] = QString::fromUtf8("фиолетовый");
    ruNames[QString::fromUtf8("blue")] = QString::fromUtf8("синий");
    ruNames[QString::fromUtf8("lightblue")] = QString::fromUtf8("голубой");
    ruNames[QString::fromUtf8("lime")] = QString::fromUtf8("зелёный");
    ruNames[QString::fromUtf8("yellow")] = QString::fromUtf8("жёлтый");
    ruNames[QString::fromUtf8("orange")] = QString::fromUtf8("оранжевый");
    ruNames[QString::fromUtf8("red")] = QString::fromUtf8("красный");


    QString result;
    if (rgb_name.contains(cssRgb)) {
        result = rgb_name[cssRgb];
        if (ruNames.contains(result))
            result = ruNames[result];
    }
    return result;
}

const QStringList & standardRussianColorNames()
{
    static QStringList validValues;
    if (validValues.isEmpty()) {
        validValues << QString::fromUtf8("прозрачный");
        validValues << QString::fromUtf8("белый");
        validValues << QString::fromUtf8("черный");
        validValues << QString::fromUtf8("чёрный");
        validValues << QString::fromUtf8("серый");
        validValues << QString::fromUtf8("фиолетовый");
        validValues << QString::fromUtf8("синий");
        validValues << QString::fromUtf8("голубой");
        validValues << QString::fromUtf8("зелёный");
        validValues << QString::fromUtf8("зеленый");
        validValues << QString::fromUtf8("жёлтый");
        validValues << QString::fromUtf8("желтый");
        validValues << QString::fromUtf8("оранжевый");
        validValues << QString::fromUtf8("красный");
    }
    return validValues;
}

QString findColorName(const Actor_Colorer::Color &color)
{
    const QStringList & validValues = standardRussianColorNames();
    Q_FOREACH(const QString & name, validValues) {
        const QColor test = parseColor(name).toRgb();
        bool r = test.red() == color.r;
        bool g = test.green() == color.g;
        bool b = test.blue() == color.b;
        if (r && g && b) {
            return name;
        }
    }
    return QString("rgb(%1,%2,%3)").arg(color.r).arg(color.g).arg(color.b);
}

}

#include "nametranslator.h"

#include <cstdint>

namespace LLVMCodeGenerator {

NameTranslator::NameTranslator()
{
}

void NameTranslator::reset()
{
    _.clear();
    Namespace root;
    _.push_back(root);
}

CString NameTranslator::add(const QString &name)
{
    const QString baseCname = suggestName(name);
    QString cname = baseCname;

    uint16_t counter = 1;
    while (isReservedName(cname) || !find(cname).empty()) {
        cname = QString("%1_%2").arg(baseCname).arg(counter);
    }

    CString result = cname.toStdString();
    ns().rmap.insert(result, name);
    ns().map.insert(name, result);
    return result;
}

void NameTranslator::beginNamespace()
{
    Namespace newNS;
    _.push_back(newNS);
}


void NameTranslator::endNamespace()
{
    _.pop_back();
}

CString NameTranslator::find(const QString &name) const
{
    int nsIndex = _.size() - 1;
    CString result;
    while (nsIndex >= 0) {
        const Namespace &ns = _.at(nsIndex);
        if (ns.map.contains(name)) {
            result = ns.map[name];
            break;
        }
        nsIndex --;
    }
    return result;
}

NameTranslator::Namespace& NameTranslator::ns()
{
    return _.back();
}

QString NameTranslator::suggestName(const QString &x) const
{
    QString result;
    static QMap<QChar,QString> RusToLat;
    if (RusToLat.isEmpty()) {
        RusToLat[QString::fromUtf8("а").at(0)] = "a";
        RusToLat[QString::fromUtf8("б").at(0)] = "b";
        RusToLat[QString::fromUtf8("в").at(0)] = "v";
        RusToLat[QString::fromUtf8("г").at(0)] = "g";
        RusToLat[QString::fromUtf8("д").at(0)] = "d";
        RusToLat[QString::fromUtf8("е").at(0)] = "e";
        RusToLat[QString::fromUtf8("ё").at(0)] = "yo";
        RusToLat[QString::fromUtf8("ж").at(0)] = "zh";
        RusToLat[QString::fromUtf8("з").at(0)] = "z";
        RusToLat[QString::fromUtf8("и").at(0)] = "i";
        RusToLat[QString::fromUtf8("й").at(0)] = "j";
        RusToLat[QString::fromUtf8("к").at(0)] = "k";
        RusToLat[QString::fromUtf8("л").at(0)] = "l";
        RusToLat[QString::fromUtf8("м").at(0)] = "m";
        RusToLat[QString::fromUtf8("н").at(0)] = "n";
        RusToLat[QString::fromUtf8("о").at(0)] = "o";
        RusToLat[QString::fromUtf8("п").at(0)] = "p";
        RusToLat[QString::fromUtf8("р").at(0)] = "r";
        RusToLat[QString::fromUtf8("с").at(0)] = "s";
        RusToLat[QString::fromUtf8("т").at(0)] = "t";
        RusToLat[QString::fromUtf8("у").at(0)] = "u";
        RusToLat[QString::fromUtf8("ф").at(0)] = "f";
        RusToLat[QString::fromUtf8("х").at(0)] = "h";
        RusToLat[QString::fromUtf8("ц").at(0)] = "ts";
        RusToLat[QString::fromUtf8("ч").at(0)] = "ch";
        RusToLat[QString::fromUtf8("ш").at(0)] = "sh";
        RusToLat[QString::fromUtf8("щ").at(0)] = "sch";
        RusToLat[QString::fromUtf8("ы").at(0)] = "yi";
        RusToLat[QString::fromUtf8("э").at(0)] = "ye";
        RusToLat[QString::fromUtf8("ю").at(0)] = "yu";
        RusToLat[QString::fromUtf8("я").at(0)] = "ya";
    }
    for (int i=0; i<x.length(); i++) {
        const QChar src = x.at(i);
        if (src == ' ') {
            result += "_";
        }
        else if (src.unicode() <= 0x7F) {
            result += src;
        }
        else {
            const QChar srcL = src.toLower();
            if (RusToLat.contains(srcL)) {
                QString letter = RusToLat[srcL];
                if (src.isUpper())
                    letter[0] = letter[0].toUpper();
                result += letter;
            }
        }
    }
    return result;
}

bool NameTranslator::isReservedName(const QString &x)
{
    // TODO implement me
    return false;
}


} // namespace LLVMCodeGenerator

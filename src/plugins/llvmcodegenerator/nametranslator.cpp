#include "nametranslator.h"

#include <cstdint>

namespace LLVMCodeGenerator {

NameTranslator::NameTranslator()
{
}

void NameTranslator::reset()
{
    l_.clear();
    Namespace root;
    l_.push_back(root);
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

CString NameTranslator::addGlobal(const QString &name)
{
    const QString baseCname = suggestName(name);
    QString cname = baseCname;

    uint16_t counter = 1;
    while (isReservedName(cname) || !find(cname).empty()) {
        cname = QString("%1_%2").arg(baseCname).arg(counter);
    }

    CString result = cname.toStdString();
    g_.rmap.insert(result, name);
    g_.map.insert(name, result);
    return result;
}

void NameTranslator::beginNamespace()
{
    Namespace newNS;
    l_.push_back(newNS);
}


void NameTranslator::endNamespace()
{
    l_.pop_back();
}

CString NameTranslator::find(const QString &name) const
{
    int nsIndex = l_.size() - 1;
    CString result;
    while (nsIndex >= 0) {
        const Namespace &ns = l_.at(nsIndex);
        if (ns.map.contains(name)) {
            result = ns.map[name];
            break;
        }
        nsIndex --;
    }
    if (result.empty() && g_.map.contains(name)) {
        result = g_.map[name];
    }
    return result;
}

NameTranslator::Namespace& NameTranslator::ns()
{
    return l_.back();
}

QString NameTranslator::suggestName(const QString &x)
{
    QString result;
    static QMap<QChar,QString> LetterToLat;
    if (LetterToLat.isEmpty()) {
        LetterToLat[QString::fromUtf8("а").at(0)] = "a";
        LetterToLat[QString::fromUtf8("б").at(0)] = "b";
        LetterToLat[QString::fromUtf8("в").at(0)] = "v";
        LetterToLat[QString::fromUtf8("г").at(0)] = "g";
        LetterToLat[QString::fromUtf8("д").at(0)] = "d";
        LetterToLat[QString::fromUtf8("е").at(0)] = "e";
        LetterToLat[QString::fromUtf8("ё").at(0)] = "yo";
        LetterToLat[QString::fromUtf8("ж").at(0)] = "zh";
        LetterToLat[QString::fromUtf8("з").at(0)] = "z";
        LetterToLat[QString::fromUtf8("и").at(0)] = "i";
        LetterToLat[QString::fromUtf8("й").at(0)] = "j";
        LetterToLat[QString::fromUtf8("к").at(0)] = "k";
        LetterToLat[QString::fromUtf8("л").at(0)] = "l";
        LetterToLat[QString::fromUtf8("м").at(0)] = "m";
        LetterToLat[QString::fromUtf8("н").at(0)] = "n";
        LetterToLat[QString::fromUtf8("о").at(0)] = "o";
        LetterToLat[QString::fromUtf8("п").at(0)] = "p";
        LetterToLat[QString::fromUtf8("р").at(0)] = "r";
        LetterToLat[QString::fromUtf8("с").at(0)] = "s";
        LetterToLat[QString::fromUtf8("т").at(0)] = "t";
        LetterToLat[QString::fromUtf8("у").at(0)] = "u";
        LetterToLat[QString::fromUtf8("ф").at(0)] = "f";
        LetterToLat[QString::fromUtf8("х").at(0)] = "h";
        LetterToLat[QString::fromUtf8("ц").at(0)] = "ts";
        LetterToLat[QString::fromUtf8("ч").at(0)] = "ch";
        LetterToLat[QString::fromUtf8("ш").at(0)] = "sh";
        LetterToLat[QString::fromUtf8("щ").at(0)] = "sch";
        LetterToLat[QString::fromUtf8("ы").at(0)] = "yi";
        LetterToLat[QString::fromUtf8("э").at(0)] = "ye";
        LetterToLat[QString::fromUtf8("ю").at(0)] = "yu";
        LetterToLat[QString::fromUtf8("я").at(0)] = "ya";
        // Greek letters
        LetterToLat[QString::fromUtf8("α").at(0)] = "alpha";
        LetterToLat[QString::fromUtf8("β").at(0)] = "beta";
        LetterToLat[QString::fromUtf8("γ").at(0)] = "gamma";
        LetterToLat[QString::fromUtf8("δ").at(0)] = "delta";
        LetterToLat[QString::fromUtf8("ε").at(0)] = "epsilon";
        LetterToLat[QString::fromUtf8("ζ").at(0)] = "zeta";
        LetterToLat[QString::fromUtf8("η").at(0)] = "eta";
        LetterToLat[QString::fromUtf8("θ").at(0)] = "theta";
        LetterToLat[QString::fromUtf8("ι").at(0)] = "iota";
        LetterToLat[QString::fromUtf8("κ").at(0)] = "kappa";
        LetterToLat[QString::fromUtf8("λ").at(0)] = "lambda";
        LetterToLat[QString::fromUtf8("μ").at(0)] = "mu";
        LetterToLat[QString::fromUtf8("ν").at(0)] = "nu";
        LetterToLat[QString::fromUtf8("ξ").at(0)] = "xi";
        LetterToLat[QString::fromUtf8("ο").at(0)] = "omicron";
        LetterToLat[QString::fromUtf8("π").at(0)] = "pi";
        LetterToLat[QString::fromUtf8("ρ").at(0)] = "rho";
        LetterToLat[QString::fromUtf8("ς").at(0)] = "final_sigma";
        LetterToLat[QString::fromUtf8("σ").at(0)] = "sigma";
        LetterToLat[QString::fromUtf8("τ").at(0)] = "tau";
        LetterToLat[QString::fromUtf8("υ").at(0)] = "upsilon";
        LetterToLat[QString::fromUtf8("φ").at(0)] = "phi";
        LetterToLat[QString::fromUtf8("χ").at(0)] = "chi";
        LetterToLat[QString::fromUtf8("ψ").at(0)] = "psi";
        LetterToLat[QString::fromUtf8("ω").at(0)] = "omega";

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
            if (LetterToLat.contains(srcL)) {
                QString letter = LetterToLat[srcL];
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

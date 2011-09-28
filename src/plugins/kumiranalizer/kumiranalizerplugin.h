/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** No Commercial Usage
**
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**************************************************************************/

#ifndef KUMIRANALIZERPLUGIN_H
#define KUMIRANALIZERPLUGIN_H

#include <extensionsystem/kplugin.h>
#include <interfaces/analizerinterface.h>

#include <QtCore/QLocale>

namespace KumirAnalizer {

class Analizer;

class KumirAnalizerPlugin
        : public ExtensionSystem::KPlugin
        , public Shared::AnalizerInterface

{
    Q_OBJECT
    Q_INTERFACES(Shared::AnalizerInterface)
    friend class AnalizerPrivate;
public:
    KumirAnalizerPlugin();
    ~KumirAnalizerPlugin();

    inline bool primaryAlphabetIsLatin() const { return false; }
    inline bool caseInsensitiveGrammatic() const { return false; }
    inline bool supportPartialCompiling() const { return true; }

    int newDocument();
    void dropDocument(int documentId);
    void setSourceText(int documentId, const QString &text);
    void setHiddenText(int documentId, const QString &text, int baseLine);
    void setHiddenTextBaseLine(int documentId, int baseLine);
    void changeSourceText(int documentId, const QList<Shared::ChangeTextTransaction> & changes);
    std::string rawSourceData(int documentId) const;
    Shared::LineProp lineProp(int documentId, const QString & text) const;
    QList<Shared::Error> errors(int documentId) const;
    QList<Shared::LineProp> lineProperties(int documentId) const;
    QList<QPoint> lineRanks(int documentId) const;
    QStringList imports(int documentId) const;
    const AST::Data * abstractSyntaxTree(int documentId) const;
    QStringList algorhitmsAvailableFor(int documentId, int lineNo) const;
    QStringList globalsAvailableFor(int documentId, int lineNo) const;
    QStringList localsAvailableFor(int documentId, int lineNo) const;
    inline virtual QList<QRegExp> supportedFileNamePattern() const {
        return QList<QRegExp>()
                << QRegExp("*.kum",Qt::CaseInsensitive,QRegExp::Wildcard);
    }

protected:
    QString initialize(const QStringList &arguments);
    void start();
    void stop();
private:
    QVector<Analizer*> m_analizers;
};

}

#endif

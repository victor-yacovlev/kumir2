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

#include "kumiranalizerplugin.h"
#include "analizer.h"

#include <QtCore>


using namespace KumirAnalizer;


KumirAnalizerPlugin::KumirAnalizerPlugin()
{
    m_analizers = QVector<Analizer*> (128, NULL);
}


KumirAnalizerPlugin::~KumirAnalizerPlugin()
{
}


QString KumirAnalizerPlugin::initialize(const QStringList &arguments)
{
    QLocale::Language language = QLocale::Russian;

    Q_FOREACH (const QString &arg, arguments) {
        if (arg.startsWith("language=")) {
            const QString lang = arg.mid(9);
            const QLocale loc(lang);
            if (loc.language()!=QLocale::C) {
                language = loc.language();
                break;
            }
        }
    }

    Analizer::setSourceLanguage(language);

    return "";
}

void KumirAnalizerPlugin::start()
{

}

void KumirAnalizerPlugin::stop()
{

}

int KumirAnalizerPlugin::newDocument()
{
    int id = 0;
    for (int i=0 ; i < m_analizers.size(); i++) {
        if (m_analizers[i]==NULL) {
            id = i;
            break;
        }
    }
    m_analizers[id] = new Analizer(this);
    return id;
}

void KumirAnalizerPlugin::dropDocument(int documentId)
{
    Q_CHECK_PTR(m_analizers[documentId]);
    m_analizers[documentId]->deleteLater();
    m_analizers[documentId] = NULL;
}

void KumirAnalizerPlugin::setSourceText(int documentId, const QString &text)
{
    Q_CHECK_PTR(m_analizers[documentId]);
    m_analizers[documentId]->setSourceText(text);
}

void KumirAnalizerPlugin::changeSourceText(int documentId, int pos, int len, const QString &repl)
{
    Q_CHECK_PTR(m_analizers[documentId]);
    m_analizers[documentId]->changeSourceText(pos, len, repl);
}

QList<Shared::Error> KumirAnalizerPlugin::errors(int documentId) const
{
    Q_CHECK_PTR(m_analizers[documentId]);
    return m_analizers[documentId]->errors();
}

QList<Shared::LineProp> KumirAnalizerPlugin::lineProperties(int documentId) const
{
    Q_CHECK_PTR(m_analizers[documentId]);
    return m_analizers[documentId]->lineProperties();
}

QList<int> KumirAnalizerPlugin::lineIndents(int documentId) const
{
    Q_CHECK_PTR(m_analizers[documentId]);
    return m_analizers[documentId]->lineIndents();
}

QStringList KumirAnalizerPlugin::imports(int documentId) const
{
    Q_CHECK_PTR(m_analizers[documentId]);
    return m_analizers[documentId]->imports();
}

const AST::Data * KumirAnalizerPlugin::abstractSyntaxTree(int documentId) const
{
    Q_CHECK_PTR(m_analizers[documentId]);
    return m_analizers[documentId]->abstractSyntaxTree();
}





Q_EXPORT_PLUGIN2(KumirAnalizerPlugin, KumirAnalizer::KumirAnalizerPlugin)

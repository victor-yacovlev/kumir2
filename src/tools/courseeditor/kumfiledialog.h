/****************************************************************************
**
** Copyright (C) 2004-2008 NIISI RAS. All rights reserved.
**
** This file is part of the KuMir.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#ifndef KUMFILEDIALOG_H
#define KUMFILEDIALOG_H

#include <QFileDialog>
#include <QtGui>

/**
	@author Victor Yacovlev <V.Yacovlev@gmail.com>
*/

struct TextFileInfo {
	QString path;
	QString encoding;
	bool accepted;
};

class KumFileDialog : public QFileDialog
{
Q_OBJECT
public:
		KumFileDialog
				( QWidget * parent = 0, const QString & caption = QString(), const QString & directory = QString(), const QString & filter = QString(), bool showEncodings = true );

    ~KumFileDialog();
		QString encoding();
		void setEncoding(const QString &e);
		static TextFileInfo getOpenFileName( QWidget * parent = 0, const QString & caption = QString(), const QString & dir = QString(), const QString & filter = QString(), QString encoding = QString("UTF-8"), bool showEncodings = true );
		static TextFileInfo getSaveFileName( QWidget * parent = 0, const QString & caption = QString(), const QString & dir = QString(), const QString & filter = QString(), QString encoding = QString("UTF-8"), bool showEncodings = true );
		void setEncodingVisible(bool flag);
		void setEncodingEnabled(QList<bool> filtersEnabled);
	protected:
		QComboBox *encodings;
		QLabel *l;
		QList<bool> filtersEnabled;
		
	protected slots:
		void checkFilter(const QString & filter);
		

};

#endif

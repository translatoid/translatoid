/*
 *   Copyright (C) 2008 Schutz Sacha <istdasklar@gmail.com>
 *   AND lex newton http://qgoogletranslate.googlecode.com
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2,
 *   or (at your option) any later version, as published by the Free
 *   Software Foundation
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.*/
#ifndef REMINDER_H
#define REMINDER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQueryModel>
class Reminder : public QObject
{
    Q_OBJECT
public:
    Reminder(QObject * parent=0);

    void save(QString source, QString translated, QString lnSource=QString(), QString lnTranslated=QString());
     void createDb();
     void connection();
    QString source(int id);
    QString translated(int id);
    QString lgSource(int id);
    QString lgTranslated(int id);
    QSqlQueryModel * model();

    int count();

    public Q_SLOTS:
        void clear();
private:
    QSqlDatabase m_db ;
    QSqlQueryModel * m_model;
};

#endif // REMINDER_H

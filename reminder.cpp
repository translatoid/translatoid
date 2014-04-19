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


#include "reminder.h"
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlField>
#include <QMessageBox>
#include <QVariant>
#include <QDateTime>
#include <KStandardDirs>
#include <KDebug>
#include <QFile>
#include <KLocale>
Reminder::Reminder(QObject *parent)
        :QObject(parent)
{
    qsrand(QDateTime::currentDateTime ().toTime_t ());
    createDb();
    connection();


m_model= new QSqlQueryModel;
}

void Reminder::connection()
{
  m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setHostName("localhost");
    m_db.setDatabaseName(KStandardDirs::locateLocal("data", "translatoid/translatoid.db"));
    m_db.setUserName("root");
    m_db.setPassword("");

    if (!m_db.open())
      kDebug()<<"Cannot connect to the databases in "<<KStandardDirs::locateLocal("data", "translatoid/translatoid.db");
    else
     kDebug()<<"connection success";
    
}
  void Reminder::createDb()
  {

    // Create the $HOME/.kde4/... /translatoid.db  : database for users
    if (!QFile::exists(KStandardDirs::locateLocal("data", "translatoid/translatoid.db")))
    {
        QFile rootFile(KStandardDirs::locate("data", "translatoid/translatoid.db"));
        rootFile.copy (KStandardDirs::locateLocal("data", "translatoid/translatoid.db"));
    }
  }


void Reminder::save(QString source, QString translated, QString lnSource, QString lnTranslated)
{

    QSqlQuery query;
    query.prepare("INSERT INTO sentence ( source, translated, source_lg, translated_lg) "
                  "VALUES (?,?,?,?)");
    query.bindValue(0, source);
    query.bindValue(1, translated);
    query.bindValue(2, lnSource);
    query.bindValue(3, lnTranslated);
    query.exec();
}
int  Reminder::count()
{
    QSqlQuery query("SELECT count(*) FROM sentence");
    if ( query.first())
        return query.value(0).toInt();
    else return 0;

}
QString Reminder::source(int id)
{
    QSqlQuery query("SELECT source FROM sentence where id="+QString::number(id));
    if ( query.first())
        return query.value(0).toString();
    else return QString();

}
QString Reminder::translated(int id)
{
    QSqlQuery query("SELECT translated FROM sentence where id="+QString::number(id));
    if (query.first())
        return query.value(0).toString();
    else return QString();
}
QString Reminder::lgSource(int id)
{
    QSqlQuery query("SELECT source_lg FROM sentence where id="+QString::number(id));
    if (query.first())
        return query.value(0).toString();
    else return 0;
}
QString Reminder::lgTranslated(int id)
{
    QSqlQuery query("SELECT translated_lg FROM sentence where id="+QString::number(id));
    if (query.first())
        return query.value(0).toString();
    else return 0;
}
QSqlQueryModel *Reminder ::model()
{

    m_model->setQuery("SELECT id, source,translated FROM sentence");
    return m_model;

}
void Reminder:: clear()
{
    //Don't know why : The SQlite delete query make a bug.....
    // So I m using an other methode!
QFile::remove(KStandardDirs::locateLocal("data", "translatoid/translatoid.db"));
createDb();
connection();
m_model->setQuery("SELECT id, source,translated FROM sentence");
}

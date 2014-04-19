#ifndef LANGUAGEMODEL_H
#define LANGUAGEMODEL_H
#include <QStandardItemModel>
#include <QStandardItem>
#include <QMap>

#include "struc_lng.h"

class LanguageModel : public QStandardItemModel
{
public:
    LanguageModel(QObject * parent=0);
    QString code(const QModelIndex& index) const;
    QString name(const QModelIndex& index) const;
    QString icon(const QModelIndex& index) const;
    Language language(const QModelIndex& index) const;
    Language languageFromCode(const QString& code) const;
    QMap<QString, Language> allLanguages() const;

protected:
    void setupLanguageMap();

private:

    QMap <QString, Language> m_languageMap;
};
#endif

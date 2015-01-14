#include "languagemodel.h"

#include <KIcon>
#include <KLocale>
#include <KStandardDirs>
LanguageModel::LanguageModel(QObject * parent)
        : QStandardItemModel(parent)
{

    setupLanguageMap();
    setColumnCount (2);

    setHeaderData(0, Qt::Horizontal, i18nc("Language code", "Code"));
    setHeaderData(1, Qt::Horizontal, i18n("Country"));

    QMapIterator<QString, Language> i(m_languageMap);
    while (i.hasNext()) {
        i.next();
//         m_listWidget->addItem(new QListWidgetItem(KIcon(i.value().icon), i.value().name));

        QStandardItem * item = new QStandardItem(i.value().code);
        item->setIcon ( KIcon(i.value().icon) );
        item->setEditable ( false );
        appendRow (item );

        QStandardItem * itemName= new QStandardItem(i.value().name);

        setItem (item->row(),1, itemName);

    }

    setSortRole(0);
}

//=============================================================
QString LanguageModel::code(const QModelIndex& index) const
{
    QStandardItem * item = itemFromIndex (index );
    return m_languageMap[item->text()].code;
}

//=============================================================
QString LanguageModel::name(const QModelIndex& index) const
{
    QStandardItem * item = itemFromIndex (index );
    return m_languageMap[item->text()].name;
}

//=============================================================
QString LanguageModel::icon(const QModelIndex& index) const
{
    QStandardItem * item = itemFromIndex (index );
    return m_languageMap[item->text()].icon;
}

//=============================================================
Language LanguageModel::language(const QModelIndex& index) const
{
    QStandardItem * item = itemFromIndex (index );
    return m_languageMap[item->text()];
}

//=============================================================
Language LanguageModel::languageFromCode(const QString& code) const
{
    return m_languageMap[code];
}

//============================================================
QMap<QString, Language> LanguageModel::allLanguages() const
{
    return m_languageMap;
}

//============================================================
void LanguageModel::setupLanguageMap()
{
    Language c;

    c.code = "auto";
    c.icon = KStandardDirs::locate("icon", "oxygen/16x16/actions/help-hint.png");
    c.name = i18n("autodetect");
    m_languageMap[c.code] = (c);

    c.code = "de";
    c.icon = "kbflags/de.png";
    c.name = i18n("German");
    m_languageMap[c.code] = (c);

    c.code = "en";
    c.icon = "kbflags/gb.png";
    c.name = i18n("English");
    m_languageMap[c.code] = (c);

    c.code = "ar";
    c.icon = "kbflags/ae.png";
    c.name = i18n("Arabic");
    m_languageMap[c.code] = (c);

    c.code = "bg";
    c.icon = "kbflags/bg.png";
    c.name = i18n("Bulgarian");
    m_languageMap[c.code] = (c);

    c.code = "ca";
    c.icon = "kbflags/ct.png";
    c.name = i18n("Catalan");
    m_languageMap[c.code] = (c);

    c.code = "zh-CN";
    c.icon = "kbflags/cn.png";
    c.name = i18n("Chinese");
    m_languageMap[c.code] = (c);

    c.code = "ko";
    c.icon = "kbflags/kr.png";
    c.name = i18n("Korean");
    m_languageMap[c.code] = (c);

    c.code = "hr";
    c.icon = "kbflags/hr.png";
    c.name = i18n("Croatian");
    m_languageMap[c.code] = (c);

    c.code = "da";
    c.icon = "kbflags/dk.png";
    c.name = i18n("Danish");
    m_languageMap[c.code] = (c);

    c.code = "et";
    c.icon = "kbflags/ee.png";
    c.name = i18n("Estonian");
    m_languageMap[c.code] = (c);

    c.code = "es";
    c.icon = "kbflags/es.png";
    c.name = i18n("Spanish");
    m_languageMap[c.code] = (c);

    c.code = "fi";
    c.icon = "kbflags/fi.png";
    c.name = i18n("Finnish");
    m_languageMap[c.code] = (c);

    c.code = "fr";
    c.icon = "kbflags/fr.png";
    c.name = i18n("French");
    m_languageMap[c.code] = (c);

    c.code = "el";
    c.icon = "kbflags/gr.png";
    c.name = i18n("Greek");
    m_languageMap[c.code] = (c);

    c.code = "iw";
    c.icon = "kbflags/il.png";
    c.name = i18n("Hebrew");
    m_languageMap[c.code] = (c);

    c.code = "hi";
    c.icon = "kbflags/in.png";
    c.name = i18n("Hindi");
    m_languageMap[c.code] = (c);

    c.code = "id";
    c.icon = "kbflags/id.png";
    c.name = i18n("Indonesian");
    m_languageMap[c.code] = (c);

    c.code = "it";
    c.icon = "kbflags/it.png";
    c.name = i18n("Italian");
    m_languageMap[c.code] = (c);

    c.code = "ja";
    c.icon = "kbflags/jp.png";
    c.name = i18n("Japanese");
    m_languageMap[c.code] = (c);

    c.code = "lv";
    c.icon = "kbflags/lv.png";
    c.name = i18n("Latvian");
    m_languageMap[c.code] = (c);

    c.code = "lt";
    c.icon = "kbflags/lt.png";
    c.name = i18n("Lithuanian");
    m_languageMap[c.code] = (c);

    c.code = "nl";
    c.icon = "kbflags/nl.png";
    c.name = i18n("Dutch");
    m_languageMap[c.code] = (c);

    c.code = "no";
    c.icon = "kbflags/no.png";
    c.name = i18n("Norwegian");
    m_languageMap[c.code] = (c);

    c.code = "pl";
    c.icon = "kbflags/pl.png";
    c.name = i18n("Polish");
    m_languageMap[c.code] = (c);


    c.code = "pt";
    c.icon = "kbflags/pt.png";
    c.name = i18n("Portuguese");
    m_languageMap[c.code] = (c);

    c.code = "ro";
    c.icon = "kbflags/ro.png";
    c.name = i18n("Romanian");
    m_languageMap[c.code] = (c);

    c.code = "ru";
    c.icon = "kbflags/ru.png";
    c.name = i18n("Russian");
    m_languageMap[c.code] = (c);

    c.code = "sr";
    c.icon = "kbflags/rs.png";
    c.name = i18n("Serbian");
    m_languageMap[c.code] = (c);

    c.code = "sk";
    c.icon = "kbflags/sk.png";
    c.name = i18n("Slovak");
    m_languageMap[c.code] = (c);


    c.code = "sl";
    c.icon = "kbflags/si.png";
    c.name = i18n("Slovenian");
    m_languageMap[c.code] = (c);

    c.code = "sv";
    c.icon = "kbflags/se.png";
    c.name = i18n("Swedish");
    m_languageMap[c.code] = (c);

    c.code = "tl";
    c.icon = "kbflags/ph.png";
    c.name = i18n("Tagalog");
    m_languageMap[c.code] = (c);


    c.code = "cs";
    c.icon = "kbflags/cz.png";
    c.name = i18n("Czech");
    m_languageMap[c.code] = (c);

    c.code = "uk";
    c.icon = "kbflags/ua.png";
    c.name = i18n("Ukrainian");
    m_languageMap[c.code] = (c);

    c.code = "vi";
    c.icon = "kbflags/vn.png";
    c.name = i18n("Vietnamese");
    m_languageMap[c.code] = (c);
    
    c.code = "th";
    c.icon = "kbflags/th.png";
    c.name = i18n("Thaï");
    m_languageMap[c.code] = (c);


    c.code = "af";
    c.icon = "kbflags/za.png";
    c.name = i18n("Afrikaans");
    m_languageMap[c.code] = (c);

    c.code = "sq";
    c.icon = "kbflags/al.png";
    c.name = i18n("Albanais");
    m_languageMap[c.code] = (c);

    c.code = "sq";
    c.icon = "kbflags/al.png";
    c.name = i18n("Albanais");
    m_languageMap[c.code] = (c);

    c.code = "be";
    c.icon = "kbflags/by.png";
    c.name = i18n("Belarusian");
    m_languageMap[c.code] = (c);

    c.code = "ga";
    c.icon = "kbflags/ie.png";
    c.name = i18n("Irish");
    m_languageMap[c.code] = (c);

    c.code = "is";
    c.icon = "kbflags/is.png";
    c.name = i18n("Icelandic");
    m_languageMap[c.code] = (c);


    c.code = "mk";
    c.icon = "kbflags/mk.png";
    c.name = i18n("Macedonian");
    m_languageMap[c.code] = (c);


    c.code = "ms";
    c.icon = "kbflags/my.png";
    c.name = i18n("Malaysia");
    m_languageMap[c.code] = (c);


    c.code = "mt";
    c.icon = "kbflags/mt.png";
    c.name = i18n("Maltese");
    m_languageMap[c.code] = (c);


    c.code = "fa";
    c.icon = "kbflags/ir.png";
    c.name = i18n("Persan");
    m_languageMap[c.code] = (c);


    c.code = "sw";
    c.icon = "kbflags/tz.png";
    c.name = i18n("Swahili");
    m_languageMap[c.code] = (c);

      c.code = "tr";
    c.icon = "kbflags/tr.png";
    c.name = i18n("Turkish");
    m_languageMap[c.code] = (c);


      c.code = "il";
    c.icon = "kbflags/il.png";
    c.name = i18n("Yiddish");
    m_languageMap[c.code] = (c);



}


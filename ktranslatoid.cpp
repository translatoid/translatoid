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


#include "ktranslatoid.h"

#include <QClipboard>
#include <QLabel>
#include <QMenu>
#include <QPainter>
#include <QTextCodec>
#include <QToolButton>
#include <QTreeView>

#include <KConfigDialog>
#include <KDebug>
#include <KMessageBox>
#include <KPushButton>
#include <KStandardDirs>
#include <KTextEdit>
#include <KIconLoader>
#include <KIO/Job>

#include <Solid/Device>
#include <Solid/Networking>

#include <Plasma/Containment>
#include <Plasma/Corona>
#include <Plasma/Theme>
#include <Plasma/Frame>

#include <qjson/parser.h>


KTranslatoid ::KTranslatoid(QObject *parent, const QVariantList &args)
        :  Plasma::PopupApplet(parent, args),
        m_job(0)
{
    m_sourceText = new Plasma::TextEdit;
    m_destText = new Plasma::TextEdit;
    m_buttonTranslate = new Plasma::PushButton;
    m_buttonChange = new Plasma::ToolButton;
    m_sourceLanguageLabel = new Plasma::Label;
    m_destLanguageLabel = new Plasma::Label;
    m_sourceLanguageButton = new Plasma::ToolButton;
    m_destLanguageButton = new Plasma::ToolButton;
    m_popupIcon = KIcon("translator");
    m_autoPaste = true;
    m_autoTranslate = true;
    m_autoSwapTranslate = true;
    m_remember = true;
    m_reminderDuration=5;
    m_reminderFrenquency= 10;
    m_languageModel = new LanguageModel;
    if (m_remember) {
	m_reminder= new Reminder;
    }

    m_sourceText->nativeWidget()->installEventFilter(this);
    connect(m_buttonTranslate, SIGNAL(clicked()), this, SLOT(translate()));
    connect(m_buttonChange, SIGNAL(clicked()), this, SLOT(swapLanguages()));
    connect(m_sourceLanguageButton, SIGNAL(clicked()), this, SLOT(changeSourceLanguage()));
    connect(m_destLanguageButton, SIGNAL(clicked()), this, SLOT(changeDestinationLanguage()));

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    setAspectRatioMode(Plasma::IgnoreAspectRatio);

    //Create 2 extender ! 1 for translation and 1 for reminder
    if (m_remember) { // FIXME this is ignored, because = TRUE!!!
	m_reminderExtender = new Plasma::ExtenderItem(extender());
   	m_reminderExtender->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }
    m_extender = new Plasma::ExtenderItem(extender());
    m_extender->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    //create Timer to call showReminder
    if (m_remember) {
    	m_reminderTimer = new QTimer;
    	connect(m_reminderTimer,SIGNAL(timeout()),this,SLOT(showReminder()));
    }
}

//========================================================================================

KTranslatoid ::~KTranslatoid()
{
    writeConfig();

}

//====================================================================================
void KTranslatoid::init()
{
    readConfig();
    constructForm();
    setupPopupIcon();

}

//=============
void KTranslatoid::constructForm()
{
    m_graphicsWidget = new QGraphicsWidget(m_extender);

    m_buttonChange->setIcon(KIcon("system-switch-user.png"));
    m_buttonChange->nativeWidget()->setToolTip(i18n("Invert the translation"));

    m_buttonTranslate->setIcon(KIcon("tools-wizard.png"));
    m_buttonTranslate->setText(i18n("Translate"));
    m_buttonTranslate->nativeWidget()->setToolTip(i18n("Click here to translate"));

    QFont font = Plasma::Theme::defaultTheme()->font(Plasma::Theme::DefaultFont);

    m_sourceText->setFont(font);
    m_destText->setFont(font);

    m_destText->nativeWidget()->setReadOnly(true);

    Plasma::ToolButton * pastButton = new Plasma::ToolButton;
    Plasma::ToolButton * copyButton = new Plasma::ToolButton;
    Plasma::ToolButton * clearButton = new Plasma::ToolButton;
    Plasma::ToolButton * reminderButton = new Plasma::ToolButton;

    pastButton->nativeWidget()->setIcon(KIcon("edit-paste.png"));
    copyButton->nativeWidget()->setIcon(KIcon("edit-copy.png"));
    clearButton->nativeWidget()->setIcon(KIcon("edit-delete.png"));
    if (m_remember) {
	reminderButton->nativeWidget()->setIcon(KIcon("rating.png"));
    }
    pastButton->nativeWidget()->setToolTip(i18n("Paste a selection to translate it"));
    copyButton->nativeWidget()->setToolTip(i18n("Copy the result"));
    clearButton->nativeWidget()->setToolTip(i18n("Clear the source text"));
    if (m_remember) {
	reminderButton->nativeWidget()->setToolTip(i18n("Click here to remember the translation"));
    }
    m_sourceLanguageButton->nativeWidget()->setToolTip(i18n("Change source language"));
    m_destLanguageButton->nativeWidget()->setToolTip(i18n("Change destination language"));

    connect(pastButton->nativeWidget(), SIGNAL(clicked()), this, SLOT(paste()));
    connect(copyButton->nativeWidget(), SIGNAL(clicked()), this, SLOT(copy()));
    connect(clearButton->nativeWidget(), SIGNAL(clicked()), m_sourceText->nativeWidget(), SLOT(clear()));
    if (m_remember) {
	connect(reminderButton->nativeWidget(),SIGNAL(clicked()),this,SLOT(saveReminder()));
    }
    QGraphicsLinearLayout*  layoutControl = new QGraphicsLinearLayout(Qt::Horizontal);

    layoutControl->addItem(m_sourceLanguageButton);
    layoutControl->addItem(m_buttonChange);
    layoutControl->addItem(m_destLanguageButton);
    if (m_remember) {
	layoutControl->addItem(reminderButton);
    }

    layoutControl->addItem(m_buttonTranslate);
    layoutControl->addItem(clearButton);
    layoutControl->addItem(pastButton);
    layoutControl->addItem(copyButton);

    layoutControl->setMaximumHeight(20);

    QGraphicsLinearLayout* mainLayout = new QGraphicsLinearLayout(Qt::Vertical);
    mainLayout->addItem(m_sourceLanguageLabel);
    mainLayout->addItem(m_sourceText);
    mainLayout->addItem(m_destLanguageLabel);
    mainLayout->addItem(m_destText);

    mainLayout->addItem(layoutControl);
    QColor col = Plasma::Theme::defaultTheme()->color(Plasma::Theme::defaultTheme()->TextColor);
    m_sourceText->setStyleSheet("color:"+col.name());

    m_destText->setStyleSheet("color:"+col.name());

    QGraphicsWidget * widget = new QGraphicsWidget;
    widget->setLayout(mainLayout);

    
    bigLayout = new QGraphicsLinearLayout(Qt::Horizontal);
    bigLayout->addItem(widget);
    m_graphicsWidget->setLayout(bigLayout);
    m_graphicsWidget->setPreferredSize(530, 320);

    m_sourceText->nativeWidget()->selectAll();

    if (Solid::Networking::status() != Solid::Networking::Connected) {
        kDebug() << i18n("Networking is disabled. Cannot connect to Google.");
    }


    //Setup extenders
    m_reminderSourceText = new Plasma::Label;
    m_reminderTranslatedText= new Plasma::Label;
    // not used
    //QColor sourceCol = Plasma::Theme::defaultTheme()->color(Plasma::Theme::defaultTheme()->TextColor);
    //QColor translatedCol =  Plasma::Theme::defaultTheme()->color(Plasma::Theme::defaultTheme()->TextColor);
    if (m_remember) {
    m_reminderSourceText->setStyleSheet("color:red;font-weight:bold");
    m_reminderTranslatedText->setStyleSheet("color:green;font-weight:bold");

    QGraphicsLinearLayout *reminderLayout = new QGraphicsLinearLayout(Qt::Vertical);
    reminderLayout->setAlignment(m_reminderSourceText,Qt::AlignTop);
    reminderLayout->setAlignment(m_reminderTranslatedText,Qt::AlignTop);
    reminderLayout->addItem(m_reminderSourceText);
    reminderLayout->addItem(m_reminderTranslatedText);

    QGraphicsWidget * reminderWidget = new QGraphicsWidget;
    reminderWidget->setLayout(reminderLayout);
    m_reminderExtender->setWidget(reminderWidget);
    m_reminderExtender->setName("reminder");
    m_reminderExtender->setTitle(i18n("Do you remember?"));
    m_reminderExtender->setIcon(KIcon("rating"));
    }
    m_extender->setWidget(m_graphicsWidget);
    m_extender->setTitle(i18n("Translatoid"));


}

//====================================================================================
//QGraphicsWidget *KTranslatoid::graphicsWidget()
//{
//
//    //Explain : when the user change the plasma theme, need to update color Text by defaut theme. Otherwise, the text color can be same
//    // than background color.
//    QColor col = Plasma::Theme::defaultTheme()->color(Plasma::Theme::defaultTheme()->TextColor);
//    m_sourceText->setStyleSheet("color:"+col.name());
//    m_destText->setStyleSheet("color:"+col.name());
//    return m_graphicsWidget;
//}

//=============================================================================
void KTranslatoid::translate()
{
    kDebug() << "Starting translation.";

    KUrl url("http://www.google.com/translate_a/t");
    //kDebug() << "Url configured." << url;

    QByteArray postData;
    QString source = m_sourceLanguage.code.toUtf8().toPercentEncoding();
    QString dest = m_destLanguage.code.toUtf8().toPercentEncoding();
    QString texte= m_sourceText->nativeWidget()->toPlainText().toUtf8().toPercentEncoding();
    QString st = "text="+texte+"&client=json&sl=" + source + "&tl=" + dest;

    postData = st.toUtf8();

    
    if (m_job) {
        disconnect(m_job, 0, this, 0);
    }

    m_data.clear();
    delete m_job;

    m_job = KIO::http_post(url, postData, KIO::HideProgressInfo);
    m_job->addMetaData("Content-Type", "application/x-www-form-urlencoded");
    m_job->addMetaData("Host", "www.google.com");
    m_job->addMetaData("User-Agent", "Mozilla/5.0");
    m_job->addMetaData("Accept-Encoding", "deflate");
    m_job->addMetaData("Connection", "Close");

    connect(m_job, SIGNAL(data(KIO::Job*,QByteArray)), this, SLOT(httpDataReceived(KIO::Job*,QByteArray)));
    connect(m_job, SIGNAL(result(KJob*)), this, SLOT(done()));
    setBusy(true);
    kDebug() << "Request started";
    kDebug()<<postData;
}

//==============================================================================
void KTranslatoid::httpDataReceived(KIO::Job *job, QByteArray data)
{
    kDebug() << "got data for" << m_job << job << data;
    if (job != m_job) {
        return;
    }

    m_data.append(data);
}

//==============================================================================
void KTranslatoid::done()
{
    setBusy(false);
    m_job = 0;
    //kDebug() << "HTTP Request done." << m_data;

    QString text;
    text = text.fromUtf8(m_data);
    /* m_data.clear();
    kDebug() << "All data is read.";*/

    QVariantList sentences;
    QVariantList dict;
    QString textToReturn = QString();
    QJson::Parser parser;
    bool ok;
    QVariantMap result = parser.parse(m_data,&ok).toMap();

    if (!ok) {
        kDebug()<<"An error occured during parsing";
        return;
    }

    if (result.contains("sentences"))
        sentences = result["sentences"].toList();

    if (result.contains("dict"))
        dict = result["dict"].toList();


foreach (QVariant trad , sentences)
    {


    if ( trad.toMap().contains("trans"))
    {

        textToReturn += trad.toMap().value("trans").toString();
    }

}
textToReturn+="<hr/>";
    QString dictionnary=QString();
    foreach ( QVariant var, dict)
    {

        if ( var.type() == QVariant::Map)
        {
            foreach ( QVariant map, var.toMap())
            {
                if ( map.type() == QVariant::String)
                    dictionnary+="<b>"+map.toString()+"</b><br/>";

                if ( map.type() == QVariant::List)
                {
                    foreach ( QVariant word, map.toList())
                    {
                        dictionnary+=( word.toString() + " ");
                    }
                }
            }
        }
    }

    textToReturn += dictionnary;
    m_destText->nativeWidget()->setHtml(textToReturn);
}

//===================================================================================
void KTranslatoid::swapLanguages()
{
    updateLanguage(m_destLanguage, m_sourceLanguage);
    if (m_autoSwapTranslate) {
        translate();
    }
    writeConfig();
}

//====================================================================================
void KTranslatoid::changeSourceLanguage()
{
    QMenu languageMenu;

    foreach (const Language &lang, m_languageModel->allLanguages()) {
        QAction *a = languageMenu.addAction(KIcon(lang.icon), lang.name);
        a->setData(lang.code);
    }

    connect(&languageMenu, SIGNAL(triggered(QAction*)),
            this, SLOT(sourceLanguageSelected(QAction*)));
    languageMenu.adjustSize();
    languageMenu.exec(containment()->corona()->popupPosition(m_sourceLanguageButton, languageMenu.size()));
}

//====================================================================================
void KTranslatoid::sourceLanguageSelected(QAction *action)
{
    if (!action) {
        return;
    }

    updateLanguage(m_languageModel->languageFromCode(action->data().toString()), m_destLanguage);
}

//====================================================================================
void KTranslatoid::changeDestinationLanguage()
{
    QMenu languageMenu;

    foreach (const Language &lang, m_languageModel->allLanguages()) {
        QAction *a = languageMenu.addAction(KIcon(lang.icon), lang.name);
        a->setData(lang.code);
    }

    connect(&languageMenu, SIGNAL(triggered(QAction*)),
            this, SLOT(destinationLanguageSelected(QAction*)));
    languageMenu.adjustSize();
    languageMenu.exec(containment()->corona()->popupPosition(m_sourceLanguageButton, languageMenu.size()));
}

//====================================================================================
void KTranslatoid::destinationLanguageSelected(QAction *action)
{
    if (!action) {
        return;
    }

    updateLanguage(m_sourceLanguage, m_languageModel->languageFromCode(action->data().toString()));
}
//=======================================================================================
void KTranslatoid::copy()
{
    m_destText->nativeWidget()->selectAll();
    m_destText->nativeWidget()->copy();
}
//=========================================================================================
void KTranslatoid::paste()
{
    m_sourceText->nativeWidget()->clear();
    m_sourceText->nativeWidget()->paste();
}

//==============================================================================================
void KTranslatoid::createConfigurationInterface(KConfigDialog *parent)
{
m_reminderTimer->stop();
    QWidget *languageForm = new QWidget;
    m_ui.setupUi(languageForm);
    m_ui.checkBoxPaste->setChecked(m_autoPaste);
    m_ui.checkBoxTranslate->setChecked(m_autoTranslate);
    m_ui.checkBoxSwapTranslate->setChecked(m_autoSwapTranslate);
    

    QWidget *reminderForm = new QWidget;
    m_uiReminder.setupUi(reminderForm);
    m_uiReminder.youRemember->setChecked(m_remember);    
    m_uiReminder.treeView->setModel(m_reminder->model());
    m_uiReminder.spinBoxDuration->setValue(m_reminderDuration);
    m_uiReminder.spinBoxFrequency->setValue(m_reminderFrenquency);

    connect(m_uiReminder.deleteButton,SIGNAL(clicked()),m_reminder,SLOT(clear()));


    parent->addPage(languageForm, i18n("Translation"), "preferences-desktop-locale");
    	parent->addPage(reminderForm, i18n("Reminder"), "story-editor");

    connect(parent, SIGNAL(applyClicked()), this, SLOT(configAccepted()));
    connect(parent, SIGNAL(okClicked()), this, SLOT(configAccepted()));
}

//================================================================================================
void KTranslatoid::configAccepted()
{
m_reminderTimer->start(m_reminderFrenquency*1000);
    m_autoPaste = m_ui.checkBoxPaste->isChecked();
    m_autoTranslate = m_ui.checkBoxTranslate->isChecked();
    m_autoSwapTranslate = m_ui.checkBoxSwapTranslate->isChecked();
    m_remember = m_uiReminder.youRemember->isChecked();
    m_reminderDuration = m_uiReminder.spinBoxDuration->value();
    m_reminderFrenquency = m_uiReminder.spinBoxFrequency->value();
    writeConfig();
}

//============================================================================================================
void KTranslatoid::popupEvent(bool show)
{

    if (show) {
        m_sourceText->grabKeyboard();
        if (m_autoPaste) {
            QClipboard *clipboard = QApplication::clipboard();


            m_sourceText->nativeWidget()->clear();
            m_sourceText->nativeWidget()->setText(clipboard->text(QClipboard::Selection));


            //             m_sourceText->nativeWidget()->paste();
        }

        if (m_autoTranslate)
            translate();

    }

    else {
        m_sourceText->ungrabKeyboard();

    }

}
//============================================================================================================
void KTranslatoid::setupPopupIcon()
{
    QPixmap pix(64, 64);
    pix.fill(Qt::transparent);
    QPainter paint(&pix);

    paint.drawPixmap(0, 0, KIcon("translator").pixmap(64, 64));

    QPixmap flag1 = KIcon(m_sourceLanguage.icon).pixmap(24, 33);
    QPixmap flag2 = KIcon(m_destLanguage.icon).pixmap(24, 33);
    //
    //    QTransform transform;
    //    transform.shear(0.15, -0.15);
    //    flag1 = flag1.transformed(transform, Qt::SmoothTransformation);
    //    flag2 = flag2.transformed(transform, Qt::SmoothTransformation);


    paint.drawPixmap(0, 40, flag1);
    paint.drawPixmap(40, 0, flag2);
    //    paint.setTransform(transform, Qt::SmoothTransformation);
    paint.setPen(Qt::white);

    //     paint.drawText(QRect(10,35,pix.width(),pix.height()), m_sourceLanguage.code+"  "+m_destLanguage.code);
    m_popupIcon = KIcon(pix);

    setPopupIcon(m_popupIcon);

}
//===============================================
void KTranslatoid::updateLanguage(Language a, Language b)
{
    m_sourceLanguage = a;
    m_destLanguage = b;
    m_sourceLanguageButton->nativeWidget()->setIcon(KIcon(m_sourceLanguage.icon));
    m_sourceLanguageLabel->setText(m_sourceLanguage.name);

    m_destLanguageButton->nativeWidget()->setIcon(KIcon(m_destLanguage.icon));
    m_destLanguageLabel->setText(m_destLanguage.name);

    setupPopupIcon();
}

//================================================
void KTranslatoid::readConfig()
{
    KConfigGroup cg = config();

    QString a = cg.readEntry("lsource", QString("fr"));
    QString b = cg.readEntry("lresult", QString("en"));
    m_autoPaste = cg.readEntry("autoPaste", true);
    m_autoTranslate = cg.readEntry("autoTranslate", true);
    m_autoSwapTranslate = cg.readEntry("autoSwapTranslate", true);

    m_remember = cg.readEntry("remember", true);
    m_reminderDuration = cg.readEntry("reminderDuration", 5);
    m_reminderFrenquency= cg.readEntry("reminderFrequency",10);

    kDebug()<<m_reminderDuration;
    kDebug()<<m_reminderFrenquency;




    Language la = m_languageModel->languageFromCode(a);
    Language lb = m_languageModel->languageFromCode(b);

    updateLanguage(la, lb);
 m_reminderTimer->start(m_reminderFrenquency*1000);


}

void KTranslatoid::writeConfig()
{
    KConfigGroup cg = config();

    cg.writeEntry("lsource", m_sourceLanguage.code);
    cg.writeEntry("lresult", m_destLanguage.code);

    cg.writeEntry("autoPaste", m_autoPaste);
    cg.writeEntry("autoTranslate", m_autoTranslate);
    cg.writeEntry("autoSwapTranslate", m_autoSwapTranslate);

    cg.writeEntry("remember", m_remember);
    cg.writeEntry("reminderDuration", m_reminderDuration);
    cg.writeEntry("reminderFrequency", m_reminderFrenquency);

    m_reminderTimer->setInterval(m_reminderFrenquency*1000);
    emit configNeedsSaving();
}

bool KTranslatoid::eventFilter(QObject * obj, QEvent *event)
{
    //Filter TextEdit Event
    //Enter  = translate
    //Shift+Enter = add new line
    if (obj == m_sourceText->nativeWidget()) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

            if (keyEvent->key() == Qt::Key_Return && keyEvent->modifiers() == Qt::ShiftModifier) {
                m_sourceText->nativeWidget()->append("");
                return true;
            }

            if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
                translate();
                return true;
            }
        }
    }

    return Plasma::PopupApplet::eventFilter(obj, event);
}

//================================================================
void KTranslatoid::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    Q_UNUSED(event);
    swapLanguages();
}

void KTranslatoid::showReminder()
{
    
    
    if ( !m_reminder->count())
        return ;
    int id = (qrand()%(m_reminder->count())) + 1;


    m_reminderSourceText->setText(m_reminder->source(id));
    m_reminderTranslatedText->setText(m_reminder->translated(id));
    m_reminderExtender->adjustSize();


    if (m_extender->isDetached())
        showPopup(m_reminderDuration*1000);

}

void KTranslatoid::saveReminder()
{
    QString source=  m_sourceText->nativeWidget()->toPlainText();
    QString translated=  m_destText->nativeWidget()->toPlainText();
    QString lgSource = m_sourceLanguage.code;
    QString lgTranslated = m_destLanguage.code;

    m_reminder->save(source,translated,lgSource,lgTranslated);

}

#include "ktranslatoid.moc"


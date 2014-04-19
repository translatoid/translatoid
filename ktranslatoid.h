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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
// Here we avoid loading the header multiple times
#ifndef KTRANSLATOR_HEADER
#define KTRANSLATOR_HEADER
// We need the Plasma Applet headers
#include <KIcon>
#include <KLineEdit>
#include <QGraphicsLinearLayout>
#include <QGraphicsWidget>
#include <QNetworkAccessManager>
#include <QPixmap>
#include <QTimer>

#include <Plasma/PopupApplet>
#include <plasma/widgets/textedit.h>
#include <plasma/widgets/pushbutton.h>
#include <plasma/widgets/toolbutton.h>
#include <plasma/widgets/combobox.h>
#include <plasma/widgets/label.h>
#include <plasma/widgets/iconwidget.h>
#include <plasma/widgets/frame.h>
#include <plasma/animator.h>
#include <plasma/widgets/treeview.h>
#include <plasma/tooltipmanager.h>

#include <Plasma/Extender>
#include <Plasma/ExtenderItem>
#include "ui_languageform.h"
#include "ui_reminderform.h"
#include "languagemodel.h"
#include "reminder.h"

namespace KIO
{
    class Job;
} // namespace KIO

// Define our plasma Applet
class KTranslatoid : public Plasma::PopupApplet
{
    Q_OBJECT
public:
    // Basic Create/Destroy
    KTranslatoid(QObject *parent, const QVariantList &args);
    ~KTranslatoid();
    //    QGraphicsWidget *graphicsWidget();
    void constructForm();
    void popupEvent(bool show);
    void readConfig();
    void writeConfig();
    void wheelEvent ( QGraphicsSceneWheelEvent * event ); 
    void init();
public slots:
    void translate();
    void swapLanguages();
    void changeSourceLanguage();
    void sourceLanguageSelected(QAction *action);
    void changeDestinationLanguage();
    void destinationLanguageSelected(QAction *action);
    void copy();
    void paste();
    void configAccepted();
    void setupPopupIcon();
    void updateLanguage(Language a, Language b);
    void httpDataReceived(KIO::Job *job, QByteArray data);
    void done();
    void showReminder();
    void saveReminder();

protected:
    void createConfigurationInterface(KConfigDialog *parent);
    bool eventFilter ( QObject * obj, QEvent * event ) ;
private:
    KIO::Job *m_job;
    QByteArray m_data;
    Plasma::ExtenderItem *  m_extender  ;
    Plasma::ExtenderItem * m_reminderExtender;

    Plasma::PushButton * m_buttonTranslate;
    Plasma::ToolButton * m_buttonChange;
    Plasma::TextEdit * m_sourceText;
    Plasma::TextEdit * m_destText;
    Plasma::Label * m_sourceLanguageLabel;
    Plasma::Label * m_destLanguageLabel;
    Plasma::ToolButton * m_sourceLanguageButton;
    Plasma::ToolButton * m_destLanguageButton;
    QGraphicsWidget *m_graphicsWidget;
    QGraphicsLinearLayout* bigLayout;
    LanguageModel * m_languageModel;

    Language m_sourceLanguage;
    Language m_destLanguage;

    Ui::LanguageForm m_ui;
    Ui::ReminderForm m_uiReminder;
    KIcon m_popupIcon;
    int m_animId;
    int m_currentButton;
    bool m_autoPaste;
    bool m_autoTranslate;
    int m_reminderDuration;
    int m_reminderFrenquency;
    QTimer * m_reminderTimer;
    Reminder * m_reminder;
    Plasma::Label * m_reminderSourceText;
    Plasma::Label * m_reminderTranslatedText;


};

// This is the command that links your applet to the .desktop file
K_EXPORT_PLASMA_APPLET(translatoid, KTranslatoid)
#endif

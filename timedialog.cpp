#include <QtGui/QtGui>
#include <QtWidgets/QtWidgets>

#include "timedialog.h"

TimeDialog::TimeDialog( QWidget* parent ): QDialog( parent )
{
    QGridLayout* timeLayout = new QGridLayout;
    timecheck = new QCheckBox( tr( "Period Alarm" ) );
    timeLayout->addWidget( timecheck, 0, 0 );
    time = new QSpinBox;
    time->setRange( 1, 300 );
    time->setSuffix( " Minutes" );
    time->setValue( 15 );
    timeLayout->addWidget( time, 1, 1 );
    timelabel = new QLabel( tr( "Alarm Every" ) );
    timeLayout->addWidget( timelabel, 1, 0 );
    timegroup = new QGroupBox( tr( "Timer Setting" ) );
    timegroup->setLayout( timeLayout );
    timeLayout->setColumnStretch( 2, 1 );
    timeLayout->setRowStretch( 2, 1 );
    QVBoxLayout* clockLayout = new QVBoxLayout;
    clockcheck = new QCheckBox( tr( "Alarm Clock" ) );
    clockLayout->addWidget( clockcheck );
    clock = new QTimeEdit();
    clock->setDisplayFormat( "hh:mm:ss" );
    clockLayout->addWidget( clock );
    clockgroup = new QGroupBox( tr( "Clock Setting" ) );
    clockgroup->setLayout( clockLayout );
    QGridLayout* buttonLayout = new QGridLayout;
    ok = createButton( tr( "&OK" ), this, SLOT( accept() ) );
    cancel = createButton( tr( "&Cancel" ), this, SLOT( reject() ) );
    buttonLayout->addWidget( ok, 0, 0 );
    buttonLayout->addWidget( cancel, 0, 1 );
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget( timegroup );
    mainLayout->addWidget( clockgroup );
    mainLayout->addLayout( buttonLayout );
    mainLayout->addSpacing( 2 );
    mainLayout->addStretch( 1 );
    setLayout( mainLayout );
    setWindowTitle( tr( "Alarm Setting" ) );
    connect( ok, SIGNAL( clicked() ), this, SLOT( writeSettings() ) );
    connect( clockcheck, SIGNAL( clicked() ), this, SLOT( setclockstate() ) );
    connect( timecheck, SIGNAL( clicked() ), this, SLOT( settimestate() ) );
    clocktimer = new QTimer( this );
    periodtimer = new QTimer( this );
    connect( clocktimer, SIGNAL( timeout() ), this, SLOT( clockAlarmMessage() ) );
    connect( periodtimer, SIGNAL( timeout() ), this, SLOT( timeAlarmMessage() ) );
    readSettings();
}

QPushButton* TimeDialog::createButton( const QString& text, QWidget* receiver,
                                       const char* member )
{
    QPushButton* button = new QPushButton( text );
    button->connect( button, SIGNAL( clicked() ), receiver, member );
    return button;
}

void TimeDialog::writeSettings()
{
    QSettings settings( "Qreadbook", "Configuration" );
    if ( clockcheck->checkState() == Qt::Checked )
    {
        settings.setValue( "clockflag", Qt::Checked );
        clocktimer->start( 1000 );
    }
    else if ( clockcheck->checkState() == Qt::Unchecked )
    {
        settings.setValue( "clockflag", Qt::Unchecked );
        clocktimer->stop();
    }
    if ( timecheck->checkState() == Qt::Checked )
    {
        settings.setValue( "timeflag", Qt::Checked );
        periodtimer->start( time->value() * 60 * 1000 );
    }
    else if ( timecheck->checkState() == Qt::Unchecked )
    {
        settings.setValue( "timeflag", Qt::Unchecked );
        periodtimer->stop();
    }
    settings.setValue( "timeperiod", time->value() );
    settings.setValue( "clockvalue", clock->time().toString() );
}

void TimeDialog::readSettings()
{
    QSettings settings( "Qreadbook", "Configuration" );
    int flag;
    flag = settings.value( "clockflag", Qt::Unchecked ).toInt();
    if ( flag == Qt::Unchecked )
    {
        clock->setTime( QTime::fromString( settings.value( "clockvalue", "00:00:00" ).toString(), "hh:mm:ss" ) );
        clockcheck->setCheckState( Qt::Unchecked );
        clock->setEnabled( false );
        clocktimer->stop();
    }
    else if ( flag == Qt::Checked )
    {
        clock->setTime( QTime::fromString( settings.value( "clockvalue", "00:00:00" ).toString(), "hh:mm:ss" ) );
        clockcheck->setCheckState( Qt::Checked );
        clock->setEnabled( true );
        clocktimer->start( 1000 );
    }
    flag = settings.value( "timeflag", Qt::Unchecked ).toInt();
    if ( flag == Qt::Unchecked )
    {
        time->setValue( settings.value( "timeperiod", 15 ).toInt() );
        timecheck->setCheckState( Qt::Unchecked );
        time->setEnabled( false );
        periodtimer->stop();
    }
    else if ( flag == Qt::Checked )
    {
        time->setValue( settings.value( "timeperiod", 15 ).toInt() );
        timecheck->setCheckState( Qt::Checked );
        time->setEnabled( true );
        periodtimer->start( time->value() * 60 * 1000 );
    }
}

void TimeDialog::setclockstate()
{
    if ( clockcheck->checkState() == Qt::Checked )
    {
        clock->setEnabled( true );
    }
    else if ( clockcheck->checkState() == Qt::Unchecked )
    {
        clock->setEnabled( false );
    }
}

void TimeDialog::settimestate()
{
    if ( timecheck->checkState() == Qt::Checked )
    {
        time->setEnabled( true );
    }
    else if ( timecheck->checkState() == Qt::Unchecked )
    {
        time->setEnabled( false );
    }
}


void TimeDialog::clockAlarmMessage()
{
    QString about = "Please relax yourself and your eyes for a while:)";
    if ( QTime::currentTime().toString() == clock->time().toString() )
    {
        QMessageBox::information( this, "QReadBook - Clock Alarm", about , QMessageBox::Ok );
    }
}

void TimeDialog::timeAlarmMessage()
{
    QString about = "Please relax yourself and your eyes for a while:)";
    QMessageBox::information( this,
                              "QReadBook - Period Alarm", about , QMessageBox::Ok );
}

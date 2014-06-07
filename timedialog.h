#ifndef TIMEDIALOG_H
#define TIMEDIALOG_H

#include <QtWidgets/QDialog>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTimeEdit>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>

class TimeDialog : public QDialog
{
        Q_OBJECT

    public:
        TimeDialog( QWidget* parent = 0 );

    protected:
        //void closeEvent(QCloseEvent *event);

    private slots:

        void writeSettings();
        void setclockstate();
        void settimestate();
        void clockAlarmMessage();
        void timeAlarmMessage();

    private:
        QCheckBox* timecheck, *clockcheck;
        QLabel* timelabel, *clocklabel;
        QSpinBox* time;
        QGroupBox* timegroup, *clockgroup;
        QTimeEdit* clock;
        QPushButton* ok, *cancel;
        QTimer* clocktimer, *periodtimer;

        QPushButton* createButton( const QString& text, QWidget* receiver,
                                   const char* member );


        void readSettings();

};

#endif

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QDirModel>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QSystemTrayIcon>
#include <QtWidgets/QMessageBox>

#include "timedialog.h"

class QAction;
class QMenu;
class QTextEdit;

class MainWindow : public QMainWindow
{
        Q_OBJECT

    public:
        MainWindow();
        MainWindow( const QString& fileName );
        const char* codecstr;

    protected:
        void closeEvent( QCloseEvent* event );

    private slots:
        void open();
        void about();
        void fullScreenModeSlot();
        void swithchControlSlot();
        void fontSelectSlot();
        void fontColorSlot();
        void bgColorSlot();
        void autoScrollSlot();
        void addStepSlot();
        void minusStepSlot();
        void dirtreeDisplaySlot();
        void dbclickopenfileSlot( const QModelIndex& );
        void minitotraySlot();
        void dbclicktraySlot( QSystemTrayIcon::ActivationReason reason );
        void opentimedialogSlot();

    private:
        void init();
        void createActions();
        void createTrayIcon();
        void createMenus();
        void createStatusBar();
        void readSettings();
        void writeSettings();
        bool maybeSave();
        void loadFile( const QString& fileName );
        bool saveFile( const QString& fileName );
        void setCurrentFile( const QString& fileName );
        QString strippedName( const QString& fullFileName );
        MainWindow* findMainWindow( const QString& fileName );

        QSplitter* splitter;
        QDirModel* dirmodel;
        QTreeView* tree;
        QTextEdit* textEdit;
        QString curFile;
        bool isUntitled;
        bool fullscreen;
        bool mousecontrol;
        int step;
        int pagestep;
        QColor* fontcolor, *bgcolor;
        QFont* font;
        QSystemTrayIcon* trayIcon;
        QString pathfilename;

        QTimer* timer;
        TimeDialog*  timedialog;

        QMenu* fileMenu;
        QMenu* configMenu;
        QMenu* helpMenu;
        QMenu* controlMenu;

        QMenu* trayIconMenu;

        QAction* openAct;

        QAction* closeAct;

        QAction* aboutAct;
        QAction* aboutQtAct;
        //added by fangweiwei
        QAction* fontAct;
        QAction* fontcolorAct;
        QAction* bgcolorAct;
        QAction* view_fullScreenAct;
        QAction* switchControlAct;  //mouse or autoscroll
        QAction* addstepAct;
        QAction* minusstepAct;
        QAction* lefttreehideAct;
        QAction* clockdialogAct;

        QAction* minimizeAct, *restoreAct;
};

#endif

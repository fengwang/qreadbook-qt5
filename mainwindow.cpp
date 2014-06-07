#include <QtCore/QtCore>
#include <QtWidgets/QtWidgets>
#include <QtGui/QtGui>

#include "mainwindow.h"

MainWindow::MainWindow()
{
    init();
    setCurrentFile( "" );
}

MainWindow::MainWindow( const QString& fileName )
{
    init();
    loadFile( fileName );
}

void MainWindow::closeEvent( QCloseEvent* event )
{
    writeSettings();
    event->accept();
    qApp->quit();
}


void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName( this,
                       "QReadBook - Open File",
                       "",
                       "Plain Text Files (*.txt)" );;
    if ( !fileName.isEmpty() )
    {
        MainWindow* existing = findMainWindow( fileName );
        if ( existing )
        {
            existing->show();
            existing->raise();
            existing->activateWindow();
            return;
        }
        else
        {
            QSettings settings( "Qreadbook", "Configuration" );
            if ( pathfilename != tr( "" ) )
                settings.setValue( pathfilename, textEdit->verticalScrollBar()->value() );
            pathfilename = fileName;
            loadFile( fileName );
        }
    }
}

void MainWindow::about()
{
    QString about = "QReadBook is a small and simple book reader programmed in C++ using the Qt4.3 toolkit.\n\nHomepage: http://code.google.com/p/qreadbook\n\nAuthor: Weiwei Fang (http://hi.baidu.com/vvfang)";
    QMessageBox::information( this,
                              "QReadBook - About", about , QMessageBox::Ok );
}


void MainWindow::init()
{
    setAttribute( Qt::WA_DeleteOnClose );
    isUntitled = true;
    fullscreen = false;
    mousecontrol = true;
    step = 2;
    splitter = new QSplitter( Qt::Horizontal );
    tree = new QTreeView( splitter );
    dirmodel = new QDirModel();
    tree->setModel( dirmodel );
    tree->setColumnHidden( 1, true );
    tree->setColumnHidden( 3, true );
    splitter->addWidget( tree );
    tree->hide();
    timedialog = new TimeDialog( this );
    pathfilename = tr( "" );
    font = new QFont();
    textEdit = new QTextEdit;
    splitter->addWidget( textEdit );
    splitter->setStretchFactor( 1, 1 );
    setCentralWidget( splitter );
    setWindowIcon( QIcon( ":/images/qreader.png" ) );
    setMouseTracking( true );
    textEdit->setAutoFillBackground( true );
    textEdit->setReadOnly( true );
    trayIcon = new QSystemTrayIcon( this );
    trayIcon->setIcon( QIcon( ":/images/qreader.png" ) );
    createActions();
    createTrayIcon();
    createMenus();
    createStatusBar();
    readSettings();
}

void MainWindow::createActions()
{
    openAct = new QAction( tr( "&Open..." ), this );
    openAct->setShortcut( tr( "Ctrl+O" ) );
    openAct->setStatusTip( tr( "Open an existing file" ) );
    connect( openAct, SIGNAL( triggered() ), this, SLOT( open() ) );
    closeAct = new QAction( tr( "&Close" ), this );
    closeAct->setShortcut( QKeySequence( Qt::Key_Escape ) );
    closeAct->setStatusTip( tr( "Close this window" ) );
    connect( closeAct, SIGNAL( triggered() ), this, SLOT( close() ) );
    aboutAct = new QAction( tr( "&About" ), this );
    aboutAct->setShortcut ( QKeySequence( Qt::Key_F1 ) );
    aboutAct->setStatusTip( tr( "Show the application's About box" ) );
    connect( aboutAct, SIGNAL( triggered() ), this, SLOT( about() ) );
    aboutQtAct = new QAction( tr( "About &Qt" ), this );
    aboutQtAct->setStatusTip( tr( "Show the Qt library's About box" ) );
    connect( aboutQtAct, SIGNAL( triggered() ), qApp, SLOT( aboutQt() ) );
    fontAct = new QAction( tr( "Font" ), this );
    fontAct->setStatusTip( tr( "Font" ) );
    fontAct->setShortcut ( QKeySequence( Qt::Key_F2 ) );
    connect( fontAct, SIGNAL( triggered() ), this, SLOT( fontSelectSlot() ) );
    fontcolorAct = new QAction( tr( "Font Color" ), this );
    fontcolorAct->setStatusTip( tr( "Font Color" ) );
    fontcolorAct->setShortcut( QKeySequence( Qt::Key_F3 ) );
    connect( fontcolorAct, SIGNAL( triggered() ), this, SLOT( fontColorSlot() ) );
    bgcolorAct = new QAction( tr( "Background Color" ), this );
    bgcolorAct->setStatusTip( tr( "Background Color" ) );
    bgcolorAct->setShortcut( QKeySequence( Qt::Key_F4 ) );
    connect( bgcolorAct, SIGNAL( triggered() ), this, SLOT( bgColorSlot() ) );
    switchControlAct = new QAction( "Switch Control", this );
    switchControlAct->setShortcut ( QKeySequence( Qt::Key_F5 ) );
    switchControlAct->setStatusTip( "Switch mouse/autoscroll control of QReadBook" );
    connect( switchControlAct, SIGNAL( triggered() ), this , SLOT( swithchControlSlot() ) );
    addstepAct = new QAction( "AddStep", this );
    addstepAct->setShortcut ( QKeySequence( Qt::Key_F6 ) );
    connect( addstepAct, SIGNAL( triggered() ), this , SLOT( addStepSlot() ) );
    minusstepAct = new QAction( "MinusStep", this );
    minusstepAct->setShortcut ( QKeySequence( Qt::Key_F7 ) );
    connect( minusstepAct, SIGNAL(  triggered() ), this , SLOT( minusStepSlot() ) );
    lefttreehideAct = new QAction( "Directory Display", this );
    lefttreehideAct->setShortcut ( QKeySequence( Qt::Key_Tab ) );
    connect( lefttreehideAct, SIGNAL(  triggered() ), this , SLOT( dirtreeDisplaySlot() ) );
    view_fullScreenAct = new QAction( "Fullscreen Mode", this );
    view_fullScreenAct->setShortcut ( QKeySequence( Qt::Key_F11 ) );
    view_fullScreenAct->setCheckable( true );
    view_fullScreenAct->setStatusTip( "Enables the fullscreen mode of QReadBook" );
    connect( view_fullScreenAct, SIGNAL( toggled( bool ) ), this , SLOT(  fullScreenModeSlot() ) );
    clockdialogAct = new QAction( "Alarm Setting", this );
    clockdialogAct->setShortcut( QKeySequence( Qt::Key_F8 ) );
    connect( clockdialogAct, SIGNAL(  triggered() ), this , SLOT( opentimedialogSlot() ) );
    timer = new QTimer( this );
    connect( timer, SIGNAL( timeout() ), this, SLOT( autoScrollSlot() ) );
    connect( tree, SIGNAL( doubleClicked( const QModelIndex& ) ), this, SLOT( dbclickopenfileSlot( const QModelIndex& ) ) );
    minimizeAct = new QAction( tr( "Tray<->Window" ), this );
    minimizeAct->setShortcut ( QKeySequence( Qt::Key_F12 ) );
    minimizeAct->setStatusTip( "Minimize QReadBook to tray" );
    connect( minimizeAct, SIGNAL( triggered() ), this, SLOT( minitotraySlot() ) );
    connect( trayIcon, SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ), this, SLOT( dbclicktraySlot( QSystemTrayIcon::ActivationReason ) ) );
}


void MainWindow::opentimedialogSlot()
{
    if ( timedialog->exec() == QDialog::Accepted )
    {
        //printf("fuck\n");
    }
}

void MainWindow::dbclicktraySlot( QSystemTrayIcon::ActivationReason reason )
{
    switch ( reason )
    {
        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::DoubleClick:
            minitotraySlot();
            break;
        case QSystemTrayIcon::MiddleClick:
            break;
        default:
            ;
    }
}

void MainWindow::minitotraySlot()
{
    if ( isHidden() )
    {
        show();
        trayIcon->hide();
    }
    else
    {
        hide();
        trayIcon->show();
    }
}

void MainWindow::createTrayIcon()
{
    trayIconMenu = new QMenu( this );
    trayIconMenu->addAction( minimizeAct );
    trayIconMenu->addAction( closeAct );
    trayIconMenu->addSeparator();
    trayIconMenu->addAction( aboutAct );
    trayIcon->setContextMenu( trayIconMenu );
}

void MainWindow::dbclickopenfileSlot( const QModelIndex& )
{
    QString  file;
    QString  pathfile;
    QModelIndex fileIndex, parentIndex;
    fileIndex = tree->currentIndex();
    file = tree->currentIndex().data( Qt::DisplayRole ).toString();
    pathfile = file;
    parentIndex = tree->currentIndex().parent();
    while ( parentIndex != tree->rootIndex() )
    {
        if ( parentIndex.data( Qt::DisplayRole ).toString() != "/" )
            pathfile.prepend( "/" );
        pathfile.prepend( parentIndex.data( Qt::DisplayRole ).toString() );
        parentIndex = parentIndex.parent();
    }
    if ( !dirmodel->isDir( fileIndex ) && file.right( 4 ) == ".txt" )
    {
        if ( !pathfile.isEmpty() )
        {
            MainWindow* existing = findMainWindow( pathfile );
            if ( existing )
            {
                existing->show();
                existing->raise();
                existing->activateWindow();
                return;
            }
            else
            {
                QSettings settings( "Qreadbook", "Configuration" );
                if ( pathfilename != tr( "" ) )
                    settings.setValue( pathfilename, textEdit->verticalScrollBar()->value() );
                pathfilename = pathfile;
                loadFile( pathfile );
            }
        }
        tree->hide();
    }
}

void MainWindow::dirtreeDisplaySlot()
{
    if ( !tree->isHidden() )
    {
        tree->hide();
    }
    else
    {
        tree->show();
    }
}

void MainWindow::fontSelectSlot()
{
    bool ok;
    QFont newfont = *font;
    font = new QFont( QFontDialog::getFont( &ok, newfont, this ) );
    if ( ok )
        textEdit->setFont( *font );
}

void MainWindow::fontColorSlot()
{
    QColor color = QColorDialog::getColor( textEdit->textColor(), this );
    if ( color.isValid() )
    {
        fontcolor = new QColor( color );
        QPalette palette;
        palette.setColor( QPalette::Text, color );
        palette.setColor( QPalette::Base, *bgcolor );
        textEdit->setPalette( palette );
    }
}

void MainWindow::bgColorSlot()
{
    QColor color = QColorDialog::getColor( textEdit->textColor(), this );
    if ( color.isValid() )
    {
        bgcolor = new QColor( color );
        QPalette palette;
        palette.setColor( QPalette::Base, color );
        palette.setColor( QPalette::Text, *fontcolor );
        textEdit->setPalette( palette );
    }
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu( tr( "&File" ) );
    fileMenu->addAction( openAct );
    fileMenu->addSeparator();
    fileMenu->addAction( closeAct );
    configMenu = menuBar()->addMenu( tr( "&View" ) );
    configMenu->addAction( fontAct );
    configMenu->addAction( fontcolorAct );
    configMenu->addAction( bgcolorAct );
    configMenu->addSeparator();
    configMenu->addAction( lefttreehideAct );
    configMenu->addSeparator();
    configMenu->addAction( view_fullScreenAct );
    controlMenu = menuBar()->addMenu( tr( "&Control" ) );
    controlMenu->addAction( switchControlAct );
    controlMenu->addAction( addstepAct );
    controlMenu->addAction( minusstepAct );
    controlMenu->addSeparator();
    controlMenu->addAction( clockdialogAct );
    controlMenu->addSeparator();
    controlMenu->addAction( minimizeAct );
    menuBar()->addSeparator();
    helpMenu = menuBar()->addMenu( tr( "&Help" ) );
    helpMenu->addAction( aboutAct );
    helpMenu->addAction( aboutQtAct );
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage( tr( "Ready" ) );
}


void MainWindow::swithchControlSlot()
{
    if ( mousecontrol )
    {
        if ( !fullscreen )
            statusBar()->showMessage( tr( "Now: AutoScroll Control (step:%1)" ).arg( step ) );
        mousecontrol = false;
        pagestep = textEdit->verticalScrollBar()->pageStep();
        timer->start( 100 );
    }
    else
    {
        if ( !fullscreen )
            statusBar()->showMessage( tr( "Now: Mouse Control" ) );
        mousecontrol = true;
        timer->stop();
        textEdit->verticalScrollBar()->setPageStep( pagestep );
    }
}

void MainWindow::addStepSlot()
{
    if ( !mousecontrol && step < 1000 )
        step = step * 2;
    if ( !fullscreen && !mousecontrol )
        statusBar()->showMessage( tr( "Now: AutoScroll Control (step:%1)" ).arg( step ) );
}

void MainWindow::minusStepSlot()
{
    if ( !mousecontrol && step > 1 )
        step = step / 2;
    if ( !fullscreen && !mousecontrol )
        statusBar()->showMessage( tr( "Now: AutoScroll Control (step:%1)" ).arg( step ) );
}

void MainWindow::autoScrollSlot()
{
    QScrollBar* sb = textEdit->verticalScrollBar();
    sb->setPageStep( step );
    sb->setValue( sb->pageStep() + sb->value() );
}

void MainWindow::fullScreenModeSlot()
{
    if ( !fullscreen )
    {
        //menuBar()->hide();
        statusBar()->hide();
        fullscreen = true;
    }
    else
    {
        //menuBar()->show();
        statusBar()->show();
        fullscreen = false;
    }
    this->setWindowState( this->windowState() ^ Qt::WindowFullScreen );
}

void MainWindow::readSettings()
{
    QSettings settings( "Qreadbook", "Configuration" );
    QPoint pos = settings.value( "pos", QPoint( 200, 200 ) ).toPoint();
    QSize size = settings.value( "size", QSize( 400, 400 ) ).toSize();
    move( pos );
    resize( size );
    fontcolor = new QColor( settings.value( "fr", 0 ).toInt(), settings.value( "fg", 0 ).toInt(), settings.value( "fb", 0 ).toInt(), settings.value( "fa", 255 ).toInt() );
    bgcolor = new QColor( settings.value( "br", 255 ).toInt(), settings.value( "bg", 255 ).toInt(), settings.value( "bb", 255 ).toInt(), settings.value( "ba", 255 ).toInt() );
    QPalette palette;
    palette.setColor( QPalette::Text, *fontcolor );
    palette.setColor( QPalette::Base, *bgcolor );
    textEdit->setPalette( palette );
    font->setFamily( settings.value( "fontfamily", font->defaultFamily() ).toString() );
    font->setPointSize( settings.value( "fontpointsize", 10 ).toInt() );
    font->setBold( settings.value( "fontbold", 0 ).toBool() );
    font->setItalic( settings.value( "fontitalic", 0 ).toBool() );
    textEdit->setFont( *font );
}

void MainWindow::writeSettings()
{
    QSettings settings( "Qreadbook", "Configuration" );
    settings.setValue( "pos", pos() );
    settings.setValue( "size", size() );
    int r, g, b, a;
    fontcolor->getRgb( &r, &g, &b, &a );
    settings.setValue( "fr", r );
    settings.setValue( "fg", g );
    settings.setValue( "fb", b );
    settings.setValue( "fa", a );
    bgcolor->getRgb( &r, &g, &b, &a );
    settings.setValue( "br", r );
    settings.setValue( "bg", g );
    settings.setValue( "bb", b );
    settings.setValue( "ba", a );
    settings.setValue( "fontfamily", font->family() );
    settings.setValue( "fontpointsize", font->pointSize() );
    settings.setValue( "fontbold", font->bold() );
    settings.setValue( "fontitalic", font->italic() );
    if ( pathfilename != tr( "" ) )
        settings.setValue( pathfilename, textEdit->verticalScrollBar()->value() );
}

void MainWindow::loadFile( const QString& fileName )
{
    QFile file( fileName );
    if ( !file.open( QFile::ReadOnly | QFile::Text ) )
    {
        QMessageBox::warning( this, tr( "QReadBook" ),
                              tr( "Cannot read file %1:\n%2." )
                              .arg( fileName )
                              .arg( file.errorString() ) );
        return;
    }
    QTextStream in( &file );
    QTextCodec* codec = QTextCodec::codecForName( codecstr );
    in.setCodec( codec );
    QApplication::setOverrideCursor( Qt::WaitCursor );
    textEdit->setPlainText( in.readAll() );
    QSettings settings( "Qreadbook", "Configuration" );
    int line = settings.value( pathfilename, -1 ).toInt();
    if ( line != -1 )
    {
        textEdit->verticalScrollBar()->setValue( line );
    }
    QApplication::restoreOverrideCursor();
    setCurrentFile( fileName );
    statusBar()->showMessage( tr( "File loaded" ), 2000 );
}

void MainWindow::setCurrentFile( const QString& fileName )
{
    isUntitled = fileName.isEmpty();
    if ( isUntitled )
    {
        curFile = tr( "Please Load Your Book" );
    }
    else
    {
        curFile = QFileInfo( fileName ).canonicalFilePath();
    }
    textEdit->document()->setModified( false );
    setWindowModified( false );
    setWindowTitle( tr( "%1[*] - %2" ).arg( strippedName( curFile ) )
                    .arg( tr( "QReadBook" ) ) );
}

QString MainWindow::strippedName( const QString& fullFileName )
{
    return QFileInfo( fullFileName ).fileName();
}

MainWindow* MainWindow::findMainWindow( const QString& fileName )
{
    QString canonicalFilePath = QFileInfo( fileName ).canonicalFilePath();
    foreach ( QWidget * widget, qApp->topLevelWidgets() )
    {
        MainWindow* mainWin = qobject_cast<MainWindow*>( widget );
        if ( mainWin && mainWin->curFile == canonicalFilePath )
            return mainWin;
    }
    return 0;
}

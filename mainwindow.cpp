#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QSettings>
#include <QMessageBox>
#include <QCloseEvent>

#include "mainwindow.h"
#include "diagram.h"
#include "file.h"

MainWindow::MainWindow(QString name, QWidget *parent) :
    QMainWindow(parent)
{
    diagram = new Diagram;
    file = new File(diagram);
    connect(diagram, SIGNAL(changed()), file, SLOT(changed()));

    newAction = new QAction(tr("N&ew"),this);
    newAction->setShortcut(tr("Ctrl+N"));
    connect(newAction, SIGNAL(triggered()), file, SLOT(add()));

    openAction = new QAction(tr("&Open"),this);
    openAction->setShortcut(tr("Ctrl+O"));
    connect(openAction, SIGNAL(triggered()), file, SLOT(load()));

    saveAction = new QAction(tr("&Save"),this);
    saveAction->setShortcut(tr("Ctrl+S"));
    connect(saveAction, SIGNAL(triggered()), file, SLOT(save()));

    saveAsAction = new QAction(tr("&Save as ..."),this);
    saveAsAction->setShortcut(tr("Alt+S"));
    connect(saveAsAction, SIGNAL(triggered()), file, SLOT(saveAs()));

    menuBar()->addAction(newAction);
    menuBar()->addAction(openAction);
    menuBar()->addAction(saveAction);
    menuBar()->addAction(saveAsAction);
    menuBar()->addSeparator();

    setCentralWidget(diagram);

    connect(file, SIGNAL(fileNameChanged(QString)), this, SLOT(setHeader(QString)));
    settings = new QSettings("DemySoft", "SimpleMindMap");
    if(!name.isEmpty()){
        file->setFileName(name);
        file->load(true);
    }
    else if(settings->value("file").isValid()){
        file->setFileName(settings->value("file").toString());
        file->load(true);
    }
    if(settings->value("geometry").isValid()) setGeometry(settings->value("geometry").toRect());
}

MainWindow::~MainWindow()
{
    if(!file->getFileName().isEmpty()) settings->setValue("file", file->getFileName());
    settings->setValue("geometry", geometry());
}

void MainWindow::closeEvent(QCloseEvent *e){
    if(file->isSaved()) e->accept();
    else switch(QMessageBox::question(this, tr("Close program"), QString(tr("Save ""%1"" ?")).arg(file->getFileName()),
            QMessageBox::Yes | QMessageBox::Cancel | QMessageBox::No)){
    case QMessageBox::Yes: file->save(); e->accept(); break;
    case QMessageBox::Cancel:  e->ignore(); break;
    default: e->accept();
    }
}

void MainWindow::setHeader(QString s)
{
    setWindowTitle(tr("Simple MindMap ... ")+s);
}

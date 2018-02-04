#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class File;
class Diagram;
class QAction;
class QSettings;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QMenu *fileMenu;
    File *file;
    Diagram *diagram;

    QAction *newAction, *saveAction, *saveAsAction, *openAction;
    QSettings *settings;

public:
    explicit MainWindow(QString name=QString(), QWidget *parent = 0);
    ~MainWindow();
public slots:
    void setHeader(QString s);
    void closeEvent(QCloseEvent *e);
};

#endif // MAINWINDOW_H

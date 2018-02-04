#ifndef FILE_H
#define FILE_H

#include <QObject>
#include "diagram.h"

class File : public QObject
{
    Q_OBJECT

    Diagram *map;
    QString fileName;
    bool saved;

public:
    explicit File(Diagram *diagram);
    QString getFileName() { return fileName; }
    void setFileName(QString name) { fileName=name; }
    bool isSaved(){ return saved; }

public slots:
    void save();
    void saveAs();
    void load(bool last=false);
    void add();
    void changed() { saved=false; }
signals:
    void fileNameChanged(QString s);
};

#endif // FILE_H

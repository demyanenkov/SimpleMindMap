#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

#include "file.h"

File::File(Diagram *diagram)
{
    map = diagram;
    fileName = "default.smm";
    saved = true;
}

void File::save()
{
    if(fileName.isEmpty()) fileName = QFileDialog::getSaveFileName(map,tr("Save File"), "","*.smm");
    if(fileName.isEmpty()) return;
    if(!fileName.contains(".smm")) fileName+=".smm";

    QFile file(fileName);
    if(!file.open(QFile::WriteOnly | QFile::Text)) return;
    emit fileNameChanged(QFileInfo(fileName).fileName());

    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("mindmap");
    int cnt = 0;

    xmlWriter.writeStartElement("scrollstate");
    xmlWriter.writeAttribute("zoom",QString("%1").arg(map->zoom));
    xmlWriter.writeAttribute("x",QString("%1").arg(map->view->mapToScene(map->view->width()/2,0).x()));
    xmlWriter.writeAttribute("y",QString("%1").arg(map->view->mapToScene(0,map->view->height()/2).y()));
    xmlWriter.writeEndElement();

    QList<QGraphicsItem *> items = map->scene->items();

    xmlWriter.writeStartElement("topics");
    QList<Node *> nodes;
    if(!items.isEmpty()) items.back();
    foreach(QGraphicsItem *item,items)
        if(item->type() == (QGraphicsItem::UserType+2)){
            Node *node = dynamic_cast<Node *>(item);
            nodes.insert(0,node);
        }

    foreach(Node *node, nodes){
        xmlWriter.writeStartElement("topic");
        xmlWriter.writeAttribute("id",QString("%1").arg(cnt++));
        xmlWriter.writeAttribute("x",QString().sprintf("%5.2f",node->pos().x()));
        xmlWriter.writeAttribute("y",QString().sprintf("%5.2f",node->pos().y()));
        xmlWriter.writeAttribute("color",node->outlineColor().name());
        xmlWriter.writeAttribute("text", node->toPlainText());
        xmlWriter.writeEndElement();
    };
    xmlWriter.writeEndElement();

    xmlWriter.writeStartElement("relations");
    foreach(QGraphicsItem *item,items){
        if(item->type() == (QGraphicsItem::UserType+1)){
            xmlWriter.writeStartElement("relation");
            Link *link = dynamic_cast<Link *>(item);
            xmlWriter.writeAttribute("source", QString("%1").arg(nodes.indexOf(link->fromNode())));
            xmlWriter.writeAttribute("target", QString("%1").arg(nodes.indexOf(link->toNode())));
            xmlWriter.writeAttribute("color",link->color().name());
            xmlWriter.writeEndElement();
        }
    };
    xmlWriter.writeEndElement();
    xmlWriter.writeEndDocument();
    file.close();

    saved=true;
}

void File::saveAs()
{
    QString old=fileName;
    fileName.clear();
    save();
    if(fileName.isEmpty()) { fileName=old; return; }
}

void File::load(bool last)
{    
    static QString color[]={"#FF0000", "#00FF00", "#0000FF", "#CCCC00", "#FF00FF", "#00FFFF", "#88FF00", "#FF0088", "#8800FF"};

    if(!last || fileName.isEmpty())
        fileName = QFileDialog::getOpenFileName(map,tr("Load File"), "","SMM files (*.smm*);; All Files (*)");

    map->scene->clear();

    QFile file(fileName);
    if(!file.open(QFile::ReadOnly | QFile::Text)) return;
    emit fileNameChanged(QFileInfo(fileName).fileName());

    QXmlStreamReader reader;
    reader.setDevice(&file);
    reader.readNext();
    QList <Node *> nodes;

    while(!reader.atEnd()){
        if(reader.isEndElement()) reader.readNext();
        if(reader.name() == "topic"){
            Node* node = new Node(0);
            node->setText(reader.attributes().value("text").toString());
            node->setPos(QPoint(reader.attributes().value("x").toString().toDouble(),reader.attributes().value("y").toString().toDouble()));
            if(!reader.attributes().value("color").isEmpty())
                node->setOutlineColor(reader.attributes().value("color").toString());
            else if(!reader.attributes().value("colorinfo").isEmpty())
                node->setOutlineColor(color[reader.attributes().value("colorinfo").toString().toInt()]);
            else node->setOutlineColor(nodes.at(reader.attributes().value("parent").toString().toInt())->outlineColor());

            nodes.append(node);
            map->addItem(node);
            int parent;
            if(!reader.attributes().value("parent").isEmpty() && (parent=reader.attributes().value("parent").toString().toInt())>-1 && nodes.at(parent)){
                Link *link = new Link(nodes.at(parent), node);
                link->setColor(node->outlineColor());
                map->addItem(link);
            }
        }
        else if(reader.name() == "relation"){
            Link* link = new Link(nodes.at(reader.attributes().value("source").toString().toInt()),nodes.at(reader.attributes().value("target").toString().toInt()));
            link->setColor(reader.attributes().value("color").toString());
            map->addItem(link);
        }
        else if(reader.name() == "scrollstate")
            map->view->centerOn(reader.attributes().value("x").toString().toDouble(),reader.attributes().value("y").toString().toDouble());
        reader.readNext();
    }
}

void File::add()
{
    if(!saved){
        int ret = QMessageBox::question(map, tr("Create new"),QString(tr("Save ""%1"" ?")).arg(fileName),
            QMessageBox::Yes | QMessageBox::Cancel | QMessageBox::No);
        if(ret == QMessageBox::Yes) save();
        if(ret == QMessageBox::Cancel) return;
    }

    fileName.clear();
    saved=true;
    map->sceneNew();
}

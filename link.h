#ifndef LINK_H
#define LINK_H

#include <QGraphicsLineItem>

class Node;

class Link : public QGraphicsLineItem
{    
    Node *myFromNode;
    Node *myToNode;

public:
    Link(Node *fromNode, Node *toNode, QColor color = Qt::black);
    ~Link();
    int type() const {return UserType + 1 ;}
    Node *fromNode() const;
    Node *toNode() const;

    void setColor(const QColor &color);
    QColor color() const;

    void trackNodes();
};

#endif

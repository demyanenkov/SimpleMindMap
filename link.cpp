#include <QtGui>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>

#include "link.h"
#include "node.h"

Link::Link(Node *fromNode, Node *toNode,  QColor color)
{
    myFromNode = fromNode;
    myToNode = toNode;

    myFromNode->addLink(this);
    myToNode->addLink(this);

    setFlags(QGraphicsItem::ItemIsSelectable);
    setZValue(-1);

    setColor(color);
    trackNodes();
}

Link::~Link()
{
    myFromNode->removeLink(this);
    myToNode->removeLink(this);
}

Node *Link::fromNode() const
{
    return myFromNode;
}

Node *Link::toNode() const
{
    return myToNode;
}

void Link::setColor(const QColor &color)
{
    if(color.isValid()) setPen(QPen(color, 1.5, Qt::DotLine));
}

QColor Link::color() const
{
    return pen().color();
}

void Link::trackNodes()
{
    QPointF from = myFromNode->pos()+myFromNode->boundingRect().center();
    QPointF to = myToNode->pos() + myToNode->boundingRect().center();
    setLine(QLineF(from, to));
}

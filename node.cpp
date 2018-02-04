#include <QtGui>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>

#include "link.h"
#include "node.h"

const int outlineR = 10;

Node::Node(Node *parent)
{
    if(parent) myOutlineColor = parent->outlineColor();
    else myOutlineColor = QColor("#5050FF");
    setOutlineColor(myOutlineColor);
    setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsScenePositionChanges);
}

Node::~Node()
{
    foreach (Link *link, myLinks) delete link;
}

void Node::setText(const QString &text)
{
    prepareGeometryChange();
    setPlainText(text);
    update();
}

QString Node::getText() const
{
    return toPlainText();
}

void Node::setTextColor(const QColor &color)
{
    setDefaultTextColor(color);
    update();
}

QColor Node::textColor() const
{
    return defaultTextColor();
}

void Node::setOutlineColor(const QColor &color)
{
    if(color.isValid()){
        myOutlineColor = color;
        myBackgroundColor = myOutlineColor.lighter(200);
    }
    update();
}

QColor Node::outlineColor() const
{
    return myOutlineColor;
}

void Node::setBackgroundColor(const QColor &color)
{
    myBackgroundColor = color;
    update();
}

QColor Node::backgroundColor() const
{
    return myBackgroundColor;
}

void Node::addLink(Link *link)
{
    myLinks.insert(link);
}

bool Node::isLinkTo(Node *node)
{
    foreach (Link *link, myLinks) if(link->toNode()==node || link->fromNode()==node) return true;
    return false;
}

void Node::removeLink(Link *link)
{
    myLinks.remove(link);
}

QRectF Node::boundingRect() const
{
    const int Margin = 1;
    return outlineRect().adjusted(-Margin, -Margin, +Margin, +Margin);
}

QPainterPath Node::shape() const
{
    QRectF rect = outlineRect();

    QPainterPath path;
    path.addRoundRect(rect, roundness(rect.width()),
                      roundness(rect.height()));
    return path;
}

void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * widget)
{
    QPen pen(myOutlineColor);
    if (option->state & QStyle::State_Selected) {
        pen.setStyle(Qt::DotLine);
        pen.setWidth(2);
    }
    else pen.setWidth(2);
    painter->setPen(pen);
    painter->setBrush(myBackgroundColor);

    QRectF rect = outlineRect();
    painter->drawRoundRect(rect, roundness(rect.width()),
                           roundness(rect.height()));
    painter->setPen(defaultTextColor());
    QGraphicsTextItem::paint(painter, option, widget);
}

void Node::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *e)
{
    if(e->modifiers()==Qt::ShiftModifier) emit linkTo(this);
    else edit();
}

void Node::edit(){
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.select(QTextCursor::Document);
    setTextInteractionFlags(Qt::TextEditorInteraction);
    setFocus();
    setTextCursor(cursor);
}

void Node::focusOutEvent(QFocusEvent *)
{
    setTextInteractionFlags(Qt::NoTextInteraction);
}

QVariant Node::itemChange(GraphicsItemChange change,
                          const QVariant &value)
{
    if(change == ItemPositionHasChanged)
        foreach (Link *link, myLinks) link->trackNodes();
    return QGraphicsItem::itemChange(change, value);
}

QRectF Node::outlineRect() const
{
    return QGraphicsTextItem::boundingRect();
}

int Node::roundness(double size) const
{
    return 200 * outlineR / int(size);
}

void Node::keyPressEvent(QKeyEvent *e)
{
    QGraphicsTextItem::keyPressEvent(e);
    foreach (Link *link, myLinks) link->trackNodes();
}

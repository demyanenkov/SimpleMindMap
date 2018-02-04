#ifndef NODE_H
#define NODE_H

#include <QGraphicsItem>
#include <QApplication>
#include <QColor>
#include <QSet>

class Link;

class Node : public QGraphicsTextItem
{
    Q_OBJECT

    QSet<Link *> myLinks;
    QColor myBackgroundColor;
    QColor myOutlineColor;

    QRectF outlineRect() const;
    int roundness(double size) const;

signals:
    void linkTo(Node *);

public:
    Node(Node *parent = 0);
    ~Node();
    int type() const {return UserType + 2 ;}

    void setText(const QString &text);
    QString getText() const;
    void setTextColor(const QColor &color);
    QColor textColor() const;
    void setOutlineColor(const QColor &color);
    QColor outlineColor() const;
    void setBackgroundColor(const QColor &color);
    QColor backgroundColor() const;

    void addLink(Link *link);
    bool isLinkTo(Node *);
    void removeLink(Link *link);
    void edit();

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *e);
    void focusOutEvent(QFocusEvent *);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    void keyPressEvent(QKeyEvent *e);
};

#endif

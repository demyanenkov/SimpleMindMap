#ifndef Diagram_H
#define Diagram_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QPair>

#include "node.h"
#include "link.h"

class Diagram : public QMainWindow
{
    Q_OBJECT

    friend class File;
    typedef QPair<Node *, Node *> NodePair;

    QMenu *editMenu;
    QAction
        *addNodeAction, *addLinkAction, *deleteAction, *setColorAction, *cutAction, *copyAction, *pasteAction,
        *bringToFrontAction, *sendToBackAction, *addTextAction, *zoomInAction, *zoomOutAction, *clearAction;

    QGraphicsScene *scene;
    QGraphicsView *view;

    int minZ, maxZ, zoom, seqNumber;
    Node *last, *last2;

    void createActions();
    void createMenus();
    void setZValue(int z);
    void setupNode(Node *node);
    Node *selectedNode() const;
    Node *lastNode() const;
    Link *selectedLink() const;
    NodePair selectedNodePair() const;

public slots:
    void linkTo(Node *);

private slots:
    void addNode();
    void addNode(QPointF pos);
    void addLink();
    void addItem(QGraphicsItem *);
    void setColor();
    void remove();
    void cut();
    void copy();
    void paste();
    void bringToFront();
    void sendToBack();
    void updateActions();
    void sceneNew();
    void zoomIn();
    void zoomOut();
    void mouseDoubleClickEvent(QMouseEvent *e);
    void selectionChanged();
    void addText();

public:
    Diagram();

signals:
    void changed();
};

#endif

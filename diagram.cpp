#include <QtGui>
#include <QColorDialog>
#include <QMenu>

#include <QColor>
#include <QPixmap>
#include <QPainter>
#include <QBrush>

#include "diagram.h"

Diagram::Diagram()
{
    scene = new QGraphicsScene(-2000, -2000, 4000, 4000);

    // Рисование сетки на заднем фоне
    scene->setBackgroundBrush(QBrush(QColor("#D0D0FF"), Qt::CrossPattern));

    connect(scene, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
    connect(scene, SIGNAL(selectionChanged()), this, SLOT(updateActions()));

    view = new QGraphicsView(scene);
    view->setDragMode(QGraphicsView::ScrollHandDrag);
    view->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    view->setContextMenuPolicy(Qt::ActionsContextMenu);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setResizeAnchor(QGraphicsView::AnchorViewCenter);
    setCentralWidget(view);

    createActions();
    createMenus();
    updateActions();

    sceneNew();
    view->scale(1.2, 1.2);
}

void Diagram::sceneNew()
{
    scene->clear();
    minZ = maxZ = 0;
    last = last2 = 0;
    seqNumber = 0;
    zoom = 100;
    view->scale(zoom/100.0,zoom/100.0);
}

void Diagram::addNode()
{
    addNode(view->mapToScene(view->geometry().center()));
}

void Diagram::addNode(QPointF pos)
{
    Node *node = new Node(last);

    node->setText(tr("Node %1").arg(seqNumber + 1));

    if(last) addItem(new Link(last, node, last->outlineColor()));
    last = node;
    setupNode(node);
    node->setPos(pos-QPointF(node->boundingRect().size().width(), node->boundingRect().size().height())/2);

    node->edit();
}

void Diagram::addLink()
{
    NodePair nodes = selectedNodePair();
    if (nodes == NodePair() || nodes.first->isLinkTo(nodes.second)) return;

    Link *link = new Link(nodes.first, nodes.second, nodes.first->outlineColor());
    addItem(link);
}

void Diagram::addItem(QGraphicsItem *item)
{
    scene->addItem(item);
    if(item->type()==Node().type()) {
        Node *node=(Node*)item;
        connect(node, SIGNAL(linkTo(Node*)), this, SLOT(linkTo(Node*)));
        connect(node, SIGNAL(xChanged()), this, SIGNAL(changed()));
        connect(node, SIGNAL(yChanged()), this, SIGNAL(changed()));
    }
}

void Diagram::setColor()
{
    QColor color;
    Node *node = selectedNode();
    Link *link = selectedLink();

    if(node){
        color = node->outlineColor();
        color = QColorDialog::getColor(color,this);
        node->setOutlineColor(color);
    }
    else if(link){
        color = link->color();
        color = QColorDialog::getColor(color,this);
        link->setColor(color);
    }

    emit changed();
}

void Diagram::remove()
{
    QList<QGraphicsItem *> items = scene->selectedItems();
    QMutableListIterator<QGraphicsItem *> i(items);
    while (i.hasNext()) {
        Link *link = dynamic_cast<Link *>(i.next());
        if (link) {
            delete link;
            i.remove();
        }
    }
    last = 0;
    qDeleteAll(items);

    emit changed();
}

void Diagram::cut()
{
    Node *node = selectedNode();
    if(!node) return;

    copy();
    delete node;
    emit changed();
}

void Diagram::copy()
{
    Node *node = selectedNode();
    if (!node)
        return;

    QString str = QString("Node %1 %2 %3 %4")
                  .arg(node->textColor().name())
                  .arg(node->outlineColor().name())
                  .arg(node->backgroundColor().name())
                  .arg(node->getText());
    QApplication::clipboard()->setText(str);
}

void Diagram::paste()
{
    QString str = QApplication::clipboard()->text();
    QStringList parts = str.split(" ");

    if (parts.count() >= 5 && parts.first() == "Node") {
        Node *node = new Node;
        node->setText(QStringList(parts.mid(4)).join(" "));
        node->setTextColor(QColor(parts[1]));
        node->setOutlineColor(QColor(parts[2]));
        node->setBackgroundColor(QColor(parts[3]));
        setupNode(node);
    }

    emit changed();
}

void Diagram::bringToFront()
{
    setZValue(++maxZ);
}

void Diagram::sendToBack()
{
    setZValue(--minZ);
}

void Diagram::updateActions()
{
    bool hasSelection = !scene->selectedItems().isEmpty();
    bool isNode = (selectedNode() != 0);
    bool isNodePair = (selectedNodePair() != NodePair());

    cutAction->setEnabled(isNode);
    copyAction->setEnabled(isNode);
    addLinkAction->setEnabled(isNodePair);
    deleteAction->setEnabled(hasSelection);
    bringToFrontAction->setEnabled(isNode);
    sendToBackAction->setEnabled(isNode);

    foreach (QAction *action, view->actions())
        view->removeAction(action);

    foreach (QAction *action, editMenu->actions()) {
        if (action->isEnabled())
            view->addAction(action);
    }
}

void Diagram::createActions()
{
    addNodeAction = new QAction(tr("Add &Node"), this);
    addNodeAction->setShortcut(tr("Ctrl+N"));
    connect(addNodeAction, SIGNAL(triggered()), this, SLOT(addNode()));

    addLinkAction = new QAction(tr("Add &Link"), this);
    addLinkAction->setShortcut(tr("Ctrl+L"));
    connect(addLinkAction, SIGNAL(triggered()), this, SLOT(addLink()));

    setColorAction = new QAction(tr("Set Colo&r"), this);
    setColorAction->setShortcut(tr("Ctrl+R"));
    connect(setColorAction, SIGNAL(triggered()), this, SLOT(setColor()));

    deleteAction = new QAction(tr("&Delete"), this);
    deleteAction->setShortcut(tr("Del"));
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(remove()));

    cutAction = new QAction(tr("Cu&t"), this);
    cutAction->setShortcut(tr("Ctrl+X"));
    connect(cutAction, SIGNAL(triggered()), this, SLOT(cut()));

    copyAction = new QAction(tr("&Copy"), this);
    copyAction->setShortcut(tr("Ctrl+C"));
    connect(copyAction, SIGNAL(triggered()), this, SLOT(copy()));

    pasteAction = new QAction(tr("&Paste"), this);
    pasteAction->setShortcut(tr("Ctrl+V"));
    connect(pasteAction, SIGNAL(triggered()), this, SLOT(paste()));

    bringToFrontAction = new QAction(tr("Bring to &Front"), this);
    connect(bringToFrontAction, SIGNAL(triggered()), this, SLOT(bringToFront()));

    sendToBackAction = new QAction(tr("&Send to Back"), this);
    connect(sendToBackAction, SIGNAL(triggered()),
            this, SLOT(sendToBack()));

    addTextAction = new QAction(tr("AddText"),this);
    connect(addTextAction, SIGNAL(triggered()),this, SLOT(addText()));

    zoomInAction = new QAction(tr("Zoom In"),this);
    zoomInAction->setShortcut(tr("Ctrl++"));
    connect(zoomInAction, SIGNAL(triggered()), this, SLOT(zoomIn()));

    zoomOutAction = new QAction(tr("Zoom Out"),this);
    zoomOutAction->setShortcut(tr("Ctrl+-"));
    connect(zoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOut()));
}

void Diagram::createMenus()
{
    editMenu = new QMenu();
    editMenu->addAction(addNodeAction);
    editMenu->addAction(addLinkAction);
    editMenu->addAction(addTextAction);
    editMenu->addAction(deleteAction);
    editMenu->addSeparator();
    editMenu->addAction(setColorAction);
    editMenu->addSeparator();
    editMenu->addAction(cutAction);
    editMenu->addAction(copyAction);
    editMenu->addAction(pasteAction);
    editMenu->addSeparator();
    editMenu->addAction(zoomInAction);
    editMenu->addAction(zoomOutAction);
    editMenu->addAction(bringToFrontAction);
    editMenu->addAction(sendToBackAction);
}

void Diagram::setZValue(int z)
{
    Node *node = selectedNode();
    if (node)
        node->setZValue(z);
}

void Diagram::setupNode(Node *node)
{
    node->setPos(QPoint(80 + (100 * (seqNumber % 5)),
                        80 + (50 * ((seqNumber / 5) % 7))));
    addItem(node);
    ++seqNumber;

    scene->clearSelection();
    node->setSelected(true);
    bringToFront();
}

Node *Diagram::selectedNode() const
{
    QList<QGraphicsItem *> items = scene->selectedItems();
    return (items.count() == 1) ? dynamic_cast<Node *>(items.first()) : 0;
}

Link *Diagram::selectedLink() const
{
    QList<QGraphicsItem *> items = scene->selectedItems();
    return (items.count() == 1) ? dynamic_cast<Link *>(items.first()) : 0;
}

Diagram::NodePair Diagram::selectedNodePair() const
{
    QList<QGraphicsItem *> items = scene->selectedItems();
    if (items.count() == 2) {
        Node *first = dynamic_cast<Node *>(items.first());
        Node *second = dynamic_cast<Node *>(items.last());
        if (first && second)
            return NodePair(first, second);
    }
    return NodePair();
}

void Diagram::mouseDoubleClickEvent(QMouseEvent *e)
{
    QPoint pos = view->mapToScene(e->pos() - view->pos()).toPoint();
    if(!view->itemAt(view->mapFromScene(pos))) addNode(pos);
    emit changed();
}

void Diagram::selectionChanged()
{
    QList<QGraphicsItem *> items = scene->selectedItems();
    if (items.count() == 1){
        last2 = last;
        last = dynamic_cast<Node *>(items.first());
    }
}

void Diagram::zoomIn()
{
    view->scale(100./zoom,100./zoom);
    zoom += 20;
    view->scale(zoom/100.0,zoom/100.0);
}

void Diagram::zoomOut()
{
    view->scale(100./zoom,100./zoom);
    zoom -= 20;
    view->scale(zoom/100.0,zoom/100.0);
}

void Diagram::addText()
{
    QGraphicsTextItem *textItem = new QGraphicsTextItem("Text");
    addItem(textItem);
    if(selectedNode()) textItem->setParent(selectedNode());
    else textItem->setParent(last);

    textItem->setPos(view->mapToScene(view->geometry().center()));
    textItem->setFlags(QGraphicsItem::ItemIsMovable);
    textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
}

void Diagram::linkTo(Node *to)
{
    if(!last2) return;

    to->setSelected(true);
    last2->setSelected(true);
    addLink();

    scene->clearSelection();
    last2->setSelected(true);
}

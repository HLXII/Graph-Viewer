#include "graphwidget.h"
#include "commands.h"

#include <QtDebug>

Q_DECLARE_METATYPE(Node*)

GraphWidget::GraphWidget(QWidget *parent)
    : QGraphicsView(parent), timerId(0)
{
    qRegisterMetaType<Node*>();

    // Setting up QGraphicScene
    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    int minimumSize = 5000;
    scene->setSceneRect(0,0,minimumSize,minimumSize);
    setScene(scene);

    //scene->addLine(QLineF(-minimumSize,-minimumSize,minimumSize,-minimumSize));
    //scene->addLine(QLineF(-minimumSize,minimumSize,minimumSize,minimumSize));
    //connect(scene, &QGraphicsScene::sceneRectChanged, this, &GraphWidget::updateView);
    //connect(scene, SIGNAL(sceneRectChanged(const QRectF &rect)), this, SLOT(updateView(const QRectF &rect)));
    //connect(scene, SIGNAL(changed(const QList<QRectF>)), this, SLOT(updateScene(const QList<QRectF>)));

    // Setting up GraphWidget
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setContextMenuPolicy(Qt::CustomContextMenu);

    // Connecting Custom Context Menu
    connect(this, SIGNAL(customContextMenuRequested(QPoint)),
                    SLOT(showCustomMenu(QPoint)));

    // Setting up Data
    pointerState = PointerState::MOVE;

    // Creating UndoStack
    undoStack = new QUndoStack(this);
    undoView = new QUndoView(undoStack);
    undoView->setWindowTitle(tr("Command List"));
    undoView->show();
    undoView->setAttribute(Qt::WA_QuitOnClose, false);
}

void GraphWidget::updatePointerState() {
    PointerState newPointerState = PointerState::MOVE;
    QString tool = qobject_cast<QAction*>(sender())->text();
    if (tool == "Add Node") {
        newPointerState = PointerState::NODE;
    } else if (tool == "Add Edge") {
        newPointerState = PointerState::EDGE_START;
    }
    updatePointerState(newPointerState);
}

void GraphWidget::updatePointerState(PointerState pointerState) {
    this->pointerState = pointerState;
    qDebug() << "Pointer State: " << this->pointerState << endl;
}

void GraphWidget::addNode()
{
    QPoint pos = qobject_cast<QAction *>(sender())->data().value<QPoint>();
    pos = this->mapToScene(pos).toPoint();
    QUndoCommand *addNodeCommand = new AddNodeCommand(this, pos);
    undoStack->push(addNodeCommand);
}

void GraphWidget::addNode(QPoint pos)
{
    pos = this->mapToScene(pos).toPoint();
    QUndoCommand *addNodeCommand = new AddNodeCommand(this, pos);
    undoStack->push(addNodeCommand);
}

void GraphWidget::deleteNode()
{
    Node* node = static_cast<Node*>(qobject_cast<QAction *>(sender())->data().value<void*>());
    QUndoCommand* deleteNodeCommand = new DeleteNodeCommand(this, node);
    undoStack->push(deleteNodeCommand);
}

//void GraphWidget::addEdge()
//{
//    Node* startNode = static_cast<Node*>(qobject_cast<QAction *>(sender())->data().value<void*>());
//    QUndoCommand * addEdgeCommand = new AddEdgeCommand(this, node);
//    undoStack->push(addEdgeCommand);
//}

void GraphWidget::addEdge(Node* source, Node* dest)
{
    QUndoCommand* addEdgeCommand = new AddEdgeCommand(this, source, dest);
    undoStack->push(addEdgeCommand);
}

void GraphWidget::showCustomMenu(QPoint pos)
{

    QMenu contextMenu(tr("Context Menu"), this);

    qDebug() << "Context Menu" << endl;

    QAction addNodeAction("Add Node", this);
    QAction addEdgeAction("Add Edge", this);
    QAction deleteNodeAction("Delete Node", this);
    QAction viewDataAction("View Data", this);


    if (QGraphicsItem* item = this->itemAt(pos))
    {
        QVariant v;
        v.setValue(QVariant::fromValue(static_cast<void*>(item)));
        addEdgeAction.setData(v);
        deleteNodeAction.setData(v);
        viewDataAction.setData(v);
        connect(&deleteNodeAction, SIGNAL(triggered()), this, SLOT(deleteNode()));
        contextMenu.addAction(&addEdgeAction);
        contextMenu.addAction(&deleteNodeAction);
        contextMenu.addAction(&viewDataAction);
    }
    else
    {
        addNodeAction.setData(QVariant(pos));
        connect(&addNodeAction, SIGNAL(triggered()), this, SLOT(addNode()));
        contextMenu.addAction(&addNodeAction);
    }

    contextMenu.exec(mapToGlobal(pos));
}

void GraphWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Escape:
        updatePointerState(PointerState::MOVE);
        scene()->clearSelection();
        break;
    case Qt::Key_F:
        break;
    case Qt::Key_Z:
        undoStack->undo();
        qDebug() << "Undo" << endl;
        break;
    case Qt::Key_R:
        undoStack->redo();
        qDebug() << "Redo" << endl;
        break;
    default:
        QGraphicsView::keyPressEvent(event);
    }
}

void GraphWidget::mousePressEvent(QMouseEvent *event)
{
    switch(pointerState) {
    case MOVE:
        QGraphicsView::mousePressEvent(event);
        break;
    case NODE:
        break;
    case EDGE_START:
        break;
    case EDGE_END:
        break;
    default:
        break;
    }
}
void GraphWidget::mouseMoveEvent(QMouseEvent* event)
{
    QGraphicsView::mouseMoveEvent(event);
}
void GraphWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsItem* item = this->scene()->itemAt(this->mapToScene(event->pos()),QTransform());
    switch(pointerState) {
    case MOVE:
        QGraphicsView::mouseReleaseEvent(event);
        if (item && item->type() == 65537) {
            qDebug() << "MOVED" << endl;
        }
        break;
    case NODE:
        addNode(event->pos());
        break;
    case EDGE_START:
        if (item && item->type() == 65537) {
            scene()->clearSelection();
            static_cast<Node*>(item)->setSelected(true);
            updatePointerState(PointerState::EDGE_END);
        }
        break;
    case EDGE_END:
        if (item && item->type() == 65537) {
            addEdge(static_cast<Node*>(scene()->selectedItems().first()), static_cast<Node*>(item));
            scene()->clearSelection();
            updatePointerState(PointerState::EDGE_START);
        }
        break;
    default:
        break;
    }
}

void GraphWidget::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
}

void GraphWidget::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);

    QRectF sceneRect = this->sceneRect();

    // Fill
    painter->fillRect(rect.intersected(sceneRect),Qt::GlobalColor::white);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(sceneRect);

    const int gridSize = 25;

    QPointF topLeft = mapToScene(viewport()->rect().topLeft());
    QPointF bottomRight = mapToScene(viewport()->rect().bottomRight());
    qreal left = int(topLeft.x()) - (int(topLeft.x()) % gridSize) - gridSize * 5;
    qreal top = int(topLeft.y()) - (int(topLeft.y()) % gridSize) - gridSize * 5;
    qreal right = int(bottomRight.x()) - (int(bottomRight.x()) % gridSize) + gridSize * 5;
    qreal bottom = int(bottomRight.y()) - (int(bottomRight.y()) % gridSize) + gridSize * 5;

    QVarLengthArray<QPointF, 100> points;

    for (qreal y = top; y < bottom; y += gridSize)
    {
        for (qreal x = left; x <= right; x += gridSize)
        {
            points.append(QPointF(x, y));
        }
    }
    painter->drawPoints(points.data(),points.size());

    update();

}


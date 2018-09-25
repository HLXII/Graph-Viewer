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
    scene->setSceneRect(0,0,1000,1000);

    int minimumSize = 1000;

    //scene->addLine(QLineF(-minimumSize,-minimumSize,minimumSize,-minimumSize));
    //scene->addLine(QLineF(-minimumSize,minimumSize,minimumSize,minimumSize));
    //connect(scene, &QGraphicsScene::sceneRectChanged, this, &GraphWidget::updateView);
    //connect(scene, SIGNAL(sceneRectChanged(const QRectF &rect)), this, SLOT(updateView(const QRectF &rect)));
    //connect(scene, SIGNAL(changed(const QList<QRectF>)), this, SLOT(updateScene(const QList<QRectF>)));
    setScene(scene);

    // Setting up GraphWidget
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    setDragMode(QGraphicsView::ScrollHandDrag);
    //setDragMode(QGraphicsView::RubberBandDrag);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)),
                    SLOT(showCustomMenu(QPoint)));

    undoStack = new QUndoStack(this);

    undoView = new QUndoView(undoStack);
    undoView->setWindowTitle(tr("Command List"));
    undoView->show();
    undoView->setAttribute(Qt::WA_QuitOnClose, false);
}

//void GraphWidget::updateScene(const QList<QRectF> &region)
//{
//    //this->scene()->setSceneRect(this->scene()->itemsBoundingRect());
//}

void GraphWidget::updateView(const QRectF &rect)
{
    Q_UNUSED(rect);

    fitInView(this->scene()->sceneRect(),Qt::KeepAspectRatioByExpanding);
}

void GraphWidget::addNode()
{
    QPoint pos = qobject_cast<QAction *>(sender())->data().value<QPoint>();
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
        //connect(&addEdgeAction, SIGNAL(triggered()), this, SLOT(addNode()));
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
    case Qt::Key_F:
        //scene()->setSceneRect(scene()->itemsBoundingRect());
        //fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
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
    QGraphicsView::mousePressEvent(event);
}
void GraphWidget::mouseMoveEvent(QMouseEvent* event)
{
    QGraphicsView::mouseMoveEvent(event);
}
void GraphWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mouseReleaseEvent(event);
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


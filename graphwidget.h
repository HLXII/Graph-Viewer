#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include "edge.h"
#include "node.h"

#include <QGraphicsView>
#include <QUndoStack>
#include <QUndoView>
#include <QMenu>

enum PointerState {MOVE, NODE, EDGE_START, EDGE_END};

class GraphWidget : public QGraphicsView
{
    Q_OBJECT

public:
    GraphWidget(QWidget *parent = nullptr);

public slots:
    void showCustomMenu(QPoint pos);
    void addNode();
    void addNode(QPoint pos);
    void deleteNode();
    void addEdge(Node*, Node*);

    void updatePointerState();
    void updatePointerState(PointerState);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void timerEvent(QTimerEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;


    void drawBackground(QPainter *painter, const QRectF &rect) override;

private:
    PointerState pointerState;

    int timerId;

    int dragOriginX;
    int dragOriginY;

    QUndoStack *undoStack;
    QUndoView * undoView;
};

#endif // GRAPHWIDGET_H

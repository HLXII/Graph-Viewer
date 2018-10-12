#ifndef COMMANDS_H
#define COMMANDS_H

#include <QUndoCommand>
#include <QPoint>
#include <qstring.h>

#include "graphwidget.h"

class AddNodeCommand : public QUndoCommand
{
public:
    AddNodeCommand(GraphWidget* graphWidget,
                   QPoint initialPosition,
                   QUndoCommand *parent = nullptr);
    ~AddNodeCommand() override;

    void undo() override;
    void redo() override;

private:
    GraphWidget* graphWidget;
    Node* node;
    QPoint initialPosition;
};

class DeleteNodeCommand : public QUndoCommand
{
public:
    DeleteNodeCommand(GraphWidget* graphWidget,
                   Node* node,
                   QUndoCommand *parent = nullptr);
    ~DeleteNodeCommand() override;

    void undo() override;
    void redo() override;

private:
    GraphWidget* graphWidget;
    Node* node;
};

class AddEdgeCommand : public QUndoCommand
{
public:
    AddEdgeCommand(GraphWidget* graphWidget,
                   Node* source,
                   Node* dest,
                   QUndoCommand *parent = nullptr);
    ~AddEdgeCommand() override;

    void undo() override;
    void redo() override;

private:
    GraphWidget* graphWidget;
    Edge* edge;
};


#endif // COMMANDS_H

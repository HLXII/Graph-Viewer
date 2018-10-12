#include "commands.h"

AddNodeCommand::AddNodeCommand(GraphWidget* graphWidget, QPoint initialPosition, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    this->graphWidget = graphWidget;
    this->initialPosition = initialPosition;
    node = new Node(graphWidget);
    node->setPos(initialPosition);
    graphWidget->scene()->addItem(node);
    graphWidget->update();
    //printf("%le %le\n", this->node->getPosition().x(), this->node->getPosition().y());
    QString commandString = QObject::tr("Node at (%1, %2)").arg(initialPosition.x()).arg(initialPosition.y());
    setText(QObject::tr("Add %1").arg(commandString));
}

AddNodeCommand::~AddNodeCommand()
{
}

void AddNodeCommand::undo()
{
    graphWidget->scene()->removeItem(node);
    graphWidget->update();
}

void AddNodeCommand::redo()
{
    graphWidget->scene()->addItem(node);
    node->setPosition(initialPosition);
    graphWidget->scene()->clearSelection();
    graphWidget->update();
}

DeleteNodeCommand::DeleteNodeCommand(GraphWidget* graphWidget, Node* node, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    this->graphWidget = graphWidget;
    this->node = node;
    graphWidget->scene()->removeItem(node);
    graphWidget->update();

    QString commandString = QObject::tr("Deleted Node");
    setText(QObject::tr("Delete %1").arg(commandString));
}

DeleteNodeCommand::~DeleteNodeCommand()
{
}

void DeleteNodeCommand::undo()
{
    graphWidget->scene()->addItem(node);
    graphWidget->update();
}

void DeleteNodeCommand::redo()
{
    graphWidget->scene()->removeItem(node);
    graphWidget->update();
}

AddEdgeCommand::AddEdgeCommand(GraphWidget* graphWidget, Node* source, Node* dest, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    this->graphWidget = graphWidget;
    this->edge = new Edge(source, dest);
    source->addEdge(this->edge);
    dest->addEdge(this->edge);
    graphWidget->scene()->addItem(edge);
    graphWidget->update();
    QString commandString = QObject::tr("Edge from %1 to %2)").arg(source->type()).arg(dest->type());
    setText(QObject::tr("Add %1").arg(commandString));
}

AddEdgeCommand::~AddEdgeCommand()
{
}

void AddEdgeCommand::undo()
{
    graphWidget->scene()->removeItem(edge);
    edge->sourceNode()->removeEdge(edge);
    edge->destNode()->removeEdge(edge);
    graphWidget->update();
}

void AddEdgeCommand::redo()
{
    graphWidget->scene()->addItem(edge);
    edge->sourceNode()->addEdge(edge);
    edge->destNode()->addEdge(edge);
    graphWidget->update();
}

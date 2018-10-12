#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //undoStack = new QUndoStack(this);

    //createUndoView();

    //connect(ui->graphWidget->scene(), &QGraphicsScene::selectionChanged,

    connect(ui->actionAdd_Node, SIGNAL(triggered()), ui->graphWidget, SLOT(updatePointerState()));
    connect(ui->actionAdd_Edge, SIGNAL(triggered()), ui->graphWidget, SLOT(updatePointerState()));
//    ui->mainToolBar->addAction(ui->dockWidget_4->toggleViewAction());
//    ui->menuBar->addAction(ui->dockWidget_4->toggleViewAction());

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createUndoView()
{
    undoView = new QUndoView(undoStack);
    undoView->setWindowTitle(tr("Command List"));
    undoView->show();
    undoView->setAttribute(Qt::WA_QuitOnClose, false);
}


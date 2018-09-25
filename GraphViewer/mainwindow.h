#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"

#include <QMainWindow>

#include <QUndoStack>
#include <QUndoView>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:

private slots:

private:
    void createUndoView();

    Ui::MainWindow *ui;

    QUndoStack *undoStack;
    QUndoView *undoView;

};

#endif // MAINWINDOW_H

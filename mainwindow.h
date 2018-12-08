#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class Form;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void OpenSlot();
    void SaveSlot();

private:
    Ui::MainWindow *ui;
    Form* form { NULL };
};

#endif // MAINWINDOW_H

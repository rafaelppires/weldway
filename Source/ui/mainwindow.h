#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <simple_motion_interface.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
  void on_findZeroPushButton_clicked();
    
private:
    Ui::MainWindow *ui;
    SimpleMotion *smotion;
};

#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <simple_motion_interface.h>
#include <slider_spin.h>
#include <units.h>

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
    
private slots:

private:
  double getSpeedConv( const QString &spd_unit );

  Ui::MainWindow *ui;
  SimpleMotion *smotion;
  SliderSpin *speedSliderSpin, *positionSliderSpin;
  SpeedConv spdconv_;
  PositionConv posconv_;
};

#endif // MAINWINDOW_H

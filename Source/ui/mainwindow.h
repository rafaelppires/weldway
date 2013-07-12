#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <simple_motion_interface.h>
#include <slider_spin.h>
#include <units.h>
#include <stdint.h>

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
  void on_executeButton_clicked();

private:
  double getMotorPosition( uint8_t axis );

  Ui::MainWindow *ui;
  SimpleMotion *xsmotion, *ysmotion, *zsmotion;
  SliderSpin *speedSliderSpin, *xposSliderSpin, *yposSliderSpin, *zposSliderSpin;
  SpeedConv spdconv_;
  PositionConv xposconv_, yposconv_, zposconv_;
};

#endif // MAINWINDOW_H

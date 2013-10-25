#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <simple_motion_interface.h>
#include <slider_spin.h>
#include <units.h>
#include <stdint.h>
#include <protocol.h>

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
  void on_getValuesButton_clicked();
  void openConnectionForm();

private:
  QString stringAxis(uint32_t value);
  Ui::MainWindow *ui;
  QLabel *ok_label_, *nok_label_;
  SimpleMotion *xsmotion, *ysmotion, *zsmotion;
  SliderSpin *speedSliderSpin, *xposSliderSpin, *yposSliderSpin, *zposSliderSpin,
             *fwSpeedSliderSpin, *fwLengthSliderSpin, *bwSpeedSliderSpin, *bwLengthSliderSpin,
             *sbWeldSpeedSliderSpin, *trSpeedSliderSpin, *trAmplSliderSpin, *trFreqSliderSpin,
             *sbtSpeedSliderSpin, *sbtAmplSliderSpin, *sbtLenSliderSpin;
};

#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <simple_motion_interface.h>
#include <slider_spin.h>
#include <units.h>
#include <stdint.h>
#include <protocol.h>
#include <keypress_manager.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
  void keyPressEvent(QKeyEvent *event);

public slots:
  void on_findZeroPushButton_clicked();
    
private slots:
  void on_executeButton_clicked();
  void on_getValuesButton_clicked();
  void openConnectionForm();
  void on_transvTrajectoryComboBox_currentTextChanged(const QString &arg1);
  void on_invertDirectionButton_clicked();
  void on_markFinalPositionButton_clicked();
  void on_markInitPositionButton_clicked();

private:
  QString stringAxis(uint32_t value);
  void setStatus( int32_t status, QLabel *label );
  void checkStatus();
  void setLimits(Vector3I &init, Vector3I &final);
  void progressUpdate( double );

  Ui::MainWindow *ui;
  QLabel *x_statlabel_, *y_statlabel_, *z_statlabel_, *a_statlabel_, *b_statlabel_;
  SimpleMotion *xsmotion, *ysmotion, *zsmotion;
  SliderSpin *speedSliderSpin, *xposSliderSpin, *yposSliderSpin, *zposSliderSpin,
             *fwSpeedSliderSpin, *fwLengthSliderSpin, *bwSpeedSliderSpin, *bwLengthSliderSpin,
             *sbWeldSpeedSliderSpin, *trSpeedSliderSpin, *trAmplSliderSpin, *trFreqSliderSpin,
             *sbtSpeedSliderSpin, *sbtAmplSliderSpin, *sbtLenSliderSpin;
  KeyPressManager keypress_manager_;
  MasterCommunicator &machine_;
};

#endif // MAINWINDOW_H

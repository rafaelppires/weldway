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
#include <QtWidgets/QGraphicsScene>

namespace Ui {
class MainWindow;
}

class TrajectoryScene : public QGraphicsScene {
  void drawBackground ( QPainter * painter, const QRectF & rect );
};

class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
  void keyPressEvent(QKeyEvent *event);

public slots:
  void on_findZeroPushButton_clicked();
  void emergencyNotify( bool );
  void executionFinished();
    
private slots:
  void on_executeButton_clicked();
  void on_getValuesButton_clicked();
  void openConnectionForm();
  void on_transvTrajectoryComboBox_currentTextChanged(const QString &arg1);
  void on_invertDirectionButton_clicked();
  void on_markFinalPositionButton_clicked();
  void on_markInitPositionButton_clicked();
  void on_longTrajectoryComboBox_currentTextChanged(const QString &arg1);
  void on_correctButton_clicked();
  void on_frequency_changed();
  void on_tabWidget_currentChanged(int index);
  void redraw();

  void on_initPosButton_clicked();

  void on_finalPosButton_clicked();

private:
  TrajectoryScene *scene_;
  QString stringAxis(uint32_t value);
  void setStatus( int32_t status, QLabel *label );
  void checkStatus();
  void setLimits(Vector3I &init, Vector3I &final);
  void progressUpdate( double );
  void emergencyUpdate( bool );
  void render(PositionVector &v);

  Ui::MainWindow *ui;
  QLabel *x_statlabel_, *y_statlabel_, *z_statlabel_, *a_statlabel_, *b_statlabel_;
  SimpleMotion *xsmotion, *ysmotion, *zsmotion;
  SliderSpin *speedSliderSpin, *xposSliderSpin, *yposSliderSpin, *zposSliderSpin,
             *fwSpeedSliderSpin, *fwLengthSliderSpin, *bwSpeedSliderSpin, *bwLengthSliderSpin,
             *sbWeldSpeedSliderSpin, *trSpeedSliderSpin, *trAmplSliderSpin, *trLmbdSliderSpin,
             *sbtSpeedSliderSpin, *sbtAmplSliderSpin, *sbtLenSliderSpin, *eRhoSliderSpin,
             *vOrSliderSpin, *hOrSliderSpin;
  KeyPressManager keypress_manager_;
  MasterCommunicator &machine_;
  boost::shared_ptr<AbstractTrajectory> executing_trajectory_;
};

#endif // MAINWINDOW_H

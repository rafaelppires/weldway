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
#include <longitudinalwidget.h>
#include <transversalwidget.h>
#include <triangularswitchback.h>
#include <customtrajconfig.h>
#include <QtWidgets/QToolBox>
#include <trajectoryscene.h>
#include <trajrectparamswidget.h>
#include <circularwidget.h>

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
  void emergencyNotify( bool );
  void executionFinished();
    
private slots:
  void on_executeButton_clicked();
  void on_getValuesButton_clicked();
  void openConnectionForm();
  void on_correctButton_clicked();
  void on_tabWidget_currentChanged(int index);
  void redraw();
  void actuallyRedraw();

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
  void oscillationsSetup();
  OscillationWidget* activeWidget();

  QToolBox *oscillationsToolBox, *trajSetupToolBox;
  TransversalWidget *transv_panel_;
  LongitudinalWidget *longit_panel_;
  CustomTrajectoryWidget *custom_panel_;
  TrajRectParamsWidget *trajparams_panel_;
  CircularWidget *trajcircle_panel_;

  Ui::MainWindow *ui;
  QLabel *x_statlabel_, *y_statlabel_, *z_statlabel_, *a_statlabel_, *b_statlabel_;
  SimpleMotion *xsmotion, *ysmotion, *zsmotion;
  SliderSpin *speedSliderSpin, *xposSliderSpin, *yposSliderSpin, *zposSliderSpin,
             *vOrSliderSpin, *hOrSliderSpin;
  KeyPressManager keypress_manager_;
  MasterCommunicator &machine_;
  boost::shared_ptr<AbstractTrajectory> executing_trajectory_;
  boost::shared_ptr<QTimer> redraw_timer_;

};

#endif // MAINWINDOW_H

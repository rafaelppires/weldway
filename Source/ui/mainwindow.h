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
    
private slots:
  void on_speedUnitComboBox_currentTextChanged(const QString &arg1);
  void changeSpinBox( int slider_value );
  void changeSlider( double spin_value );

private:
  double fromCurrentSpeed( double cur );
  double toCurrentSpeed( double mm );
  double getSpeedConv( const QString &spd_unit );

  Ui::MainWindow *ui;
  SimpleMotion *smotion;
  QString cur_spd_unit_;
};

#endif // MAINWINDOW_H

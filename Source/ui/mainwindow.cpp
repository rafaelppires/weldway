#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QLineEdit>

//-----------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow), spdconv_(1,100), posconv_(0,875) {
  ui->setupUi(this);
  smotion = new SimpleMotion( "TTL232R" );

  speedSliderSpin    = new SliderSpin( this, ui->speedSlider, ui->speedSpinBox, ui->speedUnitComboBox, spdconv_ );
  positionSliderSpin = new SliderSpin( this, ui->positionSlider, ui->positionSpinBox, ui->posUnitComboBox, posconv_ );
}

//-----------------------------------------------------------------------------
MainWindow::~MainWindow() {
  delete ui;
}

//-----------------------------------------------------------------------------
void MainWindow::on_findZeroPushButton_clicked() {
  //ui->findZeroPushButton->setText( "Whazup!" );
  smotion->startHoming();
}

//-----------------------------------------------------------------------------
double MainWindow::getSpeedConv( const QString &spd_unit ) {
  double ret = 1;
  if( spd_unit == "m/min" )
    ret = TO_M_MIN;
  else if( spd_unit == "rpm" )
    ret = TO_RPM;
  else if( spd_unit == "rps" )
    ret = TO_RPS;
  return ret;
}


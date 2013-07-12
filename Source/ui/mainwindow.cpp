#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QLineEdit>

//-----------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow),
    spdconv_(1,100), xposconv_(0,875), yposconv_(0,175), zposconv_(0,100)  {
  ui->setupUi(this);
  xsmotion = new SimpleMotion( "X_AXIS" );
  ysmotion = new SimpleMotion( "Y_AXIS" );
  zsmotion = new SimpleMotion( "Z_AXIS" );

  speedSliderSpin    = new SliderSpin( this, ui->speedSlider, ui->speedSpinBox, ui->speedUnitComboBox, spdconv_ );
  xposSliderSpin = new SliderSpin( this, ui->xpositionSlider, ui->xpositionSpinBox, ui->xposUnitComboBox, xposconv_ );
  yposSliderSpin = new SliderSpin( this, ui->ypositionSlider, ui->ypositionSpinBox, ui->yposUnitComboBox, yposconv_ );
  zposSliderSpin = new SliderSpin( this, ui->zpositionSlider, ui->zpositionSpinBox, ui->zposUnitComboBox, zposconv_ );
}

//-----------------------------------------------------------------------------
MainWindow::~MainWindow() {
  delete ui;
}

//-----------------------------------------------------------------------------
void MainWindow::on_findZeroPushButton_clicked() {
  //ui->findZeroPushButton->setText( "Whazup!" );
  xsmotion->startHoming();
  ysmotion->startHoming();
  zsmotion->startHoming();
}

#define X_AXIS 0
#define Y_AXIS 1
#define Z_AXIS 2

//-----------------------------------------------------------------------------
double MainWindow::getMotorPosition( uint8_t axis ) {
  PositionConv &conv = axis == X_AXIS ? xposconv_ : (axis == Y_AXIS ? yposconv_ : zposconv_ );
  QComboBox *unit = axis == X_AXIS ? ui->xposUnitComboBox : ( axis == Y_AXIS ? ui->yposUnitComboBox : ui->zposUnitComboBox);
  QDoubleSpinBox *value = axis == X_AXIS ? ui->xpositionSpinBox : ( axis == Y_AXIS ? ui->ypositionSpinBox : ui->zpositionSpinBox);
  double conv_cur = conv.getConv( unit->currentText().toStdString() ),
         conv_pul = conv.getConv( "pulsos" );
  return value->value() * conv_pul / conv_cur;
}

//-----------------------------------------------------------------------------
void MainWindow::on_executeButton_clicked() {
  double conv_cur = spdconv_.getConv( ui->speedUnitComboBox->currentText().toStdString() ),
         conv_rpm = spdconv_.getConv("rpm"),
         speed_rpm = ui->speedSpinBox->value() * conv_rpm / conv_cur,
         xpos =  getMotorPosition( X_AXIS ),
         ypos = -getMotorPosition( Y_AXIS ),
         zpos =  getMotorPosition( Z_AXIS );

  std::cout << "Speed: " << speed_rpm << " xpos: " << xpos << " ypos: " << ypos << " zpos: " << zpos << "\n";

  xsmotion->setMaxSpeed( speed_rpm );
  ysmotion->setMaxSpeed( speed_rpm );
  zsmotion->setMaxSpeed( speed_rpm );

  xsmotion->gotoAbsPosition( xpos );
  ysmotion->gotoAbsPosition( ypos );
  zsmotion->gotoAbsPosition( zpos );

}

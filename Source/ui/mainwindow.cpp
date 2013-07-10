#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

#define RT  100
#define PPR 400
#define SPEED_RANGE_FROM   1 // mm/s
#define SPEED_RANGE_TO   100 // mm/s
#define SPEED_RANGE      (SPEED_RANGE_TO-SPEED_RANGE_FROM)

// Default distance unit is milimeters
// Default time unit is seconds
#define STEP_LEN (1000./(RT*PPR))
#define TO_M     (1./1000)
#define TO_IN    (1./25.4)
#define TO_M_MIN (60./1000)


//-----------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow) {
  ui->setupUi(this);
  smotion = new SimpleMotion( "TTL232R" );
  cur_spd_unit_ = ui->speedUnitComboBox->currentText();
  connect( ui->speedSlider,  SIGNAL(valueChanged(int)),    SLOT(changeSpinBox(int)) );
  connect( ui->speedSpinBox, SIGNAL(valueChanged(double)), SLOT(changeSlider(double)) );
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
double MainWindow::toCurrentSpeed( double mm_speed ) {
  return mm_speed * getSpeedConv( ui->speedUnitComboBox->currentText() );
}

//-----------------------------------------------------------------------------
double MainWindow::fromCurrentSpeed( double cur_unit_speed ) {
  return cur_unit_speed / getSpeedConv( ui->speedUnitComboBox->currentText() );
}

//-----------------------------------------------------------------------------
void MainWindow::changeSpinBox( int slider_value ) {
  int min = ui->speedSlider->minimum(), max = ui->speedSlider->maximum();
  double ratio = double(slider_value - min) / (max-min),
         value = toCurrentSpeed( SPEED_RANGE_FROM + ratio * SPEED_RANGE );
  ui->speedSpinBox->setValue( value );
}

//-----------------------------------------------------------------------------
void MainWindow::changeSlider( double spin_value ) {
  spin_value = fromCurrentSpeed(spin_value);
  int min = ui->speedSlider->minimum(), max = ui->speedSlider->maximum();
  ui->speedSlider->setValue( min + (max-min) * (spin_value - SPEED_RANGE_FROM) / (SPEED_RANGE) );
}

//-----------------------------------------------------------------------------
double MainWindow::getSpeedConv( const QString &spd_unit ) {
  double ret = 1;
  if( spd_unit == "m/min" )
    ret = TO_M_MIN;
  return ret;
}

//-----------------------------------------------------------------------------
void MainWindow::on_speedUnitComboBox_currentTextChanged(const QString &arg1) {
  double conv_pre = getSpeedConv( cur_spd_unit_ ),
         conv_cur = getSpeedConv( arg1 ),
         conv = conv_cur / conv_pre,
         value_pre = ui->speedSpinBox->value();
  ui->speedSpinBox->setMaximum( SPEED_RANGE_TO * conv_cur );
  ui->speedSpinBox->setMinimum( SPEED_RANGE_FROM * conv_cur );
  ui->speedSpinBox->setValue( value_pre * conv );
  cur_spd_unit_ = arg1;
}


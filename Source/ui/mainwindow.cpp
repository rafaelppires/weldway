#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QLineEdit>
#include <formconnection.h>
#include <switch_back.h>

//-----------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow),
    spdconv_(1,100), xposconv_(0,875), yposconv_(0,175), zposconv_(0,100)  {
  ui->setupUi(this);
  xsmotion = new SimpleMotion( "X_AXIS" );
  ysmotion = new SimpleMotion( "Y_AXIS" );
  zsmotion = new SimpleMotion( "Z_AXIS" );

  connect( ui->actionConnection, SIGNAL(triggered()), SLOT(openConnectionForm()));

  speedSliderSpin = new SliderSpin( this, ui->speedSlider, ui->speedSpinBox, ui->speedUnitComboBox, spdconv_ );
  xposSliderSpin  = new SliderSpin( this, ui->xpositionSlider, ui->xpositionSpinBox, ui->xposUnitComboBox, xposconv_ );
  yposSliderSpin  = new SliderSpin( this, ui->ypositionSlider, ui->ypositionSpinBox, ui->yposUnitComboBox, yposconv_ );
  zposSliderSpin  = new SliderSpin( this, ui->zpositionSlider, ui->zpositionSpinBox, ui->zposUnitComboBox, zposconv_ );

  fwSpeedSliderSpin  = new SliderSpin( this, ui->fwSpeedSlider, ui->fwSpeedSpinBox, ui->fwSpeedUnitComboBox, spdconv_ );
  fwLengthSliderSpin = new SliderSpin( this, ui->fwLengthSlider, ui->fwLengthSpinBox, ui->fwLengthUnitComboBox, xposconv_ );
  bwSpeedSliderSpin  = new SliderSpin( this, ui->bwSpeedSlider, ui->bwSpeedSpinBox, ui->bwSpeedUnitComboBox, spdconv_ );
  bwLengthSliderSpin = new SliderSpin( this, ui->bwLengthSlider, ui->bwLengthSpinBox, ui->bwLengthUnitComboBox, xposconv_ );

  //ok_label_  = new QLabel( ui->statusbar );
  nok_label_ = new QLabel( statusBar() );
  nok_label_->setObjectName(QStringLiteral("nok_label_"));
  nok_label_->setMaximumSize( QSize(20,20) );
  nok_label_->setPixmap(QPixmap(QString::fromUtf8(":/imgs/imgs/red.png")));
  statusBar()->addWidget( nok_label_ );
  nok_label_->setScaledContents(true);
}

//-----------------------------------------------------------------------------
MainWindow::~MainWindow() {
  delete ui;

  delete xsmotion;
  delete ysmotion;
  delete zsmotion;

  delete speedSliderSpin;
  delete xposSliderSpin;
  delete yposSliderSpin;
  delete zposSliderSpin;

  delete fwSpeedSliderSpin;
  delete fwLengthSliderSpin;
  delete bwSpeedSliderSpin;
  delete bwLengthSliderSpin;
}

//-----------------------------------------------------------------------------
void MainWindow::on_findZeroPushButton_clicked() {
  //xsmotion->startHoming();
  //ysmotion->startHoming();
  //zsmotion->startHoming();

  MasterCommunicator::getInstance().startHoming( AXIS_ALL );
}

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
  //std::cout << "Speed: " << speed_rpm << " xpos: " << xpos << " ypos: " << ypos << " zpos: " << zpos << "\n";
  MasterCommunicator &mc = MasterCommunicator::getInstance();
  int idx = ui->tabWidget->currentIndex();
  if( idx == 1 ) {
    double conv_cur = spdconv_.getConv( ui->speedUnitComboBox->currentText().toStdString() ),
           conv_rpm = spdconv_.getConv("rpm"),
           speed_rpm = ui->speedSpinBox->value() * conv_rpm / conv_cur,
           xpos =  getMotorPosition( X_AXIS ),
           ypos = -getMotorPosition( Y_AXIS ),
           zpos =  getMotorPosition( Z_AXIS );
    mc.setMaxSpeed( speed_rpm, AXIS_ALL );

    AbstractProtocol::ConcurrentCmmd cmd;
    cmd[ X_AXIS ] = xpos;
    cmd[ Y_AXIS ] = ypos;
    cmd[ Z_AXIS ] = zpos;

    mc.sendPosCmmds( cmd );
  } else if( idx == 2 ) {
    int32_t fwlen = 0,
            fwspd = 0,
            bwlen = 0,
            bwspd = 0;
    SwitchBackTrajectory sb( fwlen, fwspd, bwlen, bwspd );
    mc.executeTrajectory( sb );
  }
}
//-----------------------------------------------------------------------------
void MainWindow::openConnectionForm() {
  FormConnection form;
  form.exec();
}
//-----------------------------------------------------------------------------

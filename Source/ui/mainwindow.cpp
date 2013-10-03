#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QLineEdit>
#include <formconnection.h>
#include <switch_back.h>

//-----------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow),
    spdconv_(1,100), xposconv_(0,875), yposconv_(0,175), zposconv_(0,100),
    fwlenconv_(4,20), bwlenconv_(2,10), sbweldspdconv_(1,50), fwspdconv_(4,200), bwspdconv_(2,100) {
  ui->setupUi(this);
  xsmotion = new SimpleMotion( "X_AXIS" );
  ysmotion = new SimpleMotion( "Y_AXIS" );
  zsmotion = new SimpleMotion( "Z_AXIS" );

  connect( ui->actionConnection, SIGNAL(triggered()), SLOT(openConnectionForm()));

  // Fixed Position
  speedSliderSpin = new SliderSpin( this, ui->speedSlider, ui->speedSpinBox, ui->speedUnitComboBox, spdconv_ );
  xposSliderSpin  = new SliderSpin( this, ui->xpositionSlider, ui->xpositionSpinBox, ui->xposUnitComboBox, xposconv_ );
  yposSliderSpin  = new SliderSpin( this, ui->ypositionSlider, ui->ypositionSpinBox, ui->yposUnitComboBox, yposconv_ );
  zposSliderSpin  = new SliderSpin( this, ui->zpositionSlider, ui->zpositionSpinBox, ui->zposUnitComboBox, zposconv_ );

  // Switch Back
  sbWeldSpeedSliderSpin = new SliderSpin( this, ui->sbWeldSpeedSlider, ui->sbWeldSpeedSpinBox, ui->sbWeldSpeedUnitComboBox, sbweldspdconv_);
  fwSpeedSliderSpin  = new SliderSpin( this, ui->fwSpeedSlider, ui->fwSpeedSpinBox, ui->fwSpeedUnitComboBox, fwspdconv_ );
  fwLengthSliderSpin = new SliderSpin( this, ui->fwLengthSlider, ui->fwLengthSpinBox, ui->fwLengthUnitComboBox, fwlenconv_ );
  bwSpeedSliderSpin  = new SliderSpin( this, ui->bwSpeedSlider, ui->bwSpeedSpinBox, ui->bwSpeedUnitComboBox, bwspdconv_ );
  bwLengthSliderSpin = new SliderSpin( this, ui->bwLengthSlider, ui->bwLengthSpinBox, ui->bwLengthUnitComboBox, bwlenconv_ );

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

  delete sbWeldSpeedSliderSpin;
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
void MainWindow::on_executeButton_clicked() {
  std::string pos_unit("pulsos"), spd_unit("rpm");
  MasterCommunicator &mc = MasterCommunicator::getInstance();
  int idx = ui->tabWidget->currentIndex();
  if( idx == 1 ) {
    double speed_rpm = speedSliderSpin->value( spd_unit ),
           xpos =  xposSliderSpin->value( pos_unit ),
           ypos = -yposSliderSpin->value( pos_unit ),
           zpos =  zposSliderSpin->value( pos_unit );
    mc.setMaxSpeed( speed_rpm, AXIS_ALL );

    AbstractProtocol::ConcurrentCmmd cmd;
    cmd[ X_AXIS ] = xpos;
    cmd[ Y_AXIS ] = ypos;
    cmd[ Z_AXIS ] = zpos;

    mc.sendPosCmmds( cmd );
  } else if( idx == 2 ) {
    int32_t fwlen = fwLengthSliderSpin->value( pos_unit ),
            fwspd = fwSpeedSliderSpin->value( spd_unit ),
            bwlen = bwLengthSliderSpin->value( pos_unit ),
            bwspd = bwSpeedSliderSpin->value( spd_unit );
    boost::shared_ptr<AbstractTrajectory> sb( new SwitchBackTrajectory(fwlen, fwspd, bwlen, bwspd) );
    mc.executeTrajectory( sb );
  }
}
//-----------------------------------------------------------------------------
void MainWindow::openConnectionForm() {
  FormConnection form;
  form.exec();
}
//-----------------------------------------------------------------------------

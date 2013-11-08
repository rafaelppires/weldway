#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QLineEdit>
#include <formconnection.h>
#include <switch_back.h>
#include <triangular.h>
#include <double_e.h>
#include <e_trajectory.h>
#include <rhombus.h>

//-----------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  xsmotion = new SimpleMotion( "X_AXIS" );
  ysmotion = new SimpleMotion( "Y_AXIS" );
  zsmotion = new SimpleMotion( "Z_AXIS" );

  connect( ui->actionConnection, SIGNAL(triggered()), SLOT(openConnectionForm()));

  // Fixed Position
  speedSliderSpin = new SliderSpin( this, ui->speedSlider, ui->speedSpinBox, ui->speedUnitComboBox, UnitConvPtr(new SpeedConv(1, 100)) );
  xposSliderSpin  = new SliderSpin( this, ui->xpositionSlider, ui->xpositionSpinBox, ui->xposUnitComboBox, UnitConvPtr(new PositionConv(0, 875)) );
  yposSliderSpin  = new SliderSpin( this, ui->ypositionSlider, ui->ypositionSpinBox, ui->yposUnitComboBox, UnitConvPtr(new PositionConv(0, 175)) );
  zposSliderSpin  = new SliderSpin( this, ui->zpositionSlider, ui->zpositionSpinBox, ui->zposUnitComboBox, UnitConvPtr(new PositionConv(0, 100)) );

  // Switch Back
  sbWeldSpeedSliderSpin = new SliderSpin( this, ui->sbWeldSpeedSlider, ui->sbWeldSpeedSpinBox, ui->sbWeldSpeedUnitComboBox, UnitConvPtr(new SpeedConv(1, 50)) );
  fwSpeedSliderSpin  = new SliderSpin( this, ui->fwSpeedSlider, ui->fwSpeedSpinBox, ui->fwSpeedUnitComboBox, UnitConvPtr(new SpeedConv(4, 200)) );
  fwLengthSliderSpin = new SliderSpin( this, ui->fwLengthSlider, ui->fwLengthSpinBox, ui->fwLengthUnitComboBox, UnitConvPtr(new PositionConv(4, 20)) );
  bwSpeedSliderSpin  = new SliderSpin( this, ui->bwSpeedSlider, ui->bwSpeedSpinBox, ui->bwSpeedUnitComboBox, UnitConvPtr(new SpeedConv(2, 100)) );
  bwLengthSliderSpin = new SliderSpin( this, ui->bwLengthSlider, ui->bwLengthSpinBox, ui->bwLengthUnitComboBox, UnitConvPtr(new PositionConv(2, 10)) );

  sbWeldSpeedSliderSpin->addMultiplier( fwSpeedSliderSpin, 4.0 );
  sbWeldSpeedSliderSpin->addMultiplier( bwSpeedSliderSpin, 2.0 );

  fwSpeedSliderSpin->addMultiplier( bwSpeedSliderSpin, 0.5 );
  fwLengthSliderSpin->addMultiplier( bwLengthSliderSpin, 0.5 );


  // Triangular
  trSpeedSliderSpin = new SliderSpin( this, ui->trWeldSpeedSlider, ui->trWeldSpeedSpinBox, ui->trWeldSpeedUnitComboBox, UnitConvPtr(new SpeedConv(0, 100)) );
  trAmplSliderSpin  = new SliderSpin( this, ui->trAmplitudeSlider, ui->trAmplitudeSpinBox, ui->trAmplitudeUnitComboBox, UnitConvPtr(new PositionConv(0, 50)) );
  trFreqSliderSpin  = new SliderSpin( this, ui->trFrequencySlider, ui->trFrequencySpinBox, NULL, UnitConvPtr(new UnitConv(0.1, 10)) );

  // SB + Triang
  sbtSpeedSliderSpin = new SliderSpin( this, ui->sbtWeldSpeedSlider, ui->sbtWeldSpeedSpinBox, ui->sbtWeldSpeedUnitComboBox, UnitConvPtr(new SpeedConv(0,100)) );
  sbtAmplSliderSpin  = new SliderSpin( this, ui->sbtAmplitudeSlider, ui->sbtAmplitudeSpinBox, ui->sbtAmplitudeUnitComboBox, UnitConvPtr(new PositionConv(0,50)) );
  sbtLenSliderSpin   = new SliderSpin( this, ui->sbtForwardLenSlider, ui->sbtForwardLenSpinBox, ui->sbtForwardLenUnitComboBox, UnitConvPtr(new PositionConv(0,30)) );

  //ok_label_  = new QLabel( ui->statusbar );
  x_statlabel_ = new QLabel( statusBar() );
  x_statlabel_->setObjectName(QStringLiteral("nok_label_"));
  x_statlabel_->setMaximumSize( QSize(20,20) );
  x_statlabel_->setPixmap(QPixmap(QString::fromUtf8(":/imgs/imgs/red.png")));
  statusBar()->addWidget( x_statlabel_ );
  x_statlabel_->setScaledContents(true);

  y_statlabel_ = new QLabel( statusBar() );
  y_statlabel_->setObjectName(QStringLiteral("nok_label_"));
  y_statlabel_->setMaximumSize( QSize(20,20) );
  y_statlabel_->setPixmap(QPixmap(QString::fromUtf8(":/imgs/imgs/red.png")));
  statusBar()->addWidget( y_statlabel_ );
  y_statlabel_->setScaledContents(true);

  z_statlabel_ = new QLabel( statusBar() );
  z_statlabel_->setObjectName(QStringLiteral("nok_label_"));
  z_statlabel_->setMaximumSize( QSize(20,20) );
  z_statlabel_->setPixmap(QPixmap(QString::fromUtf8(":/imgs/imgs/red.png")));
  statusBar()->addWidget( z_statlabel_ );
  z_statlabel_->setScaledContents(true);

  a_statlabel_ = new QLabel( statusBar() );
  a_statlabel_->setObjectName(QStringLiteral("nok_label_"));
  a_statlabel_->setMaximumSize( QSize(20,20) );
  a_statlabel_->setPixmap(QPixmap(QString::fromUtf8(":/imgs/imgs/red.png")));
  statusBar()->addWidget( a_statlabel_ );
  a_statlabel_->setScaledContents(true);

  b_statlabel_ = new QLabel( statusBar() );
  b_statlabel_->setObjectName(QStringLiteral("nok_label_"));
  b_statlabel_->setMaximumSize( QSize(20,20) );
  b_statlabel_->setPixmap(QPixmap(QString::fromUtf8(":/imgs/imgs/red.png")));
  statusBar()->addWidget( b_statlabel_ );
  b_statlabel_->setScaledContents(true);
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

  delete trSpeedSliderSpin;
  delete trAmplSliderSpin;
  delete trFreqSliderSpin;

  delete sbtSpeedSliderSpin;
  delete sbtAmplSliderSpin;
  delete sbtLenSliderSpin;
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

  if( mc.busy() ) {
    mc.cancel();
    printf(">>> Cancelled <<<\n");
  } else if( idx == 2 ) {
    mc.setMaxSpeed( speedSliderSpin->value( spd_unit ), AXIS_ALL );

    AbstractProtocol::ConcurrentCmmd32 cmd;
    cmd[ X_AXIS ] =  xposSliderSpin->value( pos_unit );
    cmd[ Y_AXIS ] = -yposSliderSpin->value( pos_unit );
    cmd[ Z_AXIS ] =  zposSliderSpin->value( pos_unit );

    mc.sendPosCmmds( cmd );
  } else if( idx == 3 ) { // Switch back
    int32_t fwlen = fwLengthSliderSpin->value( pos_unit ),
            weldspd = sbWeldSpeedSliderSpin->value( spd_unit );
    AbsTrajectoryPtr sb( new SwitchBackTrajectory(fwlen, weldspd) );
    mc.executeTrajectory( sb );
  } else if( idx == 4 ) { // Triangular
    boost::shared_ptr<AbstractTrajectory> tr;
    double freq  = trFreqSliderSpin->value();
    int32_t spd  = trSpeedSliderSpin->value( spd_unit ),
            ampl = trAmplSliderSpin->value( pos_unit ),
            tidx = ui->transvTrajectoryComboBox->currentIndex();
    if( tidx == 1 ) {
      tr.reset( new ETrajectory( spd, freq, ampl ) );
    } else if( tidx == 2 ) {
      tr.reset( new DoubleETrajectory( spd, freq, ampl ) );
    } else {
      tr.reset( new TriangularTrajectory( spd, freq, ampl ) );
    }
    mc.executeTrajectory( tr );
  } else if( idx == 5 ) {
    int32_t spd  = sbtSpeedSliderSpin->value( spd_unit ),
            ampl = sbtAmplSliderSpin->value( pos_unit ),
            len  = sbtLenSliderSpin->value( pos_unit );
    mc.executeTrajectory( AbsTrajectoryPtr(new Rhombus( ampl, len, ui->sbtOscCountSpinBox->value(), spd )));
  }
  fflush( stdout );
}
//-----------------------------------------------------------------------------
void MainWindow::openConnectionForm() {
  FormConnection form;
  form.exec();
  checkStatus();
}

//-----------------------------------------------------------------------------
QString MainWindow::stringAxis( uint32_t value ) {
  if( value == uint16_t(~0) )
    return "sem resposta";
  else
    return QString::number( value );
}

//-----------------------------------------------------------------------------

#include <granite_spi_interface.h>
#include <granite/vsd_cmd.h>
//-----------------------------------------------------------------------------
void MainWindow::on_getValuesButton_clicked() {
  MasterCommunicator &mc = MasterCommunicator::getInstance();
  ui->xvalueLabel->setText( faultString( mc.getStatus( FaultBits, X_AXIS ) ).c_str() );
  ui->yvalueLabel->setText( faultString( mc.getStatus( FaultBits, Y_AXIS ) ).c_str() );
  ui->zvalueLabel->setText( faultString( mc.getStatus( FaultBits, Z_AXIS ) ).c_str() );
  ui->avalueLabel->setText( faultString( mc.getStatus( FaultBits, A_AXIS ) ).c_str() );
  ui->bvalueLabel->setText( faultString( mc.getStatus( FaultBits, B_AXIS ) ).c_str() );

  //ui->anotherLabel->setText( QString(mc.getStatus( RawPosition, X_AXIS )) );
  checkStatus();
}

//-----------------------------------------------------------------------------
void MainWindow::setStatus( int32_t status, QLabel *label ) {
  if( status == int32_t(~0) )
    label->setPixmap(QPixmap(QString::fromUtf8(":/imgs/imgs/red.png")));
  else if( status == 0 )
    label->setPixmap(QPixmap(QString::fromUtf8(":/imgs/imgs/green.png")));
  else {
    label->setPixmap(QPixmap(QString::fromUtf8(":/imgs/imgs/yellow.png")));
    label->setStatusTip( faultString(status).c_str() );
  }
}

//-----------------------------------------------------------------------------
void MainWindow::checkStatus() {
  MasterCommunicator &mc = MasterCommunicator::getInstance();
  setStatus( mc.getStatus( FaultBits, X_AXIS ), x_statlabel_ );
  setStatus( mc.getStatus( FaultBits, Y_AXIS ), y_statlabel_ );
  setStatus( mc.getStatus( FaultBits, Z_AXIS ), z_statlabel_ );
  setStatus( mc.getStatus( FaultBits, A_AXIS ), a_statlabel_ );
  setStatus( mc.getStatus( FaultBits, B_AXIS ), b_statlabel_ );
}

//-----------------------------------------------------------------------------

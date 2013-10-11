#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QLineEdit>
#include <formconnection.h>
#include <switch_back.h>
#include <triangular.h>

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
  if( idx == 2 ) {
    mc.setMaxSpeed( speedSliderSpin->value( spd_unit ), AXIS_ALL );

    AbstractProtocol::ConcurrentCmmd32 cmd;
    cmd[ X_AXIS ] =  xposSliderSpin->value( pos_unit );
    cmd[ Y_AXIS ] = -yposSliderSpin->value( pos_unit );
    cmd[ Z_AXIS ] =  zposSliderSpin->value( pos_unit );

    mc.sendPosCmmds( cmd );
  } else if( idx == 3 ) { // Switch back
    int32_t fwlen = fwLengthSliderSpin->value( pos_unit ),
            weldspd = sbWeldSpeedSliderSpin->value( spd_unit );
    boost::shared_ptr<AbstractTrajectory> sb( new SwitchBackTrajectory(fwlen, weldspd) );
    mc.executeTrajectory( sb );
  } else if( idx == 4 ) { // Triangular
    int32_t spd  = trSpeedSliderSpin->value( spd_unit ),
            ampl = trAmplSliderSpin->value( pos_unit );
    boost::shared_ptr<AbstractTrajectory> tr( new TriangularTrajectory( spd, trFreqSliderSpin->value(), ampl ) );
    mc.executeTrajectory( tr );
  }
  fflush( stdout );
}
//-----------------------------------------------------------------------------
void MainWindow::openConnectionForm() {
  FormConnection form;
  form.exec();
}

//-----------------------------------------------------------------------------
QString MainWindow::stringAxis( uint32_t value ) {
  if( value == uint16_t(~0) )
    return "sem resposta";
  else
    return QString::number( value );
}

#include <granite_spi_interface.h>
//-----------------------------------------------------------------------------
void MainWindow::on_getValuesButton_clicked() {
  MasterCommunicator &mc = MasterCommunicator::getInstance();
  ui->xvalueLabel->setText( stringAxis(mc.getStatus( VelocityLimit, X_AXIS )) );
  ui->yvalueLabel->setText( stringAxis(mc.getStatus( VelocityLimit, Y_AXIS )) );
  ui->zvalueLabel->setText( stringAxis(mc.getStatus( VelocityLimit, Z_AXIS )) );
  ui->avalueLabel->setText( stringAxis(mc.getStatus( VelocityLimit, A_AXIS )) );
  ui->bvalueLabel->setText( stringAxis(mc.getStatus( VelocityLimit, B_AXIS )) );
}

//-----------------------------------------------------------------------------

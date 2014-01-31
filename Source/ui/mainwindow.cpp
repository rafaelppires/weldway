#include <QtWidgets>
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
    QMainWindow(parent), ui(new Ui::MainWindow),
    keypress_manager_(MasterCommunicator::getInstance()),
    machine_(MasterCommunicator::getInstance()) {
  ui->setupUi(this);
  xsmotion = new SimpleMotion( "X_AXIS" );
  ysmotion = new SimpleMotion( "Y_AXIS" );
  zsmotion = new SimpleMotion( "Z_AXIS" );

  connect( ui->actionConnection, SIGNAL(triggered()), SLOT(openConnectionForm()));

  // Fixed Position
  speedSliderSpin = new SliderSpin( this, ui->speedSlider, ui->speedSpinBox, ui->speedUnitComboBox, UnitConvPtr(new SpeedConv(1, 667)) );
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

  machine_.setProgressCallback( boost::bind( &MainWindow::progressUpdate, this, _1 ) );
  machine_.setEmergencyCallback( boost::bind( &MainWindow::emergencyUpdate, this, _1 ) );
  ui->emergencyLabel->setText("<font style='background-color:red' size='10'>Emergência</font>");
  ui->emergencyLabel->setVisible( false );
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

  //MasterCommunicator::getInstance().startHoming( AXIS_ALL );
  machine_.startHomingSequence( "X;YZ" );
}

//-----------------------------------------------------------------------------
void MainWindow::setLimits( Vector3I &init, Vector3I &final ) {
  double xv = ui->xinitSpinBox->value(),
         yv = ui->yinitSpinBox->value(),
         zv = ui->zinitSpinBox->value();

  std::string cur_unit = ui->initFinalPosUnitComboBox->currentText().toStdString(),
              unit = "pulsos";
  UnitConvPtr xconv = xposSliderSpin->getConversionObj(),
              yconv = yposSliderSpin->getConversionObj(),
              zconv = zposSliderSpin->getConversionObj();
  xconv->getConv(unit);
  init = Vector3I( xconv->convertFromTo( xv, cur_unit, unit ),
                   yconv->convertFromTo( yv, cur_unit, unit ),
                   zconv->convertFromTo( zv, cur_unit, unit ) );
  final = Vector3I( xconv->convertFromTo( ui->xfinalSpinBox->value() , cur_unit, unit ),
                    yconv->convertFromTo( ui->yfinalSpinBox->value(),  cur_unit, unit ),
                    zconv->convertFromTo( ui->zfinalSpinBox->value(),  cur_unit, unit ));
  machine_.setLimits( init, final );
}

//-----------------------------------------------------------------------------
void MainWindow::on_executeButton_clicked() {
  std::string pos_unit("pulsos"), spd_unit("rpm");
  int idx = ui->tabWidget->currentIndex();

  Vector3I  init, final;
  setLimits( init, final );
  machine_.setAngularOffset( ui->xangleSpinBox->value() );
  double length = init.distance( final );
  if( machine_.busy() ) {
    machine_.cancel();
    printf(">>> Cancelled <<<\n");
  } else {
    if( idx == 2 ) {
      machine_.setMaxSpeed( speedSliderSpin->value( spd_unit ), AXIS_ALL );

      AbstractProtocol::ConcurrentCmmd32 cmd;
      cmd[ X_AXIS ] =  xposSliderSpin->value( pos_unit );
      cmd[ Y_AXIS ] = -yposSliderSpin->value( pos_unit );
      cmd[ Z_AXIS ] =  zposSliderSpin->value( pos_unit );

      machine_.sendPosCmmds( cmd );
    } else if( idx == 3 ) { // Longitudinal
      int32_t tidx = ui->longTrajectoryComboBox->currentIndex(),
              weldspd = sbWeldSpeedSliderSpin->value( spd_unit );
      if( tidx == 0 ) {
        int32_t fwlen = fwLengthSliderSpin->value( pos_unit );
        AbsTrajectoryPtr sb( new SwitchBackTrajectory(fwlen, weldspd, length) );
        machine_.executeTrajectory( sb );
      } else {
        machine_.executeTrajectory( AbsTrajectoryPtr( new LinearTrajectory( weldspd, length ) ) );
      }
    } else if( idx == 4 ) { // Transversal
      boost::shared_ptr<AbstractTrajectory> tr;
      double freq  = trFreqSliderSpin->value();
      int32_t spd  = trSpeedSliderSpin->value( spd_unit ),
              ampl = trAmplSliderSpin->value( pos_unit ),
              tidx = ui->transvTrajectoryComboBox->currentIndex();
      if( tidx == 1 ) {
        tr.reset( new ETrajectory( spd, freq, ampl, length ) );
      } else if( tidx == 2 ) {
        tr.reset( new DoubleETrajectory( spd, freq, ampl, length ) );
      } else {
        uint32_t sup_stop = ui->supSpinBox->value(),
                 inf_stop = ui->infSpinBox->value();
        tr.reset( new TriangularTrajectory( spd, freq, ampl, sup_stop, inf_stop, length ) );
      }
      machine_.executeTrajectory( tr );
    } else if( idx == 5 ) {
      int32_t spd  = sbtSpeedSliderSpin->value( spd_unit ),
              ampl = sbtAmplSliderSpin->value( pos_unit ),
              len  = sbtLenSliderSpin->value( pos_unit );
      machine_.executeTrajectory( AbsTrajectoryPtr(new Rhombus( ampl, len, ui->sbtOscCountSpinBox->value(), spd, length )));
    }
    ui->executeButton->setText("Cancelar");
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
  ui->xvalueLabel->setText( faultString( machine_.getStatus( FaultBits, X_AXIS ) ).c_str() );
  ui->yvalueLabel->setText( faultString( machine_.getStatus( FaultBits, Y_AXIS ) ).c_str() );
  ui->zvalueLabel->setText( faultString( machine_.getStatus( FaultBits, Z_AXIS ) ).c_str() );
  ui->avalueLabel->setText( faultString( machine_.getStatus( FaultBits, A_AXIS ) ).c_str() );
  ui->bvalueLabel->setText( faultString( machine_.getStatus( FaultBits, B_AXIS ) ).c_str() );

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
  setStatus( machine_.getStatus( FaultBits, X_AXIS ), x_statlabel_ );
  setStatus( machine_.getStatus( FaultBits, Y_AXIS ), y_statlabel_ );
  setStatus( machine_.getStatus( FaultBits, Z_AXIS ), z_statlabel_ );
  setStatus( machine_.getStatus( FaultBits, A_AXIS ), a_statlabel_ );
  setStatus( machine_.getStatus( FaultBits, B_AXIS ), b_statlabel_ );
}

//-----------------------------------------------------------------------------
void MainWindow::on_longTrajectoryComboBox_currentTextChanged(const QString &arg1) {
  if( arg1 == "Linear" )
    ui->advanceRetreatWidget->setHidden(true);
  else
    ui->advanceRetreatWidget->setHidden(false);
}

//-----------------------------------------------------------------------------
void MainWindow::on_transvTrajectoryComboBox_currentTextChanged(const QString &arg1) {
  if( arg1 == "Triangular" )
    ui->stopTimeWidget->setHidden(false);
  else
    ui->stopTimeWidget->setHidden(true);
}

//-----------------------------------------------------------------------------
void MainWindow::keyPressEvent(QKeyEvent *event) {
  switch (event->key()) {
  case Qt::Key_8:
    printf("up");
    keypress_manager_.angularUp();
    break;
  case Qt::Key_2:
    printf("down");
    keypress_manager_.angularDown();
    break;
  case Qt::Key_4:
    printf("left");
    keypress_manager_.angularLeft();
    break;
  case Qt::Key_6:
    printf("right");
    keypress_manager_.angularRight();
    break;
  case Qt::Key_Less:
  case Qt::Key_Comma:
    printf("< left");
    keypress_manager_.xaxisLeft();
    break;
  case Qt::Key_Period:
  case Qt::Key_Greater:
    printf("> right");
    keypress_manager_.xaxisRight();
    break;
  case Qt::Key_PageUp:
    printf("z up");
    keypress_manager_.zaxisUp();
    break;
  case Qt::Key_PageDown:
    printf("z down");
    keypress_manager_.zaxisDown();
    break;
  case Qt::Key_Plus:
    printf("y front");
    keypress_manager_.yaxisFront();
    break;
  case Qt::Key_Minus:
    printf("y back");
    keypress_manager_.yaxisBack();
    break;
  default:
    QMainWindow::keyPressEvent(event);
  };
    fflush( stdout );
}

//-----------------------------------------------------------------------------
void MainWindow::on_invertDirectionButton_clicked() {
  double xt = ui->xinitSpinBox->value(),
         yt = ui->yinitSpinBox->value(),
         zt = ui->zinitSpinBox->value();
  ui->xinitSpinBox->setValue( ui->xfinalSpinBox->value() );
  ui->yinitSpinBox->setValue( ui->yfinalSpinBox->value() );
  ui->zinitSpinBox->setValue( ui->zfinalSpinBox->value() );
  ui->xfinalSpinBox->setValue( xt );
  ui->yfinalSpinBox->setValue( yt );
  ui->zfinalSpinBox->setValue( zt );
}

//-----------------------------------------------------------------------------
void MainWindow::on_markFinalPositionButton_clicked() {
    Vector3D pos = machine_.currentPosition();
    ui->xfinalSpinBox->setValue( pos.x() );
    ui->yfinalSpinBox->setValue( pos.y() );
    ui->zfinalSpinBox->setValue( pos.z() );
}

//-----------------------------------------------------------------------------
void MainWindow::on_markInitPositionButton_clicked() {
    Vector3D pos = machine_.currentPosition();
    ui->xinitSpinBox->setValue( pos.x() );
    ui->yinitSpinBox->setValue( pos.y() );
    ui->zinitSpinBox->setValue( pos.z() );
}

//-----------------------------------------------------------------------------
void MainWindow::progressUpdate( double p ) {
  int percent = 100 * p;
  QMetaObject::invokeMethod( ui->progressBar,
                             "setValue",
                             Qt::QueuedConnection,
                             Q_ARG(int, percent) );
  if( percent == 100 )
    QMetaObject::invokeMethod( this,
                               "executionFinished",
                               Qt::QueuedConnection );
}

//-----------------------------------------------------------------------------
void MainWindow::emergencyUpdate( bool e ) {
  QMetaObject::invokeMethod( this,
                             "emergencyNotify",
                             Qt::QueuedConnection,
                             Q_ARG(bool, e) );
}

//-----------------------------------------------------------------------------
void MainWindow::emergencyNotify( bool e ) {
  ui->emergencyLabel->setVisible( e );
  ui->tabWidget->setCurrentIndex( 0 );
  QTimer::singleShot( 1000, this, SLOT(on_getValuesButton_clicked()) );
}

//-----------------------------------------------------------------------------
void MainWindow::executionFinished() {
  ui->executeButton->setText( "Executar" );
}

//-----------------------------------------------------------------------------


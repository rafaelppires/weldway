#include <QtWidgets>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QLineEdit>
#include <formconnection.h>
#include <linear_transform.h>

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
  vOrSliderSpin = new SliderSpin( this, ui->vorientationSlider, ui->vorientationSpinBox, NULL, UnitConvPtr(new UnitConv(-90,90)) );
  hOrSliderSpin = new SliderSpin( this, ui->horientationSlider, ui->horientationSpinBox, NULL, UnitConvPtr(new UnitConv(-90,90)) );

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

  scene_ = new TrajectoryScene;
  ui->graphicsView->setRenderHint(QPainter::Antialiasing);
  ui->graphicsView->setInteractive(true);
  ui->graphicsView->setScene( scene_ );

  oscillationsSetup();
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
}

//-----------------------------------------------------------------------------
void MainWindow::oscillationsSetup() {
  oscillationsToolBox = new QToolBox(ui->oscillationTab);
  oscillationsToolBox->setObjectName(QStringLiteral("oscillationsToolBox"));
  oscillationsToolBox->setStyleSheet(QLatin1String(" QToolBox::tab {\n"
"     background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                 stop: 0 #E1E1E1, stop: 0.4 #DDDDDD,\n"
"                                 stop: 0.5 #D8D8D8, stop: 1.0 #D3D3D3);\n"
"     border-radius: 5px;\n"
"	 color: rgb(145, 145, 145);\n"
" }\n"
"\n"
" QToolBox::tab:selected {\n"
"	 font: 75;\n"
"     color: black;\n"
" }"));

  longit_panel_ = new LongitudinalWidget();
  oscillationsToolBox->addItem( longit_panel_, QStringLiteral("Longitudinais") );

  transv_panel_ = new TransversalWidget( this );
  oscillationsToolBox->addItem( transv_panel_, QStringLiteral("Transversais e Combinados") );

  custom_panel_ = new CustomTrajectoryWidget( this, scene_ );
  oscillationsToolBox->addItem( custom_panel_, QStringLiteral("Customizados") );
  connect(scene_, SIGNAL(sceneClicked(Vector2D)), custom_panel_, SLOT(sceneClicked(Vector2D)));

  ui->oscillationTabLayout->addWidget(oscillationsToolBox);

  //-----------------
  trajSetupToolBox = new QToolBox(ui->trajectoryTab);
  trajSetupToolBox->setObjectName(QStringLiteral("trajSetupToolBox"));
  trajSetupToolBox->setStyleSheet(oscillationsToolBox->styleSheet());

  trajparams_panel_ = new TrajRectParamsWidget( machine_, xposSliderSpin->getConversionObj(), yposSliderSpin->getConversionObj(), zposSliderSpin->getConversionObj(), this );
  trajSetupToolBox->addItem( trajparams_panel_, QStringLiteral("Eixo Longitudinal Retilíneo") );

  trajcircle_panel_ = new CircularWidget( machine_, xposSliderSpin->getConversionObj(), yposSliderSpin->getConversionObj(), zposSliderSpin->getConversionObj(), this );
  trajSetupToolBox->addItem( trajcircle_panel_, QStringLiteral("Eixo Longitudinal Circular") );

  ui->trajectoryTabLayout->addWidget(trajSetupToolBox);
}
//-----------------------------------------------------------------------------
OscillationWidget* MainWindow::activeOscillationWidget() {
  std::string cur = oscillationsToolBox->currentWidget()->objectName().toStdString();
  if( cur == "LongitudinalWidget" ) {
    return longit_panel_;
  } else if ( cur == "TransversalWidget" ) {
    return transv_panel_;
  } else if( cur == "CustomTrajectoryWidget" ) {
    return custom_panel_;
  } else {
    return 0;
  }
}
//-----------------------------------------------------------------------------
TransformationWidget* MainWindow::activeTransformWidget() {
  std::string cur = trajSetupToolBox->currentWidget()->objectName().toStdString();
  if( cur == "CircularWidget" ) {
    return trajcircle_panel_;
  } else if ( cur == "TrajRectParamsWidget" ) {
    return trajparams_panel_;
  } else {
    return 0;
  }
}

//-----------------------------------------------------------------------------
void MainWindow::on_findZeroPushButton_clicked() {
  //xsmotion->startHoming();
  //ysmotion->startHoming();
  //zsmotion->startHoming();

  //MasterCommunicator::getInstance().startHoming( AXIS_ALL );
  machine_.startHomingSequence();
}

//-----------------------------------------------------------------------------
void MainWindow::setLimits( ) {
  TransformationWidget *tw = activeTransformWidget();
  if( tw )
  machine_.setLimits( tw->initPos(), tw->initTorch() );
}

//-----------------------------------------------------------------------------
void MainWindow::on_executeButton_clicked() {
  std::string pos_unit("pulsos"), spd_unit("rpm");
  int idx = ui->tabWidget->currentIndex();

  setLimits();
  double xangle = trajparams_panel_->xangle();
  machine_.setAngularOffset( xangle );

  if( machine_.busy() ) {
    machine_.cancel();
    printf(">>> Cancelled <<<\n");
  } else if( ui->tabWidget->currentWidget()->objectName() == "oscillationTab" ) {    
    OscillationWidget *ow = activeOscillationWidget();
    TransformationWidget *tw = activeTransformWidget();
    if( ow && tw ) {
      executing_trajectory_ = ow->trajectory( tw->transformation() );
    }

    if( executing_trajectory_ ) {
      machine_.executeTrajectory( executing_trajectory_ );
      ui->executeButton->setText("Cancelar");
    }
  } else {
    executing_trajectory_.reset();
    if( idx == 2 ) {
      Vector3D pos( xposSliderSpin->value( pos_unit ),
                    yposSliderSpin->value( pos_unit ),
                    zposSliderSpin->value( pos_unit ) );
      gotoPosition( pos, Vector2D(hOrSliderSpin->value(""),vOrSliderSpin->value("")), speedSliderSpin->value( spd_unit ) );
    }
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
void MainWindow::on_correctButton_clicked() {
  /*
  std::string pos_unit("pulsos"), spd_unit("rpm");
  int idx = ui->tabWidget->currentIndex();
  Rhombus *rt = 0;
  if( idx == 3 ) { // Longitudinal
     int32_t tidx = ui->longTrajectoryComboBox->currentIndex(),
             weldspd = sbWeldSpeedSliderSpin->value( spd_unit );
     SwitchBackTrajectory *sb = 0;
     if( tidx == 0 &&
         (sb = dynamic_cast<SwitchBackTrajectory *>( executing_trajectory_.get() ))) {
       int32_t fwlen = fwLengthSliderSpin->value( pos_unit );
       sb->applyCorrection( fwlen, weldspd );
     } else if(LinearTrajectory *l = dynamic_cast<LinearTrajectory*>( executing_trajectory_.get() )) {
       l->applyCorrection( weldspd );
     }
   } else if( idx == 4 ) { // Transversal
     double lmbd = trLmbdSliderSpin->value( pos_unit ),
            spd  = trSpeedSliderSpin->value( spd_unit ),
            ampl = trAmplSliderSpin->value( pos_unit );
     int32_t tidx = ui->transvTrajectoryComboBox->currentIndex();
     ETrajectory *et = 0;
     DoubleETrajectory *de = 0;
     if( tidx == 1 &&
         (et = dynamic_cast<ETrajectory *>( executing_trajectory_.get() ))) {
       double rho = eRhoSliderSpin->value();
       et->applyCorrection( spd, lmbd, ampl, rho );
     } else if( tidx == 2 &&
                (de = dynamic_cast<DoubleETrajectory *>( executing_trajectory_.get() ))) {
       de->applyCorrection( spd, lmbd, ampl );
     } else if( TriangularTrajectory *tt = dynamic_cast<TriangularTrajectory *>( executing_trajectory_.get() ) ) {
       uint32_t sup_stop = 0,
                inf_stop = 0;
       tt->applyCorrection( spd, lmbd, ampl, sup_stop, inf_stop );
     }
   } else if( idx == 5 &&
              (rt = dynamic_cast<Rhombus *>( executing_trajectory_.get() )) ) {
     int32_t spd  = sbtSpeedSliderSpin->value( spd_unit ),
             ampl = sbtAmplSliderSpin->value( pos_unit ),
             len  = sbtLenSliderSpin->value( pos_unit );
     rt->applyCorrection( ampl, len, ui->sbtOscCountSpinBox->value(), spd );
   }
   */
}


//-----------------------------------------------------------------------------
void MainWindow::render( PositionVector &v ) {
  scene_->clear();
  PositionVector::iterator it = v.begin(), end = v.end(), n = it;
  QPen pen;
  pen.setWidth(2);
  for(; it != end; ++it ) {
   ++n;
   if( n != end ) { scene_->addLine( it->x(), -it->y(), n->x(), -n->y(), pen ); }
  }
  ui->graphicsView->fitInView( scene_->itemsBoundingRect(), Qt::KeepAspectRatio );
}

//-----------------------------------------------------------------------------
void MainWindow::redraw() {
  if( !redraw_timer_ ) {
    redraw_timer_.reset( new QTimer(this) );
    redraw_timer_->setSingleShot( true );
    connect( redraw_timer_.get(), SIGNAL(timeout()), this, SLOT(actuallyRedraw()));
  }
  redraw_timer_->start( 100 );
}

//-----------------------------------------------------------------------------
void MainWindow::actuallyRedraw() {
  OscillationWidget *w = activeOscillationWidget();
  if( w ) {
    PositionVector v;
    w->draft( v );
    render( v );
  }
}

//-----------------------------------------------------------------------------
void MainWindow::on_tabWidget_currentChanged( int index ) {
  redraw();
}

//-----------------------------------------------------------------------------
void MainWindow::on_initPosButton_clicked() {
  TransformationWidget *tw = activeTransformWidget();
  gotoPosition( tw->initPos(), tw->initTorch(), 650 );
}

//-----------------------------------------------------------------------------
void MainWindow::on_finalPosButton_clicked() {
  TransformationWidget *tw = activeTransformWidget();
  gotoPosition( tw->finalPos(), tw->finalTorch(), 650 );
}

//-----------------------------------------------------------------------------
void MainWindow::gotoPosition( const Vector3I &pos, const Vector2D &torch, uint32_t spd ) {
  Vector2I angpos = machine_.angularOffset( ANGULAR_VERTICAL,   torch.y() ) +
                    machine_.angularOffset( ANGULAR_HORIZONTAL, torch.x() );
  machine_.gotoPosition(pos, spd, angpos);
}

//-----------------------------------------------------------------------------

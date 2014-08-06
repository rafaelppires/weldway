#include "transversalwidget.h"
#include "ui_transversalwidget.h"
#include <triangular.h>
#include <double_e.h>
#include <e_trajectory.h>
#include <double8.h>
#include <bricks.h>
#include <double_triang.h>
#include <double_triang2nd.h>

//-----------------------------------------------------------------------------
TransversalWidget::TransversalWidget(QWidget *parent) :
  OscillationWidget(parent),
  ui(new Ui::TransversalWidget) {
  ui->setupUi(this);
  
  trSpeedSliderSpin = new SliderSpin( this, ui->trWeldSpeedSlider, ui->trWeldSpeedSpinBox, ui->trWeldSpeedUnitComboBox, UnitConvPtr(new SpeedConv(0, 30)) );
  trAmplSliderSpin  = new SliderSpin( this, ui->trAmplitudeSlider, ui->trAmplitudeSpinBox, ui->trAmplitudeUnitComboBox, UnitConvPtr(new PositionConv(0, 50)) );
  trLmbdSliderSpin  = new SliderSpin( this, ui->trLambdaSlider,    ui->trLambdaSpinBox,    ui->trLambdaUnitComboBox,    UnitConvPtr(new PositionConv(1, 14)) );

  connect( trSpeedSliderSpin, SIGNAL(valueChanged()), this, SLOT(on_frequency_changed()) );
  connect( trLmbdSliderSpin,  SIGNAL(valueChanged()), this, SLOT(on_frequency_changed()) );
  connect( trAmplSliderSpin,  SIGNAL(valueChanged()), parent, SLOT(redraw()) );
  
  eRhoSliderSpin = new SliderSpin( this, ui->rhoSlider, ui->rhoSpinBox, NULL, UnitConvPtr(new UnitConv(0,0.99)) );
  connect( eRhoSliderSpin,  SIGNAL(valueChanged()), parent, SLOT(redraw()));
  
  on_transvTrajectoryComboBox_currentTextChanged( ui->transvTrajectoryComboBox->currentText() );

  connect( this, SIGNAL(updated()), parent, SLOT(redraw()) );
  connect( ui->stopInfSpinBox, SIGNAL(valueChanged(double)), parent, SLOT(redraw()));
  connect( ui->stopSupSpinBox, SIGNAL(valueChanged(double)), parent, SLOT(redraw()));
}

//-----------------------------------------------------------------------------
TransversalWidget::~TransversalWidget() {
  delete ui;
  
  delete trSpeedSliderSpin;
  delete trAmplSliderSpin;
  delete trLmbdSliderSpin;
  
  delete eRhoSliderSpin;
}

//-----------------------------------------------------------------------------
void TransversalWidget::on_frequency_changed() {
  double f = trSpeedSliderSpin->value("mm/s") / trLmbdSliderSpin->value("mm");
  char value[64];
  sprintf( value, "%.1f", f );
  ui->frLabel->setText( value );
  emit updated();
}

//-----------------------------------------------------------------------------
AbsTrajectoryPtr TransversalWidget::trajectory(double xangle, Vector3D rotate) {
  std::string pos_unit("pulsos"), spd_unit("rpm");
  double lmbd = trLmbdSliderSpin->value( pos_unit ),
         spd  = trSpeedSliderSpin->value( spd_unit ),
         ampl = trAmplSliderSpin->value( pos_unit );
  int32_t tidx = ui->transvTrajectoryComboBox->currentIndex();
  if( tidx == 1 ) {
    double rho = eRhoSliderSpin->value();
    return AbsTrajectoryPtr( new ETrajectory( spd, lmbd, ampl, rho, rotate, xangle) );
  } else if( tidx == 2 ) {
    return AbsTrajectoryPtr( new DoubleETrajectory( spd, lmbd, ampl, rotate, xangle) );
  } else if( tidx == 3 ) {
    double rho = eRhoSliderSpin->value();
    return AbsTrajectoryPtr( new Double8Trajectory( spd, lmbd, ampl, rho, rotate, xangle) );
  } else if( tidx == 4 ){
    return AbsTrajectoryPtr( new BricksTrajectory( spd, ampl, rotate, xangle) );
  } else if( tidx == 5 ){
    return AbsTrajectoryPtr( new DoubleTriangularTraj( spd, lmbd, ampl, rotate, xangle) );
  } else if( tidx == 6 ){
    return AbsTrajectoryPtr( new DoubleTriangular2ndTraj( spd, lmbd, ampl, rotate, xangle) );
  } else {
    double sup_stop = ui->stopSupSpinBox->value()/100,
           inf_stop = ui->stopInfSpinBox->value()/100,
           factor = ui->spdExtrSpinBox->value();
    return AbsTrajectoryPtr( new TriangularTrajectory( spd, lmbd, ampl, sup_stop, inf_stop, factor, rotate, xangle) );
  }
}
//-----------------------------------------------------------------------------
void TransversalWidget::draft( PositionVector &v ) {
  std::string pos_unit("pulsos"), spd_unit("rpm");
  double lmbd = trLmbdSliderSpin->value( pos_unit ),
         spd  = trSpeedSliderSpin->value( spd_unit ),
         ampl = trAmplSliderSpin->value( pos_unit );
  int32_t tidx = ui->transvTrajectoryComboBox->currentIndex();
  if( tidx == 1 ) {
    double rho = eRhoSliderSpin->value();
    ETrajectory::draft(v, spd, lmbd, ampl, rho);
  } else if( tidx == 2 ) {
    DoubleETrajectory::draft(v, spd, lmbd, ampl);
  } else if( tidx == 3 ) {
    double rho = eRhoSliderSpin->value();
    Double8Trajectory::draft(v, spd, lmbd, ampl, rho);
  } else if( tidx == 4 ){
    BricksTrajectory::draft(v, spd, ampl);
  } else if( tidx == 5 ){
    DoubleTriangularTraj::draft(v, spd, lmbd, ampl);
  } else if( tidx == 6 ){
    DoubleTriangular2ndTraj::draft(v, spd, lmbd, ampl);
  } else {
    double sup_stop = ui->stopSupSpinBox->value()/100,
           inf_stop = ui->stopInfSpinBox->value()/100,
           factor = ui->spdExtrSpinBox->value();
    TriangularTrajectory::draft(v, spd, lmbd, ampl, sup_stop, inf_stop, factor);
  }
}

//-----------------------------------------------------------------------------
void TransversalWidget::on_transvTrajectoryComboBox_currentTextChanged(const QString &arg1) {
  if( arg1 == "Triangular" ) {
    ui->stopTimeWidget->setHidden(false);
    ui->rhoWidget->setHidden(true);
  } else {
    ui->rhoWidget->setHidden( arg1 != "E" && arg1 != "Duplo 8" );
    ui->stopTimeWidget->setHidden(true);
  }
  emit updated();
}

//-----------------------------------------------------------------------------

#include "longitudinalwidget.h"
#include "ui_longitudinalwidget.h"
#include <switch_back.h>

//-----------------------------------------------------------------------------
LongitudinalWidget::LongitudinalWidget(QWidget *parent) :
  OscillationWidget(parent),
  ui(new Ui::LongitudinalWidget) {
  ui->setupUi(this);

  sbWeldSpeedSliderSpin = new SliderSpin( this, ui->sbWeldSpeedSlider, ui->sbWeldSpeedSpinBox, ui->sbWeldSpeedUnitComboBox, UnitConvPtr(new SpeedConv(1, 50)) );
  fwSpeedSliderSpin  = new SliderSpin( this, ui->fwSpeedSlider, ui->fwSpeedSpinBox, ui->fwSpeedUnitComboBox, UnitConvPtr(new SpeedConv(4, 200)) );
  fwLengthSliderSpin = new SliderSpin( this, ui->fwLengthSlider, ui->fwLengthSpinBox, ui->fwLengthUnitComboBox, UnitConvPtr(new PositionConv(4, 20)) );
  bwSpeedSliderSpin  = new SliderSpin( this, ui->bwSpeedSlider, ui->bwSpeedSpinBox, ui->bwSpeedUnitComboBox, UnitConvPtr(new SpeedConv(2, 100)) );
  bwLengthSliderSpin = new SliderSpin( this, ui->bwLengthSlider, ui->bwLengthSpinBox, ui->bwLengthUnitComboBox, UnitConvPtr(new PositionConv(2, 10)) );
  
  sbWeldSpeedSliderSpin->addMultiplier( fwSpeedSliderSpin, 4.0 );
  sbWeldSpeedSliderSpin->addMultiplier( bwSpeedSliderSpin, 2.0 );

  fwSpeedSliderSpin->addMultiplier( bwSpeedSliderSpin, 0.5 );
  fwLengthSliderSpin->addMultiplier( bwLengthSliderSpin, 0.5 );
}

//-----------------------------------------------------------------------------
LongitudinalWidget::~LongitudinalWidget() {
  delete sbWeldSpeedSliderSpin;
  delete fwSpeedSliderSpin;
  delete fwLengthSliderSpin;
  delete bwSpeedSliderSpin;
  delete bwLengthSliderSpin;
  
  delete ui;
}

//-----------------------------------------------------------------------------
AbsTrajectoryPtr LongitudinalWidget::trajectory(double xangle, Vector3D rotate) {
  std::string pos_unit("pulsos"), spd_unit("rpm");
  int32_t tidx = ui->longTrajectoryComboBox->currentIndex(),
		  weldspd = sbWeldSpeedSliderSpin->value( spd_unit );
  if( tidx == 0 ) {
	int32_t fwlen = fwLengthSliderSpin->value( pos_unit );
    return AbsTrajectoryPtr( new SwitchBackTrajectory(fwlen, weldspd, rotate, xangle) );
  } else {
    return AbsTrajectoryPtr( new LinearTrajectory( weldspd, rotate, xangle) );
  }
}

//-----------------------------------------------------------------------------
void LongitudinalWidget::on_longTrajectoryComboBox_currentTextChanged(const QString &arg1) {
  if( arg1 == "Linear" )
    ui->advanceRetreatWidget->setHidden(true);
  else
    ui->advanceRetreatWidget->setHidden(false);
}

//-----------------------------------------------------------------------------

#include "longitudinalwidget.h"
#include "ui_longitudinalwidget.h"
#include <switch_back.h>
#include <linear_transform.h>

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
  spdratioSliderSpin = new SliderSpin( this, ui->speedRatioSlider, ui->speedRatioSpinBox, NULL, UnitConvPtr(new UnitConv(0.1,2)) );

  //fwLengthSliderSpin->addMultiplier( bwLengthSliderSpin, 0.5 );
  bwLengthSliderSpin->addConstraint( fwLengthSliderSpin, SliderSpin::GREATER_THAN );

  connect(spdratioSliderSpin, SIGNAL(valueChanged()), this, SLOT(changeRatios()));
  spdratioSliderSpin->setValue(0.5);
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
void LongitudinalWidget::changeRatios() {
  double r = spdratioSliderSpin->value(""),
        fw = fwLengthSliderSpin->value(""),
        bw = bwLengthSliderSpin->value(""),
      mult = (bw+fw*r)/(fw-bw);

  sbWeldSpeedSliderSpin->addMultiplier( fwSpeedSliderSpin, mult/r );
  sbWeldSpeedSliderSpin->addMultiplier( bwSpeedSliderSpin, mult );
  fwSpeedSliderSpin->addMultiplier( bwSpeedSliderSpin, r );

  sbWeldSpeedSliderSpin->resetValue();
}

//-----------------------------------------------------------------------------
AbsTrajectoryPtr LongitudinalWidget::trajectory(TrajectoryTransformPtr tt) {
  std::string pos_unit("pulsos"), spd_unit("rpm");
  int32_t tidx = ui->longTrajectoryComboBox->currentIndex(),
       weldspd = sbWeldSpeedSliderSpin->value( spd_unit );

  if( tidx == 0 ) {
    int32_t fwlen = fwLengthSliderSpin->value( pos_unit ),
            bwlen = bwLengthSliderSpin->value( pos_unit );
    uint8_t polarity = (ui->fstNegRadioButton->isChecked() ? 0x1 : 0) |
                       (ui->SndNegRadioButton->isChecked() ? 0x2 : 0) |
                       (ui->TrdNegRadioButton->isChecked() ? 0x4 : 0);
    return AbsTrajectoryPtr( new SwitchBackTrajectory(fwlen, bwlen, weldspd, spdratioSliderSpin->value(""), polarity, tt) );
  } else {
    return AbsTrajectoryPtr( new LinearTrajectory( weldspd, tt) );
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

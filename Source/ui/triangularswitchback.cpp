#include "triangularswitchback.h"
#include "ui_triangularswitchback.h"
#include <rhombus.h>

//-----------------------------------------------------------------------------
TriangularSwitchback::TriangularSwitchback(QWidget *parent) :
  OscillationWidget(parent),
  ui(new Ui::TriangularSwitchback) {
  ui->setupUi(this);
  
  sbtSpeedSliderSpin = new SliderSpin( this, ui->sbtWeldSpeedSlider, ui->sbtWeldSpeedSpinBox, ui->sbtWeldSpeedUnitComboBox, UnitConvPtr(new SpeedConv(0,100)) );
  sbtAmplSliderSpin  = new SliderSpin( this, ui->sbtAmplitudeSlider, ui->sbtAmplitudeSpinBox, ui->sbtAmplitudeUnitComboBox, UnitConvPtr(new PositionConv(0,50)) );
  sbtLenSliderSpin   = new SliderSpin( this, ui->sbtForwardLenSlider, ui->sbtForwardLenSpinBox, ui->sbtForwardLenUnitComboBox, UnitConvPtr(new PositionConv(0,30)) );
  
  connect(sbtAmplSliderSpin, SIGNAL(valueChanged()), parent, SLOT(redraw()));
  connect(sbtLenSliderSpin, SIGNAL(valueChanged()),  parent, SLOT(redraw()));
  connect(ui->sbtOscCountSpinBox, SIGNAL(valueChanged(int)), parent, SLOT(redraw()) );
}

//-----------------------------------------------------------------------------
TriangularSwitchback::~TriangularSwitchback() {
  delete ui;
  
  delete sbtSpeedSliderSpin;
  delete sbtAmplSliderSpin;
  delete sbtLenSliderSpin;
}

//-----------------------------------------------------------------------------
AbsTrajectoryPtr TriangularSwitchback::trajectory(double xangle, Vector3D rotate) {
  std::string pos_unit("pulsos"), spd_unit("rpm");
  int32_t spd  = sbtSpeedSliderSpin->value( spd_unit ),
          ampl = sbtAmplSliderSpin->value( pos_unit ),
          len  = sbtLenSliderSpin->value( pos_unit );
  return AbsTrajectoryPtr(new Rhombus( ampl, len, ui->sbtOscCountSpinBox->value(), spd, rotate, xangle));
}

//-----------------------------------------------------------------------------
void TriangularSwitchback::draft(PositionVector &v) {
  std::string pos_unit("pulsos"), spd_unit("rpm");
  int32_t spd  = sbtSpeedSliderSpin->value( spd_unit ),
          ampl = sbtAmplSliderSpin->value( pos_unit ),
          len  = sbtLenSliderSpin->value( pos_unit );
  Rhombus::draft(v, ampl, len, ui->sbtOscCountSpinBox->value(), spd);
}

//-----------------------------------------------------------------------------

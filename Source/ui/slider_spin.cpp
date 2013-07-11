#include <slider_spin.h>
#include <QLineEdit>

SliderSpin::SliderSpin( QObject *parent, QSlider *slider, QDoubleSpinBox *spinbox, QComboBox *unitcombo, UnitConv &unit ) :
      QObject(parent), slider_(slider), spinbox_(spinbox), unitcombo_(unitcombo),
      spin_commanded_(false), slider_commanded_(false), unit_(unit) {
  connect( slider,  SIGNAL(valueChanged(int)),    SLOT(changeSpinBox(int)) );
  connect( spinbox, SIGNAL(editingFinished()),    SLOT(changeSlider()) );
  connect( spinbox, SIGNAL(valueChanged(double)), SLOT(onSpinValueChanged()) );

  cur_unit_ = unitcombo->currentText();
  onUnitComboCurrentTextChanged( cur_unit_ );
}

//-----------------------------------------------------------------------------
void SliderSpin::changeSpinBox( int slider_value ) {
  if( slider_commanded_ ) { slider_commanded_ = false; return; }
  int min = slider_->minimum(), max = slider_->maximum();
  double ratio = double(slider_value - min) / (max-min),
         value = toCurrent( unit_.from + ratio * unit_.range );
  spinbox_->setValue( value );
  spin_commanded_ = true;
}

//-----------------------------------------------------------------------------
void SliderSpin::changeSlider() {
  if( spin_commanded_ ) { spin_commanded_ = false; return; }
  double spin_value = fromCurrent( spinbox_->value() );
  int min = slider_->minimum(),
      max = slider_->maximum(),
      calc_value = min + (max-min) * (spin_value - unit_.from) / unit_.range;
  slider_->setValue( calc_value );
  slider_commanded_ = true;
}

//-----------------------------------------------------------------------------
void SliderSpin::onSpinValueChanged() {
  if( spinbox_->findChild<QLineEdit*>()->hasSelectedText() )
    changeSlider();
}

//-----------------------------------------------------------------------------
void SliderSpin::onUnitComboCurrentTextChanged( const QString &arg1 ) {
  double conv_pre = unit_.getConv( cur_unit_.toStdString() ),
         conv_cur = unit_.getConv( arg1.toStdString() ),
         conv = conv_cur / conv_pre,
         value_pre = spinbox_->value();
  spinbox_->setMaximum( unit_.to * conv_cur );
  spinbox_->setMinimum( unit_.from * conv_cur );
  spinbox_->setSingleStep( unit_.range * conv_cur / 10. );
  spinbox_->setValue( value_pre * conv );
  cur_unit_ = arg1;
}

//-----------------------------------------------------------------------------
double SliderSpin::toCurrent( double value ) {
  return value * unit_.getConv( unitcombo_->currentText().toStdString() );
}

//-----------------------------------------------------------------------------
double SliderSpin::fromCurrent( double value ) {
  return value / unit_.getConv( unitcombo_->currentText().toStdString() );
}

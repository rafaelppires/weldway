#include <slider_spin.h>
#include <QLineEdit>

SliderSpin::SliderSpin(QObject *parent, QSlider *slider, QDoubleSpinBox *spinbox, QComboBox *unitcombo, UnitConvPtr unit ) :
      QObject(parent), slider_(slider), spinbox_(spinbox), unitcombo_(unitcombo),
      spin_commanded_(false), slider_commanded_(false), unit_(unit), cur_unit_("") {
  connect( slider,    SIGNAL(valueChanged(int)),    SLOT(changeSpinBox(int)) );
  connect( spinbox,   SIGNAL(editingFinished()),    SLOT(changeSlider()) );
  connect( spinbox,   SIGNAL(valueChanged(double)), SLOT(onSpinValueChanged()) );
  if( unitcombo ) {
    connect( unitcombo, SIGNAL(currentTextChanged(QString)), SLOT(onUnitComboCurrentTextChanged(QString)) );
    cur_unit_ = unitcombo->currentText();
  }
  onUnitComboCurrentTextChanged( cur_unit_ );
}

//-----------------------------------------------------------------------------
void SliderSpin::changeSpinBox( int slider_value ) {
  if( slider_commanded_ ) { slider_commanded_ = false; return; }
  int min = slider_->minimum(), max = slider_->maximum();
  double ratio = double(slider_value - min) / (max-min),
         value = toCurrent( unit_->from + ratio * unit_->range );
  spin_commanded_ = true;
  spinbox_->setValue( value );
}

//-----------------------------------------------------------------------------
void SliderSpin::changeSlider() {
  if( spin_commanded_ ) { spin_commanded_ = false; return; }
  double spin_value = fromCurrent( spinbox_->value() );
  int min = slider_->minimum(),
      max = slider_->maximum(),
      calc_value = min + (max-min) * (spin_value - unit_->from) / unit_->range;
  slider_commanded_ = true;
  slider_->setValue( calc_value );
}

//-----------------------------------------------------------------------------
void SliderSpin::setValue( double v ) {
  spinbox_->setValue( toCurrent(v) );
  changeSlider();
}

//-----------------------------------------------------------------------------
void SliderSpin::onSpinValueChanged() {
  if( spinbox_->findChild<QLineEdit*>()->hasSelectedText() )
    changeSlider();

  MultiplierMap::iterator it = multipliers_.begin(), end = multipliers_.end();
  for(; it != end; ++it ) {
    it->first->setValue( it->second * fromCurrent( spinbox_->value() ) );
  }

  ConstraintsMap::iterator jt = constraints_.begin(), jend = constraints_.end();
  for(; jt != jend; ++jt ) {
    switch( jt->second ) {
    case GREATER_THAN:
      if( jt->first->fromCurrent( jt->first->spinbox_->value() ) <= fromCurrent( spinbox_->value() ) )
        jt->first->setValue( fromCurrent( spinbox_->value() ) + 0.1 );
      break;
    case LESS_THAN:
      if( jt->first->fromCurrent( jt->first->spinbox_->value() ) >= fromCurrent( spinbox_->value() ) )
        jt->first->setValue( fromCurrent( spinbox_->value() ) - 0.1 );
      break;
    case EQUAL_TO:
      if( jt->first->fromCurrent( jt->first->spinbox_->value() ) != fromCurrent( spinbox_->value() ) )
        jt->first->setValue( fromCurrent( spinbox_->value() ) );
      break;
    };
  }

  emit valueChanged();
}

//-----------------------------------------------------------------------------
void SliderSpin::onUnitComboCurrentTextChanged( const QString &arg1 ) {
  double conv_pre = unit_->getConv( cur_unit_.toStdString() ),
         conv_cur = unit_->getConv( arg1.toStdString() ),
         conv = conv_cur / conv_pre,
         value_pre = spinbox_->value();
  spinbox_->setMaximum( unit_->to * conv_cur );
  spinbox_->setMinimum( unit_->from * conv_cur );
  spinbox_->setSingleStep( unit_->range * conv_cur / 10. );
  spinbox_->setValue( value_pre * conv );
  cur_unit_ = arg1;
}

//-----------------------------------------------------------------------------
double SliderSpin::toCurrent( double value ) {
  std::string unit = unitcombo_ ? unitcombo_->currentText().toStdString() : std::string("");
  return unit_->convertTo( value, unit );
}

//-----------------------------------------------------------------------------
double SliderSpin::fromCurrent( double value ) {
  std::string unit = unitcombo_ ? unitcombo_->currentText().toStdString() : std::string("");
  return unit_->convertFrom( value, unit );
}

//-----------------------------------------------------------------------------
double SliderSpin::value( std::string u ) {
  std::string unit = unitcombo_ ? unitcombo_->currentText().toStdString() : std::string("");
  return unit_->convertFromTo( spinbox_->value(), unit, u );
}
//-----------------------------------------------------------------------------
void SliderSpin::resetValue() {
  double x;
  x = fromCurrent( spinbox_->value() );
  setValue( x+0.1 );
  setValue( x );
}

//-----------------------------------------------------------------------------
void SliderSpin::addMultiplier( SliderSpin *ss, double m, bool goon  ) {
  multipliers_[ss] = m;
  if( goon )
    ss->addMultiplier( this, 1./m, false );
}

//-----------------------------------------------------------------------------
void SliderSpin::addConstraint( SliderSpin *ss, Constraints c, bool goon  ) {
  constraints_[ss] = c;

  if( goon )
    switch( c ) {
    case GREATER_THAN:
      ss->addConstraint( this, LESS_THAN, false );
      break;
    case LESS_THAN:
      ss->addConstraint( this, GREATER_THAN, false );
      break;
    case EQUAL_TO:
      ss->addConstraint( this, EQUAL_TO, false );
      break;
    };
}

//-----------------------------------------------------------------------------
UnitConvPtr SliderSpin::getConversionObj() {
  return unit_;
}

//-----------------------------------------------------------------------------

#ifndef _SLIDER_SPIN_H_
#define _SLIDER_SPIN_H_

#include <QSlider>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <units.h>

class SliderSpin : public QObject {
    Q_OBJECT

public:
  enum Constraints {
    GREATER_THAN,
    LESS_THAN,
    EQUAL_TO
  };

  SliderSpin( QObject *parent, QSlider *slider, QDoubleSpinBox *spinbox, QComboBox *unitcombo, UnitConvPtr unit );
  double value( std::string unit = "" );
  void resetValue();
  void setValue( double );
  void addMultiplier( SliderSpin*, double , bool goon = true );
  void addConstraint( SliderSpin *, Constraints, bool goon = true );
  UnitConvPtr getConversionObj();

private slots:
  void changeSpinBox(int);
  void changeSlider();
  void onSpinValueChanged();
  void onUnitComboCurrentTextChanged(const QString&);

signals:
  void valueChanged();

private:
  typedef std::map<SliderSpin*,double> MultiplierMap;
  typedef std::map<SliderSpin*,Constraints> ConstraintsMap;
  double toCurrent( double value );
  double fromCurrent( double value );

  QSlider *slider_;
  QDoubleSpinBox *spinbox_;
  QComboBox *unitcombo_;
  QString cur_unit_;
  UnitConvPtr unit_;
  bool spin_commanded_, slider_commanded_;
  MultiplierMap multipliers_;
  ConstraintsMap constraints_;
};

#endif

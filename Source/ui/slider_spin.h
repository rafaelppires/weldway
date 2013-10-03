#ifndef _SLIDER_SPIN_H_
#define _SLIDER_SPIN_H_

#include <QSlider>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <units.h>

class SliderSpin : public QObject {
    Q_OBJECT

public:
  SliderSpin( QObject *parent, QSlider *slider, QDoubleSpinBox *spinbox, QComboBox *unitcombo, UnitConvPtr unit );
  double value( std::string unit = "" );
  void setValue( double );
  void addMultiplier(SliderSpin*, double , bool goon = true );

private slots:
  void changeSpinBox(int);
  void changeSlider();
  void onSpinValueChanged();
  void onUnitComboCurrentTextChanged(const QString&);

private:
  typedef std::map<SliderSpin*,double> MultiplierMap;
  double toCurrent( double value );
  double fromCurrent( double value );

  QSlider *slider_;
  QDoubleSpinBox *spinbox_;
  QComboBox *unitcombo_;
  QString cur_unit_;
  UnitConvPtr unit_;
  bool spin_commanded_, slider_commanded_;
  MultiplierMap multipliers_;
};

#endif

#ifndef _SLIDER_SPIN_H_
#define _SLIDER_SPIN_H_

#include <QSlider>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <units.h>

class SliderSpin : public QObject {
    Q_OBJECT

public:
  SliderSpin( QObject *parent, QSlider *slider, QDoubleSpinBox *spinbox, QComboBox *unitcombo, UnitConv &unit );

private slots:
  void changeSpinBox(int);
  void changeSlider();
  void onSpinValueChanged();
  void onUnitComboCurrentTextChanged(const QString&);

private:
  double toCurrent( double value );
  double fromCurrent( double value );

  QSlider *slider_;
  QDoubleSpinBox *spinbox_;
  QComboBox *unitcombo_;
  QString cur_unit_;
  UnitConv &unit_;
  bool spin_commanded_, slider_commanded_;
};

#endif

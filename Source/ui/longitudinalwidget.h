#ifndef LONGITUDINALWIDGET_H
#define LONGITUDINALWIDGET_H

#include <oscillation_widget.h>
#include <slider_spin.h>
#include <trajectory.h>

namespace Ui {
class LongitudinalWidget;
}

//-----------------------------------------------------------------------------
class LongitudinalWidget : public OscillationWidget {
  Q_OBJECT

public:
  explicit LongitudinalWidget(QWidget *parent = 0);
  ~LongitudinalWidget();
  virtual AbsTrajectoryPtr trajectory(double xangle, Vector3D rotate);
  virtual void draft(PositionVector&) {}

private slots:
  void on_longTrajectoryComboBox_currentTextChanged(const QString &arg1);

private:
  Ui::LongitudinalWidget *ui;
  SliderSpin *sbWeldSpeedSliderSpin, *fwSpeedSliderSpin, *fwLengthSliderSpin, *bwSpeedSliderSpin, *bwLengthSliderSpin;
};

#endif // LONGITUDINALWIDGET_H
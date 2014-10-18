#ifndef TRIANGULARSWITCHBACK_H
#define TRIANGULARSWITCHBACK_H

#include <oscillation_widget.h>
#include <slider_spin.h>
#include <trajectory.h>

namespace Ui {
class TriangularSwitchback;
}

//-----------------------------------------------------------------------------
class TriangularSwitchback : public OscillationWidget {
  Q_OBJECT

public:
  explicit TriangularSwitchback(QWidget *parent = 0);
  ~TriangularSwitchback();
  virtual AbsTrajectoryPtr trajectory(TrajectoryTransformPtr tt);
  virtual void draft(PositionVector&);
  
private:
  Ui::TriangularSwitchback *ui;
  SliderSpin *sbtSpeedSliderSpin, *sbtAmplSliderSpin, *sbtLenSliderSpin;
};

#endif // TRIANGULARSWITCHBACK_H

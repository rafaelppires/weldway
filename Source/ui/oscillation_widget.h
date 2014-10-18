#ifndef OSCILLATION_WIDGET_H
#define OSCILLATION_WIDGET_H

#include <QWidget>
#include <trajectory.h>
#include <boost/shared_ptr.hpp>

class OscillationWidget : public QWidget {
public:
  OscillationWidget(QWidget *parent) : QWidget(parent) {}
  virtual void draft(PositionVector&) = 0;
  virtual AbsTrajectoryPtr trajectory(TrajectoryTransformPtr) = 0;
};

#endif // OSCILLATION_WIDGET_H

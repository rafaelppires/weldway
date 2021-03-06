#ifndef TRANSFORMATIONWIDGET_H
#define TRANSFORMATIONWIDGET_H

#include <QWidget>
#include <trajectory_transform.h>
#include <boost/shared_ptr.hpp>

class TransformationWidget : public QWidget {
public:
  TransformationWidget(QWidget *parent) : QWidget(parent) {}
  virtual TrajectoryTransformPtr transformation() = 0;
  virtual Vector3I initPos() = 0;
  virtual Vector2D initTorch() = 0;
  virtual Vector3I finalPos() = 0;
  virtual Vector2D finalTorch() = 0;
};

#endif // TRANSFORMATIONWIDGET_H

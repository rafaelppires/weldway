#ifndef CIRCULARWIDGET_H
#define CIRCULARWIDGET_H

#include <QWidget>
#include <master_communicator.h>
#include <transformation_widget.h>

namespace Ui {
class CircularWidget;
}

class CircularWidget : public TransformationWidget {
  Q_OBJECT

public:
  explicit CircularWidget(MasterCommunicator &m, UnitConvPtr xconv, UnitConvPtr yconv, UnitConvPtr zconv, QWidget *parent=0);
  ~CircularWidget();
  virtual TrajectoryTransformPtr transformation();
  virtual Vector3I initPos();
  Vector3I centerPos();

private:
  Ui::CircularWidget *ui;
  MasterCommunicator &machine_;
  UnitConvPtr xconv_, yconv_, zconv_;
};

#endif // CIRCULARWIDGET_H

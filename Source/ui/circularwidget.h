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
  virtual Vector2D initTorch();
  virtual Vector3I finalPos();
  virtual Vector2D finalTorch();
  Vector3I centerPos();

private slots:
  void on_markInitPositionButton_clicked();

  void on_markCentralPositionButton_clicked();

private:
  Ui::CircularWidget *ui;
  MasterCommunicator &machine_;
  UnitConvPtr xconv_, yconv_, zconv_;
};

#endif // CIRCULARWIDGET_H

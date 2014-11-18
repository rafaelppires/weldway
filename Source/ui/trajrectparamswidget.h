#ifndef TRAJRECTPARAMSWIDGET_H
#define TRAJRECTPARAMSWIDGET_H

#include <QWidget>
#include <master_communicator.h>
#include <transformation_widget.h>

namespace Ui {
class TrajRectParamsWidget;
}

class TrajRectParamsWidget : public TransformationWidget {
  Q_OBJECT

public:
  explicit TrajRectParamsWidget(MasterCommunicator &, UnitConvPtr xconv, UnitConvPtr yconv, UnitConvPtr zconv, QWidget *parent=0);
  ~TrajRectParamsWidget();
  virtual Vector3I initPos();
  virtual Vector2D initTorch();
  virtual Vector3I finalPos();
  virtual Vector2D finalTorch() { return initTorch(); }
  virtual TrajectoryTransformPtr transformation();

  double xangle();
  double vangle();
  double hangle();

private slots:
  void on_invertDirectionButton_clicked();
  void on_markInitPositionButton_clicked();
  void on_markFinalPositionButton_clicked();

private:
  Ui::TrajRectParamsWidget *ui;
  MasterCommunicator &machine_;
  UnitConvPtr xconv_, yconv_, zconv_;
};

#endif // TRAJRECTPARAMSWIDGET_H

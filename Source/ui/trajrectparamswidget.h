#ifndef TRAJRECTPARAMSWIDGET_H
#define TRAJRECTPARAMSWIDGET_H

#include <QWidget>
#include <master_communicator.h>

namespace Ui {
class TrajRectParamsWidget;
}

class TrajRectParamsWidget : public QWidget
{
  Q_OBJECT

public:
  explicit TrajRectParamsWidget(MasterCommunicator &, QWidget *parent=0);
  ~TrajRectParamsWidget();
  Vector3I initPos(UnitConvPtr xconv, UnitConvPtr yconv, UnitConvPtr zconv);
  Vector3I finalPos(UnitConvPtr xconv, UnitConvPtr yconv, UnitConvPtr zconv);

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
};

#endif // TRAJRECTPARAMSWIDGET_H

#ifndef TRANSVERSALWIDGET_H
#define TRANSVERSALWIDGET_H

#include <oscillation_widget.h>
#include <slider_spin.h>
#include <trajectory.h>
#include <triangularswitchback.h>

namespace Ui {
class TransversalWidget;
}

//-----------------------------------------------------------------------------
class TransversalWidget : public OscillationWidget {
  Q_OBJECT

public:
  explicit TransversalWidget(QWidget *parent = 0);
  ~TransversalWidget();
  virtual AbsTrajectoryPtr trajectory(double xangle, Vector3D rotate);
  virtual void draft(PositionVector&);
  
signals:
  void updated();

private slots:
  void on_frequency_changed();
  void on_transvTrajectoryComboBox_currentTextChanged(const QString &arg1);

private:
  Ui::TransversalWidget *ui;
  SliderSpin *trSpeedSliderSpin, *trAmplSliderSpin, *trLmbdSliderSpin,  *eRhoSliderSpin;
  TriangularSwitchback *triswitch_panel_;
};

#endif // TRANSVERSALWIDGET_H

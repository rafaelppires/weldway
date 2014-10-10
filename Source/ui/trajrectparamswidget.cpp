#include "trajrectparamswidget.h"
#include "ui_trajrectparamswidget.h"

//-----------------------------------------------------------------------------
TrajRectParamsWidget::TrajRectParamsWidget(MasterCommunicator &m, QWidget *parent) :
  QWidget(parent), machine_(m),
  ui(new Ui::TrajRectParamsWidget) {
  ui->setupUi(this);
}

//-----------------------------------------------------------------------------
TrajRectParamsWidget::~TrajRectParamsWidget() {
  delete ui;
}

//-----------------------------------------------------------------------------
Vector3I TrajRectParamsWidget::initPos(UnitConvPtr xconv, UnitConvPtr yconv, UnitConvPtr zconv) {
  double xv = ui->xinitSpinBoxx->value(),
         yv = ui->yinitSpinBoxx->value(),
         zv = ui->zinitSpinBoxx->value();

  std::string cur_unit = ui->initFinalPosUnitComboBox->currentText().toStdString(),
              unit = "pulsos";

  return Vector3I( xconv->convertFromTo( xv, cur_unit, unit ),
                   yconv->convertFromTo( yv, cur_unit, unit ),
                   zconv->convertFromTo( zv, cur_unit, unit ) );
}

//-----------------------------------------------------------------------------
Vector3I TrajRectParamsWidget::finalPos(UnitConvPtr xconv, UnitConvPtr yconv, UnitConvPtr zconv) {
  double xv = ui->xfinalSpinBoxx->value(),
         yv = ui->yfinalSpinBoxx->value(),
         zv = ui->zfinalSpinBoxx->value();

  std::string cur_unit = ui->initFinalPosUnitComboBox->currentText().toStdString(),
              unit = "pulsos";

  return Vector3I( xconv->convertFromTo( xv, cur_unit, unit ),
                   yconv->convertFromTo( yv, cur_unit, unit ),
                   zconv->convertFromTo( zv, cur_unit, unit ) );
}

//-----------------------------------------------------------------------------
double TrajRectParamsWidget::xangle() {
 return ui->xangleSpinBoxx->value();
}

//-----------------------------------------------------------------------------
double TrajRectParamsWidget::vangle() {
 return ui->vAngleSpinBoxx->value();
}

//-----------------------------------------------------------------------------
double TrajRectParamsWidget::hangle() {
 return ui->hAngleSpinBoxx->value();
}

//-----------------------------------------------------------------------------
void TrajRectParamsWidget::on_invertDirectionButton_clicked() {
  double xt = ui->xinitSpinBoxx->value(),
         yt = ui->yinitSpinBoxx->value(),
         zt = ui->zinitSpinBoxx->value();
  ui->xinitSpinBoxx->setValue( ui->xfinalSpinBoxx->value() );
  ui->yinitSpinBoxx->setValue( ui->yfinalSpinBoxx->value() );
  ui->zinitSpinBoxx->setValue( ui->zfinalSpinBoxx->value() );
  ui->xfinalSpinBoxx->setValue( xt );
  ui->yfinalSpinBoxx->setValue( yt );
  ui->zfinalSpinBoxx->setValue( zt );
}

//-----------------------------------------------------------------------------
void TrajRectParamsWidget::on_markInitPositionButton_clicked() {
  Vector3D pos = machine_.currentPosition();
  ui->xinitSpinBoxx->setValue( pos.x() );
  ui->yinitSpinBoxx->setValue( pos.y() );
  ui->zinitSpinBoxx->setValue( pos.z() );
}

//-----------------------------------------------------------------------------
void TrajRectParamsWidget::on_markFinalPositionButton_clicked() {
  Vector3D pos = machine_.currentPosition();
  ui->xfinalSpinBoxx->setValue( pos.x() );
  ui->yfinalSpinBoxx->setValue( pos.y() );
  ui->zfinalSpinBoxx->setValue( pos.z() );
}

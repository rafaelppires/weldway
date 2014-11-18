#include "circularwidget.h"
#include "ui_circularwidget.h"
#include <circular_transform.h>
#include <geometry.h>

//-----------------------------------------------------------------------------
CircularWidget::CircularWidget(MasterCommunicator &m, UnitConvPtr xconv, UnitConvPtr yconv, UnitConvPtr zconv, QWidget *parent) :
  TransformationWidget(parent), machine_(m), xconv_(xconv), yconv_(yconv), zconv_(zconv),
  ui(new Ui::CircularWidget) {
  ui->setupUi(this);
}

//-----------------------------------------------------------------------------
CircularWidget::~CircularWidget() {
  delete ui;
}
//-----------------------------------------------------------------------------
Vector3I CircularWidget::initPos() {
  double xv = ui->xinitSpinBoxx->value(),
         yv = ui->yinitSpinBoxx->value(),
         zv = ui->zinitSpinBoxx->value();

  std::string cur_unit = ui->initFinalPosUnitComboBox->currentText().toStdString(),
              unit = "pulsos";

  return Vector3I( xconv_->convertFromTo( xv, cur_unit, unit ),
                   yconv_->convertFromTo( yv, cur_unit, unit ),
                   zconv_->convertFromTo( zv, cur_unit, unit ) );
}
//-----------------------------------------------------------------------------
Vector2D CircularWidget::initTorch() {
  return Vector2D( ui->hAngleSpinBoxx->value(), ui->vAngleSpinBoxx->value() );
}

//-----------------------------------------------------------------------------
Vector3I CircularWidget::finalPos() {
    return initPos(); // CHANGE
}

//-----------------------------------------------------------------------------
Vector2D CircularWidget::finalTorch() {
    return initTorch(); // CHANGE
}

//-----------------------------------------------------------------------------
Vector3I CircularWidget::centerPos() {
  double xv = ui->xcenterSpinBox->value(),
         yv = ui->ycenterSpinBox->value(),
         zv = ui->zcenterSpinBox->value();

  std::string cur_unit = ui->initFinalPosUnitComboBox->currentText().toStdString(),
              unit = "pulsos";

  return Vector3I( xconv_->convertFromTo( xv, cur_unit, unit ),
                   yconv_->convertFromTo( yv, cur_unit, unit ),
                   zconv_->convertFromTo( zv, cur_unit, unit ) );
}

//-----------------------------------------------------------------------------
TrajectoryTransformPtr CircularWidget::transformation() {
  Vector2D torch(deg2rad(ui->vAngleSpinBoxx->value()),deg2rad(ui->hAngleSpinBoxx->value()));
  double xangle = ui->xangleSpinBoxx->value(),
         range  = ui->rangeSpinBox->value();
  return TrajectoryTransformPtr( new CircularTransform(centerPos(),initPos(),deg2rad(xangle),deg2rad(range),torch,yconv_->convertFromTo(76,"mm","pulsos")) );
}

//-----------------------------------------------------------------------------
void CircularWidget::on_markInitPositionButton_clicked() {
  Vector3D pos = machine_.currentPosition();
  ui->xinitSpinBoxx->setValue( pos.x() );
  ui->yinitSpinBoxx->setValue( pos.y() );
  ui->zinitSpinBoxx->setValue( pos.z() );
}

//-----------------------------------------------------------------------------
void CircularWidget::on_markCentralPositionButton_clicked() {
  Vector3D pos = machine_.currentPosition();
  ui->xcenterSpinBox->setValue( pos.x() );
  ui->ycenterSpinBox->setValue( pos.y() );
  ui->zcenterSpinBox->setValue( pos.z() );
}

//-----------------------------------------------------------------------------

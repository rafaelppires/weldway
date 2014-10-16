#include "circularwidget.h"
#include "ui_circularwidget.h"

CircularWidget::CircularWidget(MasterCommunicator &m, QWidget *parent) :
  QWidget(parent), machine_(m),
  ui(new Ui::CircularWidget) {
  ui->setupUi(this);
}

CircularWidget::~CircularWidget()
{
  delete ui;
}

#ifndef CIRCULARWIDGET_H
#define CIRCULARWIDGET_H

#include <QWidget>
#include <master_communicator.h>

namespace Ui {
class CircularWidget;
}

class CircularWidget : public QWidget
{
  Q_OBJECT

public:
  explicit CircularWidget(MasterCommunicator &m, QWidget *parent=0);
  ~CircularWidget();

private:
  Ui::CircularWidget *ui;
  MasterCommunicator &machine_;
};

#endif // CIRCULARWIDGET_H

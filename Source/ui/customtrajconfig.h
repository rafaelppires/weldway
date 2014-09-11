#ifndef CUSTOMTRAJCONFIG_H
#define CUSTOMTRAJCONFIG_H

#include <QWidget>
#include <oscillation_widget.h>
#include <trajectoryscene.h>
#include <custom_trajectory.h>
#include <QListWidgetItem>
#include <QGraphicsEllipseItem>

namespace Ui {
class CustomTrajectoryWidget;
}

//-----------------------------------------------------------------------------
class CustomNode : public QGraphicsEllipseItem {
public:
  CustomNode(const Vector2D &p, const Vector2D &s, double r) : QGraphicsEllipseItem(p.x() * s.x()-r/2, p.y() * s.y()-r/2,r,r, 0) {
    setAcceptHoverEvents(true);
    setAcceptDrops(true);
  }

  void hoverEnterEvent ( QGraphicsSceneHoverEvent * event ) {
    QPen pen;
    pen.setColor( QColor("green") );
    pen.setWidth(4);
    setPen(pen);
    setBrush(QBrush("black"));
  }

  void hoverLeaveEvent( QGraphicsSceneHoverEvent * event ) {
    setPen(QPen());
    setBrush(QBrush());
  }

  void dragMoveEvent( QDragMoveEvent * event ) {
    setRect( 0, 0, 10, 10 );
  }
};

//-----------------------------------------------------------------------------
class CustomTrajectoryWidget : public OscillationWidget {
  Q_OBJECT

public:
  explicit CustomTrajectoryWidget(QWidget *parent, TrajectoryScene *);
  ~CustomTrajectoryWidget();

  virtual AbsTrajectoryPtr trajectory(double xangle, Vector3D rotate) { return AbsTrajectoryPtr(); }
  virtual void draft(PositionVector&) {}

public slots:
  void sceneClicked(Vector2D pt);

private slots:
 void updatePath(bool onlyscene = false);
 void on_newSaveButton_clicked();
 void on_editCancelButton_clicked();
 void on_addPtButton_clicked();
 void on_delPtButton_clicked();
 void on_pointListWidget_itemSelectionChanged();

private:
  void addPoint( const Vector2D &p, double, double);
  void newEntryAction();
  void newSaveAction();
  void newCancelAction();
  void editEntryAction();
  void editSaveAction();
  void editCancelAction();

  Ui::CustomTrajectoryWidget *ui;
  TrajectoryScene *scene_;
  Vector2D scale_;
  SegmentVector segments_;
  boost::shared_ptr<CustomTrajectory> editing_trajectory_;
  bool nonrec_;
};

#endif // CUSTOMTRAJCONFIG_H

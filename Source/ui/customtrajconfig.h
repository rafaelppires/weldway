#ifndef CUSTOMTRAJCONFIG_H
#define CUSTOMTRAJCONFIG_H

#include <QWidget>
#include <oscillation_widget.h>
#include <trajectoryscene.h>
#include <custom_trajectory.h>
#include <QListWidgetItem>
#include <QGraphicsEllipseItem>
#include <slider_spin.h>

namespace Ui {
class CustomTrajectoryWidget;
}

//-----------------------------------------------------------------------------
class CustomTrajectoryWidget : public OscillationWidget {
  Q_OBJECT

public:
  explicit CustomTrajectoryWidget(QWidget *parent, TrajectoryScene *);
  ~CustomTrajectoryWidget();

  virtual AbsTrajectoryPtr trajectory(double xangle, Vector3D rotate);
  virtual void draft(PositionVector&);

  void lineMoved(int idx, QPointF delta);

signals:
  void updated();

public slots:
  void sceneClicked(Vector2D pt);

private slots:
 void on_frequency_changed();
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
  CustomTrajectoryPtr editing_trajectory_;
  CustomTrajVector trajectory_list_;
  bool nonrec_;
  SliderSpin *trSpeedSliderSpin, *trAmplSliderSpin, *trLmbdSliderSpin;
};

//-----------------------------------------------------------------------------
class CustomEdge : public QGraphicsLineItem {
public:
  CustomEdge(CustomTrajectoryWidget *prnt, const Vector2D &p0, const Vector2D &p1, const Vector2D &s, const QPen &pen, int idx) :
    QGraphicsLineItem(p0.x()* s.x(), p0.y()* s.y(), p1.x()* s.x(), p1.y() * s.y(), 0), parent_(prnt), index_(idx) {
    setPen( pen );
    setAcceptHoverEvents(true);
    setFlag(ItemIsMovable);
    setFlag(ItemSendsScenePositionChanges);
  }

  void hoverEnterEvent ( QGraphicsSceneHoverEvent * event ) {
    prevpen_ = pen();
    QPen pen;
    pen.setColor( QColor("green") );
    pen.setWidth(4);
    setPen(pen);
  }

  void hoverLeaveEvent( QGraphicsSceneHoverEvent * event ) {
    setPen(prevpen_);
  }

  void nonMovable() {
    setFlag(ItemIsMovable, false);
    setFlag(ItemSendsScenePositionChanges, false);
  }

  QVariant itemChange( GraphicsItemChange change, const QVariant & value ) {
    switch (change) {
    case ItemPositionHasChanged:
        parent_->lineMoved(index_, pos());
        break;
    default:
        break;
    };
    return value;//QGraphicsItem::itemChange(change, value);
  }

  int index() { return index_; }

public:
  bool moved_;
private:
  int index_;
  QPen prevpen_;
  CustomTrajectoryWidget *parent_;
};

//-----------------------------------------------------------------------------
class CustomNode : public QGraphicsEllipseItem {
public:
  CustomNode(CustomTrajectoryWidget *prnt, const Vector2D &p, const Vector2D &s, double r, int i) :
    QGraphicsEllipseItem(p.x() * s.x()-r/2, p.y() * s.y()-r/2,r,r, 0),
    moved_(false), parent_(prnt), prev_(0), next_(0), index_(i), syncY_(0), sync_nonrec_(false), movePrev_(true), moveNext_(true) {
    setFlag(ItemIsMovable);
    setFlag(ItemSendsScenePositionChanges);
    setAcceptHoverEvents(true);
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

  QVariant itemChange( GraphicsItemChange change, const QVariant & value ) {
    switch (change) {
    case ItemPositionHasChanged:
    {
        moved_ = true;
        Vector2D cur(boundingRect().center().x()+pos().x(),
                     boundingRect().center().y()+pos().y());
        if( prev_ && movePrev_ ) {
          QLineF l = prev_->line();
          prev_->setLine( l.x1(), l.y1(), cur.x(), cur.y() );
        }
        if( next_ && moveNext_ ) {
          QLineF l = next_->line();
          next_->setLine( cur.x(), cur.y(), l.x2(), l.y2() );
        }

        if( sync_nonrec_ )
          sync_nonrec_ = false;
        else if( syncY_ ) {
          syncY_->syncNonRec();
          syncY_->setPos( syncY_->pos().x(), cur.y() );
        }
        movePrev_ = moveNext_ = true;
        break;
    }
    default:
        break;
    };
    return QGraphicsItem::itemChange(change, value);
  }

  void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
      update();
      QGraphicsItem::mouseReleaseEvent(event);
  }

  void movePrevOnly( QPointF d ) { movePrev_ = true; moveNext_ = false; setPos(d); }
  void moveNextOnly( QPointF d ) { movePrev_ = false; moveNext_ = true; setPos(d); }
  void syncY( CustomNode *s ) { syncY_ = s; }
  void syncNonRec() { sync_nonrec_ = true; }

  bool moved() { return moved_; }
  void movedReset() { moved_ = false; }
  void afterEdge(CustomEdge *e) { next_ = e; }
  void beforeEdge(CustomEdge *e) { prev_ = e; }
public:
  int index_;
  bool moved_;
  CustomTrajectoryWidget *parent_;
  CustomEdge *prev_, *next_;
  CustomNode *syncY_; bool sync_nonrec_, movePrev_, moveNext_;
};


#endif // CUSTOMTRAJCONFIG_H

#ifndef TRAJECTORYSCENE_H
#define TRAJECTORYSCENE_H

#include <QtWidgets/QGraphicsScene>
#include <vectorxd.h>

class TrajectoryScene : public QGraphicsScene {
  Q_OBJECT

public:
  TrajectoryScene();

signals:
  void sceneClicked(Vector2D point);

private:
  void drawBackground ( QPainter * painter, const QRectF & rect );
  void mouseReleaseEvent ( QGraphicsSceneMouseEvent * mouseEvent );
};

#endif // TRAJECTORYSCENE_H

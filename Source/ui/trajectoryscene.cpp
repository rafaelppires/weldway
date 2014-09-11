#include <trajectoryscene.h>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <cmath>
#include <iostream>
#include <vectorxd.h>

//-----------------------------------------------------------------------------
TrajectoryScene::TrajectoryScene() {
}

//-----------------------------------------------------------------------------
void TrajectoryScene::drawBackground( QPainter * painter, const QRectF & rect ) {
  painter->setPen( Qt::lightGray );
  double x = int(rect.x() / 40) * 40, x0 = x;
  for(; fabs(x-x0) < rect.width(); x += 40 )
    painter->drawLine( x, rect.y(), x, rect.y()+rect.height() );
  double y = int(rect.y() / 40) * 40, y0 = y;
  for(; fabs(y-y0) < rect.height(); y += 40 )
    painter->drawLine( rect.x(), y, rect.x()+rect.width(), y );
}

//-----------------------------------------------------------------------------
void TrajectoryScene::mouseReleaseEvent ( QGraphicsSceneMouseEvent* mouseEvent ) {
  Vector2D p(mouseEvent->scenePos().x(), mouseEvent->scenePos().y());
  emit sceneClicked(p);
}

//-----------------------------------------------------------------------------

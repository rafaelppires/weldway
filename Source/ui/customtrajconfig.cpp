#include "customtrajconfig.h"
#include "ui_customtrajconfig.h"

//-----------------------------------------------------------------------------
CustomTrajectoryWidget::CustomTrajectoryWidget(QWidget *parent, TrajectoryScene *scene) :
  OscillationWidget(parent),
  ui(new Ui::CustomTrajectoryWidget),
  scene_(scene),
  scale_(250,250), nonrec_(false) {
  ui->setupUi(this);
  ui->editWidget->setHidden(true);
}

//-----------------------------------------------------------------------------
CustomTrajectoryWidget::~CustomTrajectoryWidget() {
  delete ui;
}

//-----------------------------------------------------------------------------
void CustomTrajectoryWidget::sceneClicked(Vector2D pt) {
  if( isVisible() )
    scene_->addEllipse(pt.x(), pt.y(), 2, 2 );
}

//-----------------------------------------------------------------------------
void CustomTrajectoryWidget::on_newSaveButton_clicked() {
  if( ui->newSaveButton->text() == "Nova" ) {
    ui->editWidget->setHidden(false);
    ui->newSaveButton->setText("Salvar");
    ui->editCancelButton->setText("Cancelar");
    newEntryAction();
  } else {
    ui->editWidget->setHidden(true);
    ui->newSaveButton->setText("Nova");
    ui->editCancelButton->setText("Editar");
    if(ui->newSaveButton->text() == "Salvar") {
      newSaveAction();
    } else if(ui->newSaveButton->text() == "Cancelar") {
      editCancelAction();
    }
  }
}

//-----------------------------------------------------------------------------
void CustomTrajectoryWidget::on_editCancelButton_clicked() {
  if(ui->editCancelButton->text() == "Editar" ) {
    ui->newSaveButton->setText("Cancelar");
    ui->editCancelButton->setText("Salvar");
    editEntryAction();
    ui->editWidget->setHidden(false);
  } else {
    ui->editWidget->setHidden(true);
    ui->newSaveButton->setText("Nova");
    ui->editCancelButton->setText("Editar");
    if( ui->newSaveButton->text() == "Salvar" ) {
      editSaveAction();
    } else if( ui->editCancelButton->text() == "Cancelar" ) {
      newCancelAction();
    }
  }
}

//-----------------------------------------------------------------------------
void CustomTrajectoryWidget::newEntryAction() {
  editing_trajectory_.reset( new CustomTrajectory );
  addPoint( Vector2D(1,0), 1, -1 );
}

//-----------------------------------------------------------------------------
void CustomTrajectoryWidget::newSaveAction() {

}

//-----------------------------------------------------------------------------
void CustomTrajectoryWidget::newCancelAction() {

}

//-----------------------------------------------------------------------------
void CustomTrajectoryWidget::editEntryAction() {

}

//-----------------------------------------------------------------------------
void CustomTrajectoryWidget::editSaveAction() {

}

//-----------------------------------------------------------------------------
void CustomTrajectoryWidget::editCancelAction() {

}

//-----------------------------------------------------------------------------
void CustomTrajectoryWidget::addPoint( const Vector2D &p, double spd, double fnt) {
  if( ui->pointListWidget->count() == 0 ) {
    editing_trajectory_->addSegment( p, spd, fnt );
    updatePath();
  }
}

//-----------------------------------------------------------------------------
void CustomTrajectoryWidget::on_addPtButton_clicked() {
  int idx = ui->pointListWidget->currentRow();
  if( idx >= 0 ) editing_trajectory_->split( idx );
  updatePath();
}

//-----------------------------------------------------------------------------
void CustomTrajectoryWidget::on_delPtButton_clicked() {

}

//-----------------------------------------------------------------------------
void CustomTrajectoryWidget::updatePath( bool onlyscene ) {
  if( nonrec_ ) { nonrec_ = false; return; }
  if( !onlyscene ) {
    segments_ = editing_trajectory_->getSegments();
    ui->pointListWidget->clear();
  }
  SegmentVector::iterator it = segments_.begin(), end = segments_.end();
  scene_->clear();
  Vector2D p(0,0), prev = p;
  int r = 8, hr = r/2;
  scene_->addEllipse(p.x()-hr,p.y()-hr,r,r);
  int idx = ui->pointListWidget->currentRow();
  for(int i = 0; it!=end; ++it, ++i) {
    p += it->point;
    std::stringstream ss; ss << it->point;
    if( !onlyscene ) ui->pointListWidget->addItem( ss.str().c_str() );
    scene_->addItem( new CustomNode(p, scale_, r) );
    QPen pen;
    if( i == idx ) {
      pen.setColor( QColor("blue") );
      pen.setWidth(3);
    } else
      pen.setColor( QColor("black") );
    scene_->addLine( prev.x()* scale_.x(), prev.y()* scale_.y(), p.x()* scale_.x(), p.y() * scale_.y(), pen );
    prev = p;
  }
}

//-----------------------------------------------------------------------------
void CustomTrajectoryWidget::on_pointListWidget_itemSelectionChanged() {
  updatePath(true);
  int idx = ui->pointListWidget->currentRow();
  ui->xcoordSpinBox->setValue( segments_[idx].point.x() );
  ui->ycoordSpinBox->setValue( segments_[idx].point.y() );
}

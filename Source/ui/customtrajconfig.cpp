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

  trSpeedSliderSpin = new SliderSpin( this, ui->trWeldSpeedSlider, ui->trWeldSpeedSpinBox, ui->trWeldSpeedUnitComboBox, UnitConvPtr(new SpeedConv(0, 30)) );
  trAmplSliderSpin  = new SliderSpin( this, ui->trAmplitudeSlider, ui->trAmplitudeSpinBox, ui->trAmplitudeUnitComboBox, UnitConvPtr(new PositionConv(0, 50)) );
  trLmbdSliderSpin  = new SliderSpin( this, ui->trLambdaSlider,    ui->trLambdaSpinBox,    ui->trLambdaUnitComboBox,    UnitConvPtr(new PositionConv(1, 14)) );

  connect( trSpeedSliderSpin, SIGNAL(valueChanged()), this, SLOT(on_frequency_changed()) );
  connect( trLmbdSliderSpin,  SIGNAL(valueChanged()), this, SLOT(on_frequency_changed()) );
  connect( trAmplSliderSpin,  SIGNAL(valueChanged()), parent, SLOT(redraw()) );

  connect( this, SIGNAL(updated()), parent, SLOT(redraw()) );
}

//-----------------------------------------------------------------------------
CustomTrajectoryWidget::~CustomTrajectoryWidget() {
  delete ui;
}

//-----------------------------------------------------------------------------
void CustomTrajectoryWidget::on_frequency_changed() {
  double f = trSpeedSliderSpin->value("mm/s") / trLmbdSliderSpin->value("mm");
  char value[64];
  sprintf( value, "%.1f", f );
  ui->frLabel->setText( value );
  emit updated();
}

//-----------------------------------------------------------------------------
void CustomTrajectoryWidget::sceneClicked(Vector2D pt) {
  if( !isVisible() ) return;

  bool changed = false;
  foreach( QGraphicsItem *item, scene_->items() ) {
    if( CustomNode *node = qgraphicsitem_cast<CustomNode *>(item) ) {
      QRectF r = node->boundingRect();
      if( node->moved() ) {
        QPointF delta = node->pos();
        Vector2D d( delta.x()/scale_.x(), delta.y()/scale_.y() );
        if(editing_trajectory_) {
          Vector2D scale =
          node->index_ == segments_.size() ? editing_trajectory_->moveDestination( node->index_-1, d ) :
                                             editing_trajectory_->moveSource( node->index_, d );
          scale_.x() *= scale.x();
          scale_.y() *= scale.y();
        }
        std::cout << "moved node is now " << node->pos().x() << "," << node->pos().y() << " or " << r.center().x() << "," << r.center().y() << "\n";
        node->movedReset();
        changed = true;
      }
    }
  }
  if( changed ) updatePath();

  if( CustomEdge *edge = qgraphicsitem_cast<CustomEdge *>(scene_->itemAt(pt.x(),pt.y(),QTransform())) )
    ui->pointListWidget->setCurrentRow( edge->index() );
}

//-----------------------------------------------------------------------------
void CustomTrajectoryWidget::on_newSaveButton_clicked() {
  if( ui->newSaveButton->text() == "Nova" ) {
    ui->editWidget->setHidden(false);
    ui->paramsWidget->setHidden(true);
    ui->newSaveButton->setText("Salvar");
    ui->editCancelButton->setText("Cancelar");
    newEntryAction();
  } else {
    ui->editWidget->setHidden(true);
    ui->editCancelButton->setText("Editar");
    if(ui->newSaveButton->text() == "Salvar") {
      newSaveAction();
    } else if(ui->newSaveButton->text() == "Cancelar") {
      editCancelAction();
    }
    ui->newSaveButton->setText("Nova");
    ui->paramsWidget->setHidden(false);
  }
}

//-----------------------------------------------------------------------------
void CustomTrajectoryWidget::on_editCancelButton_clicked() {
  if(ui->editCancelButton->text() == "Editar" ) {
    ui->paramsWidget->setHidden(true);
    ui->newSaveButton->setText("Cancelar");
    ui->editCancelButton->setText("Salvar");
    editEntryAction();
    ui->editWidget->setHidden(false);
  } else {
    ui->editWidget->setHidden(true);
    ui->newSaveButton->setText("Nova");
    if( ui->editCancelButton->text() == "Salvar" ) {
      editSaveAction();
    } else if( ui->editCancelButton->text() == "Cancelar" ) {
      newCancelAction();
    }
    ui->editCancelButton->setText("Editar");
    ui->paramsWidget->setHidden(false);
  }
}

//-----------------------------------------------------------------------------
void CustomTrajectoryWidget::newEntryAction() {
  editing_trajectory_.reset( new CustomTrajectory );
  addPoint( Vector2D(1,0), 1, -1 );
}

//-----------------------------------------------------------------------------
void CustomTrajectoryWidget::newSaveAction() {
  if( !editing_trajectory_ ) return;
  editing_trajectory_->setName( ui->trajNameLineEdit->text().toStdString() );
  ui->trajListComboBox->addItem( editing_trajectory_->name().c_str() );
  trajectory_list_.push_back( editing_trajectory_ );
  clearEditing();
  emit updated();
}

//-----------------------------------------------------------------------------
void CustomTrajectoryWidget::newCancelAction() {
  clearEditing();
  emit updated();
}
//-----------------------------------------------------------------------------

void CustomTrajectoryWidget::clearEditing() {
  ui->trajNameLineEdit->clear();
  editing_trajectory_.reset();
  ui->pointListWidget->clear();
  scene_->clear();
}

//-----------------------------------------------------------------------------
void CustomTrajectoryWidget::editEntryAction() {
  int idx = ui->trajListComboBox->currentIndex();
  if( idx < 0 || idx >= trajectory_list_.size() ) return;

  ui->trajListComboBox->setEnabled( false );
  editing_trajectory_ = trajectory_list_[idx];
  segments_clone_ = editing_trajectory_->getSegments();
  ui->trajNameLineEdit->setText( editing_trajectory_->name().c_str() );
  updatePath();
}

//-----------------------------------------------------------------------------
void CustomTrajectoryWidget::editSaveAction() {
  if( !editing_trajectory_ ) return;
  editing_trajectory_->setName( ui->trajNameLineEdit->text().toStdString() );
  ui->trajListComboBox->setItemText( ui->trajListComboBox->currentIndex(), ui->trajNameLineEdit->text() );
  clearEditing();
  ui->trajListComboBox->setEnabled( true );
  emit updated();
}

//-----------------------------------------------------------------------------
void CustomTrajectoryWidget::editCancelAction() {
  editing_trajectory_->setSegments( segments_clone_ );
  clearEditing();
  ui->trajListComboBox->setEnabled( true );
  emit updated();
}

//-----------------------------------------------------------------------------
void CustomTrajectoryWidget::addPoint( const Vector2D &p, double spd, double fnt) {
  if( editing_trajectory_ && ui->pointListWidget->count() == 0 ) {
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
  int idx = ui->pointListWidget->currentRow();
  if( idx >= 0 ) editing_trajectory_->remove( idx );
  updatePath();
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
  int r = 10, lastidx = segments_.size()-1;

  int idx = ui->pointListWidget->currentRow();
  CustomNode *first = new CustomNode(this, p, scale_, r, 0), *last, *prevnode = first;
  scene_->addItem( first );

  for(int i = 0; it!=end; ++it, ++i) {
    p += it->point;
    std::stringstream ss; ss << Vector2D(it->point.x()*100,it->point.y()*-100);
    if( !onlyscene ) ui->pointListWidget->addItem( ss.str().c_str() );
    CustomNode *node = new CustomNode(this, p, scale_, r, i+1);
    if( i == lastidx ) last = node;
    scene_->addItem( node );
    QPen pen;
    if( i == idx ) {
      pen.setColor( QColor("blue") );
      pen.setWidth(3);
    } else
      pen.setColor( QColor("black") );
    CustomEdge *edge = new CustomEdge(this, prev, p, scale_, pen, i);
    if( i == 0 || i == lastidx ) edge->nonMovable();
    if( prevnode ) prevnode->afterEdge(edge);
    node->beforeEdge(edge);
    scene_->addItem( edge );
    prev = p;
    prevnode = node;
  }

  if( first && last ) {
    first->syncY( last );
    last->syncY( first );
  }
}
//-----------------------------------------------------------------------------
void CustomTrajectoryWidget::lineMoved( int idx, QPointF delta ) {
  foreach( QGraphicsItem *item, scene_->items() ) {
    if( CustomNode *node = qgraphicsitem_cast<CustomNode *>(item) ) {
      if( node->index_ == idx )   node->movePrevOnly( delta );
      if( node->index_ == idx+1 ) node->moveNextOnly( delta );
    }
  }
}

//-----------------------------------------------------------------------------
void CustomTrajectoryWidget::on_pointListWidget_itemSelectionChanged() {
  updatePath(true);
  int idx = ui->pointListWidget->currentRow();
  ui->xcoordSpinBox->setValue( segments_[idx].point.x()*100 );
  ui->ycoordSpinBox->setValue( segments_[idx].point.y()*-100 );
}
//-----------------------------------------------------------------------------
void CustomTrajectoryWidget::draft(PositionVector &v) {
  int idx = ui->trajListComboBox->currentIndex();
  if( ui->editCancelButton->text() != "Editar" ||
      ui->newSaveButton->text() != "Nova" || idx == -1 || idx < 0 || idx >= trajectory_list_.size() )
    return;
  CustomTrajectoryPtr trajectory = trajectory_list_[idx];

  std::string pos_unit("pulsos"), spd_unit("rpm");
  double lmbd = trLmbdSliderSpin->value( pos_unit ),
         spd  = trSpeedSliderSpin->value( spd_unit ),
         ampl = trAmplSliderSpin->value( pos_unit );
  trajectory->draft(v, spd, lmbd, ampl);
}
//-----------------------------------------------------------------------------
AbsTrajectoryPtr CustomTrajectoryWidget::trajectory(double xangle, Vector3D rotate) {
  int idx = ui->trajListComboBox->currentIndex();
  if( ui->editCancelButton->text() != "Editar" ||
      ui->newSaveButton->text() != "Nova" || idx == -1 || idx < 0 || idx >= trajectory_list_.size() )
    return AbsTrajectoryPtr();
  CustomTrajectoryPtr trajectory = trajectory_list_[idx];

  std::string pos_unit("pulsos"), spd_unit("rpm");
  double lmbd = trLmbdSliderSpin->value( pos_unit ),
         spd  = trSpeedSliderSpin->value( spd_unit ),
         ampl = trAmplSliderSpin->value( pos_unit );
  return trajectory->getExecutable( rotate, xangle, spd, lmbd, ampl );
}

//-----------------------------------------------------------------------------
void CustomTrajectoryWidget::on_trajListComboBox_currentIndexChanged(int index) {
  emit updated();
}

//-----------------------------------------------------------------------------

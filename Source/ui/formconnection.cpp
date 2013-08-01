#include "formconnection.h"
#include "ui_formconnection.h"
#include <iostream>
#include <master_communicator.h>

//-----------------------------------------------------------------------------
FormConnection::FormConnection(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormConnection) {
  ui->setupUi(this);

  connect( ui->buttonBox, SIGNAL(accepted()), SLOT(onAccept()) );

  devlist_ = ParallelPort::list();
  ParallelPort::ParallelList::iterator it = devlist_.begin(),
                                       end = devlist_.end();

  for(int i = 0; it != end; ++it, ++i) {
    ui->parallelPortComboBox->addItem( QString( it->first.c_str() ) );
  }

  if( devlist_.size() )
    ui->parallelRadioButton->setChecked( true );
  else
    ui->parallelRadioButton->setEnabled( false );
}
//-----------------------------------------------------------------------------
void FormConnection::onAccept() {
  if( ui->parallelRadioButton->isChecked() ) {
    int idx = ui->parallelPortComboBox->currentIndex();
    if( idx < 0 || idx >= devlist_.size() ) return;
    MasterCommunicator::getInstance().setupParallelPort( devlist_[idx].second );
  }
}

//-----------------------------------------------------------------------------
FormConnection::~FormConnection() {
    delete ui;
}
//-----------------------------------------------------------------------------

#include "formconnection.h"
#include "ui_formconnection.h"
#include <iostream>
FormConnection::FormConnection(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormConnection) {
  ParallelPort::ParallelList devlist_ = ParallelPort::list();
  ParallelPort::ParallelList::iterator it = devlist_.begin(),
                                       end = devlist_.end();
  std::cout << devlist_.size() << "\n";
  for(; it != end; ++it)
    ui->parallelPortComboBox->addItem( QString( it->first.c_str() ) );

  ui->setupUi(this);
}

FormConnection::~FormConnection() {
    delete ui;
}

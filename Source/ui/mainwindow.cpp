#include "mainwindow.h"
#include "ui_mainwindow.h"

//-----------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
    smotion = new SimpleMotion( "TTL232R" );
}

//-----------------------------------------------------------------------------
MainWindow::~MainWindow() {
    delete ui;
}

//-----------------------------------------------------------------------------
void MainWindow::on_findZeroPushButton_clicked() {
    //ui->findZeroPushButton->setText( "Whazup!" );
    smotion->startHoming();
}

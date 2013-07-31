#ifndef FORMCONNECTION_H
#define FORMCONNECTION_H

#include <QDialog>
#include <parallel.h>

namespace Ui {
class FormConnection;
}

class FormConnection : public QDialog
{
    Q_OBJECT
    
public:
    explicit FormConnection(QWidget *parent = 0);
    ~FormConnection();
    
private:
    ParallelPort::ParallelList devlist_;
    Ui::FormConnection *ui;
};

#endif // FORMCONNECTION_H

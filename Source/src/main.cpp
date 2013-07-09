
#include <windows.h>
#include <mainwindow.h>
#include <QApplication>

int main(int argc, char *argv[]) {
    SetPriorityClass ( GetCurrentProcess(), REALTIME_PRIORITY_CLASS );
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

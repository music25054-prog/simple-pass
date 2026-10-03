#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    MainWindow w;
    if (!w.isInitialized()) {
        return 0; // Корректное завершение, если окно входа закрыли или отменили
    }

    w.show();
    return a.exec();
}

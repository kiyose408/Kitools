#include <QApplication>
#include "core/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("PC Productivity Toolbox");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Kitools");

    MainWindow mainWindow;
    mainWindow.setWindowTitle("PC效率工具箱");
    mainWindow.setMinimumSize(400, 500);
    mainWindow.show();

    return app.exec();
}

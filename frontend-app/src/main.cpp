// -*- coding: utf-8 -*-
#include <QApplication>
#include <QFile>
#include <QFontDatabase>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("GameConsole");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("GameConsole");

    QFile styleFile(":/styles/theme.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        app.setStyleSheet(QString::fromUtf8(styleFile.readAll()));
        styleFile.close();
    }

    MainWindow window;
    window.setMinimumSize(1000, 700);
    window.resize(1200, 800);
    window.setWindowTitle("游戏掌机");
    window.show();

    return app.exec();
}

// main.cpp
#include <QApplication>
#include "startupdialog.h"
#include "fileopener.h"
#include "thememanager.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    loadSavedTheme();

    StartupDialog dialog;
    if (dialog.exec() != QDialog::Accepted)
        return 0;

    openDixelFile(dialog.selectedPath());

    return QApplication::exec();
}
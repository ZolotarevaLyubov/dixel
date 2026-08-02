// fileopener.cpp
#include "fileopener.h"
#include "mainwindow.h"
#include "tablewindow.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QWidget>

QString detectFileType(const QString &path) {
    QFile file(path);
    if (!file.exists() || !file.open(QIODevice::ReadOnly | QIODevice::Text))
        return "text";

    QByteArray raw = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(raw);
    if (doc.isObject()) {
        QJsonObject obj = doc.object();
        if (obj.contains("type"))
            return obj["type"].toString();
    }
    return "text";
}

void openDixelFile(const QString &path, QWidget *windowToClose) {
    QString type = detectFileType(path);

    if (type == "table") {
        auto *tableWindow = new TableWindow(path);
        tableWindow->show();
    } else {
        auto *mainWindow = new MainWindow(path);
        mainWindow->show();
    }

    if (windowToClose) {
        windowToClose->close();
    }
}
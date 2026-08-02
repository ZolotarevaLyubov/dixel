// fileopener.h
#ifndef DIXEL_FILEOPENER_H
#define DIXEL_FILEOPENER_H

#include <QString>

class QWidget;

QString detectFileType(const QString &path);
void openDixelFile(const QString &path, QWidget *windowToClose = nullptr);

#endif //DIXEL_FILEOPENER_H
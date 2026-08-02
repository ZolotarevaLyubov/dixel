// startupdialog.cpp
#include "startupdialog.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QFileDialog>


StartupDialog::StartupDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Dixel");
    auto *newBtn = new QPushButton("Новый файл");
    auto *openBtn = new QPushButton("Открыть из папки");
    auto *layout = new QVBoxLayout(this);
    layout->addWidget(newBtn);
    layout->addWidget(openBtn);

    connect(newBtn, &QPushButton::clicked, this, &StartupDialog::onNewFile);
    connect(openBtn, &QPushButton::clicked, this, &StartupDialog::onOpenFile);
}

void StartupDialog::onNewFile() {
    m_path = QFileDialog::getSaveFileName(this, "Создать файл", "", "Dixel files (*.dixel)");
    if (!m_path.isEmpty()) accept();
}

void StartupDialog::onOpenFile() {
    m_path = QFileDialog::getOpenFileName(this, "Открыть файл", "", "Dixel files (*.dixel)");
    if (!m_path.isEmpty()) accept();
}
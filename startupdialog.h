// startupdialog.h
#pragma once
#include <QDialog>

class StartupDialog : public QDialog {
    Q_OBJECT
public:
    explicit StartupDialog(QWidget *parent = nullptr);
    QString selectedPath() const { return m_path; }

private slots:
    void onNewFile();
    void onOpenFile();

private:
    QString m_path;
};
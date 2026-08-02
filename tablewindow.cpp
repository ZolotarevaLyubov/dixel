// tablewindow.cpp
#include "tablewindow.h"
#include <QTableWidget>
#include <QHeaderView>
#include <QToolBar>
#include <QAction>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include "fileopener.h"
#include <QMenuBar>
#include <QFileDialog>
#include <QCloseEvent>

TableWindow::TableWindow(const QString &filePath, QWidget *parent)
    : QMainWindow(parent), m_filePath(filePath) {
    setWindowTitle("Таблица — " + filePath);
    resize(1000, 700);

    m_table = new QTableWidget(ROW_COUNT, COL_COUNT, this);
    setCentralWidget(m_table);

    m_table->setHorizontalHeaderLabels(generateColumnLabels(COL_COUNT));
    m_table->horizontalHeader()->setDefaultSectionSize(80);
    m_table->verticalHeader()->setDefaultSectionSize(24);

    m_table->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_table->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);

    connect(m_table, &QTableWidget::itemChanged, this, &TableWindow::onCellChanged);

    //auto *toolbar = addToolBar("Таблица");
    //auto *saveAction = toolbar->addAction("Сохранить");
    connect(m_table, &QTableWidget::itemChanged, this, &TableWindow::onCellChanged);

    setupMenu();
    loadFile();
}

void TableWindow::onCellChanged() {
    m_modified = true;
}

void TableWindow::closeEvent(QCloseEvent *event) {
    if (m_modified) {
        auto reply = QMessageBox::question(this, "Несохранённые изменения",
            "В таблице есть несохранённые изменения. Сохранить перед закрытием?",
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (reply == QMessageBox::Save) {
            saveFile();
            event->accept();
        } else if (reply == QMessageBox::Discard) {
            event->accept();
        } else {
            event->ignore();
        }
    } else {
        event->accept();
    }
}

QStringList TableWindow::generateColumnLabels(int count) {
    QStringList labels;
    for (int i = 0; i < count; ++i) {
        labels << QChar('A' + i);
    }
    return labels;
}

void TableWindow::saveFile() {
    m_table->setFocus();

    QJsonObject root;
    root["type"] = "table";

    QJsonArray rows;
    for (int r = 0; r < m_table->rowCount(); ++r) {
        QJsonArray row;
        for (int c = 0; c < m_table->columnCount(); ++c) {
            QTableWidgetItem *item = m_table->item(r, c);
            row.append(item ? item->text() : "");
        }
        rows.append(row);
    }
    root["data"] = rows;

    QJsonDocument doc(root);

    QFile file(m_filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.write(doc.toJson());
        file.close();
        m_modified = false;
        QMessageBox::information(this, "Сохранено", "Таблица сохранена");
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить файл");
    }
}

void TableWindow::loadFile() {
    QFile file(m_filePath);
    if (!file.exists() || !file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QByteArray raw = file.readAll();
    file.close();

    m_modified = false;

    QJsonDocument doc = QJsonDocument::fromJson(raw);
    if (!doc.isObject()) return;

    QJsonObject root = doc.object();
    if (root["type"].toString() != "table") return;

    QJsonArray rows = root["data"].toArray();
    for (int r = 0; r < rows.size() && r < m_table->rowCount(); ++r) {
        QJsonArray row = rows[r].toArray();
        for (int c = 0; c < row.size() && c < m_table->columnCount(); ++c) {
            QString value = row[c].toString();
            if (!value.isEmpty()) {
                m_table->setItem(r, c, new QTableWidgetItem(value));
            }
        }
    }
}

void TableWindow::setupMenu() {
    auto *fileMenu = menuBar()->addMenu("Файл");

    auto *newAction = fileMenu->addAction("Новый файл");
    connect(newAction, &QAction::triggered, this, &TableWindow::newFile);

    auto *openAction = fileMenu->addAction("Открыть файл");
    connect(openAction, &QAction::triggered, this, &TableWindow::openFile);

    auto *saveAction = fileMenu->addAction("Сохранить");
    connect(saveAction, &QAction::triggered, this, &TableWindow::saveFile);
}

void TableWindow::newFile() {
    QString path = QFileDialog::getSaveFileName(this, "Создать файл", "", "Dixel files (*.dixel)");
    if (!path.isEmpty()) {
        openDixelFile(path, this);
    }
}

void TableWindow::openFile() {
    QString path = QFileDialog::getOpenFileName(this, "Открыть файл", "", "Dixel files (*.dixel)");
    if (!path.isEmpty()) {
        openDixelFile(path, this);
    }
}
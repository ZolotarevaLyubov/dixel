// tablewindow.h
#ifndef DIXEL_TABLEWINDOW_H
#define DIXEL_TABLEWINDOW_H

#include <QMainWindow>

class QTableWidget;

class TableWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit TableWindow(const QString &filePath, QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void saveFile();
    void newFile();
    void openFile();
    void onCellChanged();

private:
    void loadFile();
    QStringList generateColumnLabels(int count);
    void setupMenu();
    bool m_modified = false;

    QTableWidget *m_table;
    QString m_filePath;
    static const int ROW_COUNT = 50;
    static const int COL_COUNT = 26;

    //QStringList generateColumnLabels(int count);
};

#endif //DIXEL_TABLEWINDOW_H
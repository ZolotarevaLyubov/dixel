// mainwindow.h
#ifndef DIXEL_MAINWINDOW_H
#define DIXEL_MAINWINDOW_H

#include <QMainWindow>


class QFontComboBox;
class QComboBox;
class QScrollArea;
class QTextDocument;
class PagedTextWidget;
class QLabel;
class MinimapWidget;
class ZoomWidget;


class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(const QString &filePath, QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void exportToPdf();
    void createTable();
    void toggleBold();
    void toggleItalic();
    void toggleUnderline();
    void changeFontFamily(const QFont &font);
    void changeFontSize(const QString &size);
    void chooseColor();
    void saveFile();
    void newFile();
    void openFile();
    void setLightTheme();
    void setDarkTheme();
    void alignLeft();
    void alignCenter();
    void alignRight();
    void changeLineSpacing(int index);
    void toggleParagraphSpacing(int index);
    void updatePageCountLabel(int count);
    void scrollToPage(int pageIndex);
    void zoomIn();
    void zoomOut();
    void applyListStyle(int index);

private:
    void setupToolbar();
    void setupMenu();
    void loadFile();

    PagedTextWidget *m_pagedWidget;
    QTextDocument *m_document;
    QScrollArea *m_scrollArea;
    QFontComboBox *m_fontCombo;
    QComboBox *m_sizeCombo;
    QComboBox *m_spacingCombo;
    QString m_filePath;
    //QIcon createNoSpacingIcon();
    QComboBox *m_paragraphSpacingCombo;
    QLabel *m_pageCountLabel;
    ZoomWidget *m_zoomWidget;
    QComboBox *m_listCombo;

    MinimapWidget *m_minimap;

    static const int PAGE_WIDTH = 794;
    static const int PAGE_HEIGHT = 1123;
};

#endif //DIXEL_MAINWINDOW_H
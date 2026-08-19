// mainwindow.cpp
#include "mainwindow.h"
#include "tablewindow.h"
#include "fileopener.h"
#include "thememanager.h"
#include "pagedtextwidget.h"

#include <QToolBar>
#include <QMenuBar>
#include <QFontComboBox>
#include <QComboBox>
#include <QAction>
#include <QActionGroup>
#include <QColorDialog>
#include <QTextCharFormat>
#include <QTextBlockFormat>
#include <QPrinter>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QFile>
#include <QTextStream>
#include <QTextDocument>
#include <QScrollArea>
#include <QHBoxLayout>
#include <QPainter>
#include <QLabel>
#include <QStatusBar>
#include "minimapwidget.h"
#include <QScrollBar>
#include "zoomwidget.h"
#include <QMouseEvent>
#include <QEvent>
#include <QCursor>
#include <QFontMetrics>
#include <QFontDatabase>
#include <QTextListFormat>
#include <QInputDialog>
#include "notepopup.h"

MainWindow::MainWindow(const QString &filePath, QWidget *parent)
    : QMainWindow(parent), m_filePath(filePath) {
    setWindowTitle(filePath);
    resize(900, 750);

    m_document = new QTextDocument(this);
    m_document->setDocumentMargin(60);

    m_pagedWidget = new PagedTextWidget(m_document, this);
    connect(m_pagedWidget, &PagedTextWidget::saveRequested, this, &MainWindow::saveFile);
    connect(m_pagedWidget, &PagedTextWidget::noteMarkerInserted, this, &MainWindow::onNoteMarkerInserted);
    connect(m_pagedWidget, &PagedTextWidget::noteHovered, this, &MainWindow::onNoteHovered);
    connect(m_pagedWidget, &PagedTextWidget::noteHoverEnded, this, &MainWindow::onNoteHoverEnded);
    connect(m_pagedWidget, &PagedTextWidget::noteClicked, this, &MainWindow::onNoteClicked);
    m_notePopup = new NotePopup(this);
    m_notePopup->hide();

    connect(m_notePopup, &NotePopup::textChanged, this, [this](const QString &text) {
    if (m_activeNoteIndex >= 0) {
        m_pagedWidget->setNoteText(m_activeNoteIndex, text);
        m_document->setModified(true);
    }
});

    auto *container = new QWidget();
    auto *containerLayout = new QHBoxLayout(container);
    containerLayout->addStretch();
    containerLayout->addWidget(m_pagedWidget);
    containerLayout->addStretch();
    containerLayout->setContentsMargins(0, 20, 0, 20);
    container->setObjectName("pageContainer");

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidget(container);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setObjectName("pageScrollArea");
    m_scrollArea->setStyleSheet("QScrollArea#pageScrollArea { border: none; }");

    m_zoomWidget = new ZoomWidget(m_scrollArea->viewport());
    m_zoomWidget->hide();
    connect(m_zoomWidget, &ZoomWidget::zoomInRequested, this, &MainWindow::zoomIn);
    connect(m_zoomWidget, &ZoomWidget::zoomOutRequested, this, &MainWindow::zoomOut);

    qApp->installEventFilter(this);

    m_minimap = new MinimapWidget(m_document, this);
    connect(m_minimap, &MinimapWidget::pageClicked, this, &MainWindow::scrollToPage);

    auto *centralContainer = new QWidget(this);
    auto *centralLayout = new QHBoxLayout(centralContainer);
    centralLayout->addWidget(m_scrollArea, 1);
    centralLayout->addWidget(m_minimap);
    centralLayout->setContentsMargins(0, 0, 0, 0);
    centralLayout->setSpacing(0);

    setCentralWidget(centralContainer);

    setupToolbar();
    setupMenu();
    loadFile();

    m_pageCountLabel = new QLabel(this);
    statusBar()->addPermanentWidget(m_pageCountLabel);
    connect(m_pagedWidget, &PagedTextWidget::pageCountChanged, this, &MainWindow::updatePageCountLabel);
    updatePageCountLabel(m_pagedWidget->pageCount());
}

void MainWindow::setupToolbar() {
    auto *toolbar = addToolBar("Форматирование");
    toolbar->setIconSize(QSize(20, 20));

    m_fontCombo = new QFontComboBox(this);
    toolbar->addWidget(m_fontCombo);
    connect(m_fontCombo, &QFontComboBox::currentFontChanged, this, &MainWindow::changeFontFamily);

    m_sizeCombo = new QComboBox(this);
    m_sizeCombo->setEditable(true);
    const QList<int> sizes = {8, 9, 10, 11, 12, 14, 16, 18, 20, 24, 28, 32, 36, 48};
    for (int s : sizes) m_sizeCombo->addItem(QString::number(s));
    m_sizeCombo->setCurrentText("12");
    toolbar->addWidget(m_sizeCombo);
    connect(m_sizeCombo, &QComboBox::currentTextChanged, this, &MainWindow::changeFontSize);

    auto *boldAction = toolbar->addAction("Ж");
    boldAction->setCheckable(true);
    connect(boldAction, &QAction::triggered, this, &MainWindow::toggleBold);

    auto *italicAction = toolbar->addAction("К");
    italicAction->setCheckable(true);
    connect(italicAction, &QAction::triggered, this, &MainWindow::toggleItalic);

    auto *underlineAction = toolbar->addAction("Ч");
    underlineAction->setCheckable(true);
    connect(underlineAction, &QAction::triggered, this, &MainWindow::toggleUnderline);

    toolbar->addSeparator();

    auto *colorAction = toolbar->addAction("Цвет");
    connect(colorAction, &QAction::triggered, this, &MainWindow::chooseColor);

    toolbar->addSeparator();

    auto *alignGroup = new QActionGroup(this);

    auto *alignLeftAction = toolbar->addAction("⯇");
    alignLeftAction->setCheckable(true);
    alignLeftAction->setChecked(true);
    alignGroup->addAction(alignLeftAction);
    connect(alignLeftAction, &QAction::triggered, this, &MainWindow::alignLeft);

    auto *alignCenterAction = toolbar->addAction("☰");
    alignCenterAction->setCheckable(true);
    alignGroup->addAction(alignCenterAction);
    connect(alignCenterAction, &QAction::triggered, this, &MainWindow::alignCenter);

    auto *alignRightAction = toolbar->addAction("⯈");
    alignRightAction->setCheckable(true);
    alignGroup->addAction(alignRightAction);
    connect(alignRightAction, &QAction::triggered, this, &MainWindow::alignRight);

    toolbar->addSeparator();

    m_spacingCombo = new QComboBox(this);
    const QList<QPair<QString, int>> spacingOptions = {
        {"Плотный", 80},
        {"Обычный", 100},
        {"Полуторный", 150},
        {"Двойной", 200}
    };
    for (const auto &opt : spacingOptions) {
        m_spacingCombo->addItem(opt.first, opt.second);
    }
    m_spacingCombo->setCurrentIndex(1);
    toolbar->addWidget(m_spacingCombo);
    connect(m_spacingCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::changeLineSpacing);

    m_paragraphSpacingCombo = new QComboBox(this);
    m_paragraphSpacingCombo->addItem("С отступом", 10);
    m_paragraphSpacingCombo->addItem("Без отступа", 0);
    m_paragraphSpacingCombo->setCurrentIndex(0);
    toolbar->addWidget(m_paragraphSpacingCombo);
    connect(m_paragraphSpacingCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::toggleParagraphSpacing);

    toolbar->addSeparator();

    m_listCombo = new QComboBox(this);
    m_listCombo->addItem("Нумерация: без списка", -1);
    m_listCombo->addItem("Нумерация: • Маркеры", QTextListFormat::ListDisc);
    m_listCombo->addItem("Нумерация: ○ Кружки", QTextListFormat::ListCircle);
    m_listCombo->addItem("Нумерация: 1. Цифры", QTextListFormat::ListDecimal);
    m_listCombo->addItem("Нумерация: a. Буквы", QTextListFormat::ListLowerAlpha);
    m_listCombo->addItem("Нумерация: i. Римские", QTextListFormat::ListLowerRoman);
    toolbar->addWidget(m_listCombo);
    connect(m_listCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::applyListStyle);
}

void MainWindow::setupMenu() {
    auto *fileMenu = menuBar()->addMenu("Файл");

    auto *newAction = fileMenu->addAction("Новый файл");
    connect(newAction, &QAction::triggered, this, &MainWindow::newFile);

    auto *openAction = fileMenu->addAction("Открыть файл");
    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);

    fileMenu->addSeparator();

    auto *saveAction = fileMenu->addAction("Сохранить");
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveFile);

    auto *pdfAction = fileMenu->addAction("Экспорт в PDF");
    connect(pdfAction, &QAction::triggered, this, &MainWindow::exportToPdf);

    fileMenu->addSeparator();

    auto *tableAction = fileMenu->addAction("Перейти в таблицу");
    connect(tableAction, &QAction::triggered, this, &MainWindow::createTable);

    auto *viewMenu = menuBar()->addMenu("Вид");
    auto *themeMenu = viewMenu->addMenu("Тема");

    auto *lightAction = themeMenu->addAction("Дневная");
    connect(lightAction, &QAction::triggered, this, &MainWindow::setLightTheme);

    auto *darkAction = themeMenu->addAction("Ночная");
    connect(darkAction, &QAction::triggered, this, &MainWindow::setDarkTheme);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (m_document->isModified()) {
        auto reply = QMessageBox::question(this, "Несохранённые изменения",
            "В файле есть несохранённые изменения. Сохранить перед закрытием?",
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

void MainWindow::loadFile() {
    QFile file(m_filePath);
    if (file.exists() && file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        in.setEncoding(QStringConverter::Utf8);
        QString content = in.readAll();
        file.close();

        QStringList parts = content.split("\n<!--DIXEL_NOTES_SEPARATOR-->\n");

        m_document->setHtml(parts[0]);

        if (parts.size() > 1 && !parts[1].isEmpty()) {
            QStringList notes = parts[1].split("<!--DIXEL_NOTE_SEPARATOR-->");
            m_pagedWidget->loadNotes(notes);
        }

        m_document->setModified(false);
    }
}

void MainWindow::saveFile() {
    QFile file(m_filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out.setEncoding(QStringConverter::Utf8);

        out << m_document->toHtml();
        out << "\n<!--DIXEL_NOTES_SEPARATOR-->\n";
        out << m_pagedWidget->notesTexts().join("<!--DIXEL_NOTE_SEPARATOR-->");

        file.close();
        m_document->setModified(false);
        QMessageBox::information(this, "Сохранено", "Файл сохранён");
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить файл");
    }
}

void MainWindow::newFile() {
    QString path = QFileDialog::getSaveFileName(this, "Создать файл", "", "Dixel files (*.dixel)");
    if (!path.isEmpty()) {
        openDixelFile(path, this);
    }
}

void MainWindow::openFile() {
    QString path = QFileDialog::getOpenFileName(this, "Открыть файл", "", "Dixel files (*.dixel)");
    if (!path.isEmpty()) {
        openDixelFile(path, this);
    }
}

void MainWindow::toggleBold() {
    QTextCharFormat fmt;
    fmt.setFontWeight(m_pagedWidget->fontWeight() == QFont::Bold ? QFont::Normal : QFont::Bold);
    m_pagedWidget->mergeCurrentCharFormat(fmt);
    m_pagedWidget->setFocus();
}

void MainWindow::toggleItalic() {
    QTextCharFormat fmt;
    fmt.setFontItalic(!m_pagedWidget->fontItalic());
    m_pagedWidget->mergeCurrentCharFormat(fmt);
    m_pagedWidget->setFocus();
}

void MainWindow::toggleUnderline() {
    QTextCharFormat fmt;
    fmt.setFontUnderline(!m_pagedWidget->fontUnderline());
    m_pagedWidget->mergeCurrentCharFormat(fmt);
    m_pagedWidget->setFocus();
}

void MainWindow::changeFontFamily(const QFont &font) {
    QFontDatabase fontDb;
    QList<QFontDatabase::WritingSystem> systems = fontDb.writingSystems(font.family());
    bool supportsCyrillic = systems.contains(QFontDatabase::Cyrillic);

    qDebug() << "Font:" << font.family() << "writing systems:" << systems << "supports Cyrillic:" << supportsCyrillic;

    if (!supportsCyrillic) {
        QMessageBox::warning(this, "Шрифт не поддерживает кириллицу",
            QString("Шрифт \"%1\" не содержит русских букв. "
                    "Текст на русском языке будет отображаться другим шрифтом.")
            .arg(font.family()));
    }

    m_pagedWidget->setCurrentFont(font);
    m_pagedWidget->setFocus();
}


void MainWindow::changeFontSize(const QString &size) {
    bool ok;
    int pointSize = size.toInt(&ok);
    if (ok && pointSize > 0) {
        m_pagedWidget->setFontPointSize(pointSize);
    }
    m_pagedWidget->setFocus();
}

void MainWindow::chooseColor() {
    QColor color = QColorDialog::getColor(Qt::black, this, "Выбор цвета текста");
    if (color.isValid()) {
        QTextCharFormat fmt;
        fmt.setForeground(color);
        m_pagedWidget->mergeCurrentCharFormat(fmt);
        m_pagedWidget->setFocus();
    }
}

void MainWindow::alignLeft() {
    m_pagedWidget->setAlignment(Qt::AlignLeft);
    m_pagedWidget->setFocus();
}

void MainWindow::alignCenter() {
    m_pagedWidget->setAlignment(Qt::AlignCenter);
    m_pagedWidget->setFocus();
}

void MainWindow::alignRight() {
    m_pagedWidget->setAlignment(Qt::AlignRight);
    m_pagedWidget->setFocus();
}

void MainWindow::changeLineSpacing(int index) {
    int percentage = m_spacingCombo->itemData(index).toInt();
    QTextBlockFormat blockFmt;
    blockFmt.setLineHeight(percentage, QTextBlockFormat::ProportionalHeight);
    m_pagedWidget->mergeCurrentBlockFormat(blockFmt);
    m_pagedWidget->setFocus();
}

void MainWindow::exportToPdf() {
    QString path = QFileDialog::getSaveFileName(this, "Сохранить как PDF", "", "PDF files (*.pdf)");
    if (path.isEmpty()) return;

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(path);

    m_document->print(&printer);

    QMessageBox::information(this, "Готово", "Файл сохранён в PDF");
}

void MainWindow::createTable() {
    if (!m_pagedWidget->toPlainText().isEmpty()) {
        QMessageBox::information(this, "Новый файл нужен",
            "В этом файле уже есть текст. Чтобы создать таблицу, выбери новый файл.");

        QString path = QFileDialog::getSaveFileName(this, "Создать файл для таблицы", "", "Dixel files (*.dixel)");
        if (path.isEmpty()) return;

        auto *tableWindow = new TableWindow(path);
        tableWindow->show();
    } else {
        auto *tableWindow = new TableWindow(m_filePath);
        tableWindow->show();
    }
}

void MainWindow::setLightTheme() {
    applyTheme(AppTheme::Light);
}

void MainWindow::setDarkTheme() {
    applyTheme(AppTheme::Dark);
}

void MainWindow::toggleParagraphSpacing(int index) {
    int margin = m_paragraphSpacingCombo->itemData(index).toInt();

    QTextBlockFormat blockFmt;
    blockFmt.setBottomMargin(margin);
    m_pagedWidget->mergeCurrentBlockFormat(blockFmt);
    m_pagedWidget->setFocus();
}

void MainWindow::updatePageCountLabel(int count) {
    m_pageCountLabel->setText(QString("Страниц: %1").arg(count));
}

void MainWindow::scrollToPage(int pageIndex) {
    const int PAGE_GAP_LOCAL = 20;
    int targetY = pageIndex * (PAGE_HEIGHT + PAGE_GAP_LOCAL) + 20; // +20 — учитываем верхний отступ контейнера
    m_scrollArea->verticalScrollBar()->setValue(targetY);
}

void MainWindow::zoomIn() {
    m_pagedWidget->setZoom(m_pagedWidget->zoom() + 0.1);
}

void MainWindow::zoomOut() {
    m_pagedWidget->setZoom(m_pagedWidget->zoom() - 0.1);
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::MouseMove) {
        QPoint globalPos = QCursor::pos();
        QPoint viewportPos = m_scrollArea->viewport()->mapFromGlobal(globalPos);

        // Правый верхний угол именно листа, а не всего окна
        QPoint pageTopRight = m_pagedWidget->mapTo(m_scrollArea->viewport(), QPoint(m_pagedWidget->width(), 0));

        QRect hotZone(pageTopRight.x() - 130, 0, 130, 70);

        if (hotZone.contains(viewportPos)) {
            m_zoomWidget->move(pageTopRight.x() - 130, 20);
            m_zoomWidget->show();
            m_zoomWidget->raise();
        } else if (!m_zoomWidget->geometry().contains(viewportPos)) {
            m_zoomWidget->hide();
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::applyListStyle(int index) {
    int styleValue = m_listCombo->itemData(index).toInt();
    if (styleValue == -1) {
        m_pagedWidget->stopList();
    } else {
        m_pagedWidget->toggleList(static_cast<QTextListFormat::Style>(styleValue));
    }
    m_pagedWidget->setFocus();
}

void MainWindow::onNoteMarkerInserted(int index) {
    bool ok;
    QString text = QInputDialog::getMultiLineText(this, "Заметка", "Текст заметки (до 200 символов):", "", &ok);
    if (ok) {
        if (text.length() > 200) text = text.left(200);
        m_pagedWidget->setNoteText(index, text);
    }
}

void MainWindow::onNoteHovered(int index, QPoint globalPos) {
    if (m_notePopup->isVisible() && m_activeNoteIndex >= 0) return; // не перебиваем открытый режим редактирования

    m_activeNoteIndex = index;
    m_notePopup->setEditable(false);
    m_notePopup->setText(m_pagedWidget->getNoteText(index));
    m_notePopup->move(globalPos.x() + 15, globalPos.y() - 10);
    m_notePopup->show();
    m_notePopup->raise();
}

void MainWindow::onNoteHoverEnded() {
    m_notePopup->hide();
    m_activeNoteIndex = -1;
}

void MainWindow::onNoteClicked(int index, QPoint globalPos) {
    m_activeNoteIndex = index;
    m_notePopup->setEditable(true);
    m_notePopup->setText(m_pagedWidget->getNoteText(index));
    m_notePopup->move(globalPos.x() + 15, globalPos.y() - 10);
    m_notePopup->show();
    m_notePopup->raise();
    m_notePopup->setFocus();

}
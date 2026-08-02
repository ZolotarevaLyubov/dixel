// pagedtextwidget.cpp
#include "pagedtextwidget.h"
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTextBlock>
#include <QClipboard>
#include <QGuiApplication>
#include <QMenu>
#include <QContextMenuEvent>
#include <QRegularExpression>

PagedTextWidget::PagedTextWidget(QTextDocument *document, QWidget *parent)
    : QWidget(parent), m_document(document), m_cursor(document) {
    m_document->setPageSize(QSizeF(PAGE_WIDTH, PAGE_HEIGHT));

    int pageCount = m_document->pageCount();
    int totalHeight = pageCount * PAGE_HEIGHT + (pageCount - 1) * PAGE_GAP;
    setFixedSize(PAGE_WIDTH, totalHeight);

    connect(m_document, &QTextDocument::contentsChanged, this, [this]() {
        int pageCount = m_document->pageCount();
        int totalHeight = pageCount * PAGE_HEIGHT + (pageCount - 1) * PAGE_GAP;
        setFixedSize(PAGE_WIDTH, totalHeight);
        update();
        emit pageCountChanged(pageCount);
    });

    setFocusPolicy(Qt::StrongFocus);
}

int PagedTextWidget::hitTestToDocPos(const QPoint &widgetPos) {
    int pageHeightWithGap = PAGE_HEIGHT + PAGE_GAP;
    int page = widgetPos.y() / pageHeightWithGap;
    int localY = widgetPos.y() % pageHeightWithGap;
    if (localY > PAGE_HEIGHT) localY = PAGE_HEIGHT; // клик попал в серый промежуток

    qreal docY = page * PAGE_HEIGHT + localY;
    QPointF docPoint(widgetPos.x(), docY);

    return m_document->documentLayout()->hitTest(docPoint, Qt::FuzzyHit);
}

void PagedTextWidget::mousePressEvent(QMouseEvent *event) {
    int pos = hitTestToDocPos(event->pos());
    if (pos >= 0) {
        m_cursor.setPosition(pos);
        m_pendingFormat = m_cursor.charFormat();
        setFocus();
        update();
    }
}

void PagedTextWidget::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        int pos = hitTestToDocPos(event->pos());
        if (pos >= 0) {
            m_cursor.setPosition(pos, QTextCursor::KeepAnchor);
            update();
        }
    }
}

void PagedTextWidget::keyPressEvent(QKeyEvent *event) {

    if (event->modifiers() & Qt::ControlModifier) {
        switch (event->key()) {
            case Qt::Key_C: copySelection(); return;
            case Qt::Key_X: cutSelection(); return;
            case Qt::Key_V: pasteClipboard(); return;
            case Qt::Key_Z: undoAction(); return;
            case Qt::Key_Y: redoAction(); return;
            case Qt::Key_A: selectAll(); return;
            case Qt::Key_S: emit saveRequested(); return;
            default: break;
        }
    }

    switch (event->key()) {
        case Qt::Key_Left:
            m_cursor.movePosition(QTextCursor::Left);
            break;
        case Qt::Key_Right:
            m_cursor.movePosition(QTextCursor::Right);
            break;
        case Qt::Key_Up:
            m_cursor.movePosition(QTextCursor::Up);
            break;
        case Qt::Key_Down:
            m_cursor.movePosition(QTextCursor::Down);
            break;
        case Qt::Key_Backspace:
            m_cursor.deletePreviousChar();
            break;
        case Qt::Key_Delete:
            m_cursor.deleteChar();
            break;
        case Qt::Key_Return:
        case Qt::Key_Enter: {
            m_cursor.insertBlock();
            QTextBlockFormat blockFmt;
            blockFmt.setBottomMargin(10);
            m_cursor.mergeBlockFormat(blockFmt);
            break;
        }
        default:
            if (!event->text().isEmpty()) {
                QTextCharFormat fmt = m_cursor.charFormat();
                fmt.merge(m_pendingFormat);
                m_cursor.insertText(event->text(), fmt);
            }
            break;
    }
    update();
}

void PagedTextWidget::setTextCursor(const QTextCursor &cursor) {
    m_cursor = cursor;
    update();
}

void PagedTextWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.fillRect(rect(), QColor("#525659"));

    int pageCount = m_document->pageCount();

    for (int page = 0; page < pageCount; ++page) {
        int yOffset = page * (PAGE_HEIGHT + PAGE_GAP);

        // Белый фон листа
        painter.fillRect(QRect(0, yOffset, PAGE_WIDTH, PAGE_HEIGHT), Qt::white);

        // Рисуем срез документа, соответствующий этой странице
        painter.save();
        painter.translate(0, yOffset - page * PAGE_HEIGHT);
        painter.setClipRect(QRectF(0, page * PAGE_HEIGHT, PAGE_WIDTH, PAGE_HEIGHT));

        QAbstractTextDocumentLayout::PaintContext ctx;
        ctx.clip = QRectF(0, page * PAGE_HEIGHT, PAGE_WIDTH, PAGE_HEIGHT);

        QPalette palette;
        palette.setColor(QPalette::Text, Qt::black);
        ctx.palette = palette;

        if (m_cursor.hasSelection()) {
            QAbstractTextDocumentLayout::Selection selection;
            selection.cursor = m_cursor;
            selection.format.setBackground(QColor(160, 200, 250));
            ctx.selections.append(selection);
        }
        m_document->documentLayout()->draw(&painter, ctx);

        QTextBlock block = m_cursor.block();
        int cursorPos = m_cursor.position();
        QTextLine line = block.layout()->lineForTextPosition(cursorPos - block.position());

        if (line.isValid()) {
            QRectF blockRect = m_document->documentLayout()->blockBoundingRect(block);
            qreal x = line.cursorToX(cursorPos - block.position()) + blockRect.left();
            qDebug() << "Cursor draw X:" << x;
            //QRectF blockRect = m_document->documentLayout()->blockBoundingRect(block);
            qreal absoluteY = blockRect.top() + line.y();

            qreal pageTop = page * PAGE_HEIGHT;
            qreal pageBottom = pageTop + PAGE_HEIGHT;

            if (absoluteY >= pageTop && absoluteY < pageBottom) {
                painter.setPen(QPen(Qt::black, 1));
                painter.drawLine(QPointF(x, absoluteY), QPointF(x, absoluteY + line.height()));
            }
        }

        painter.restore();
    }
}

void PagedTextWidget::mergeCurrentCharFormat(const QTextCharFormat &format) {
    if (m_cursor.hasSelection()) {
        m_cursor.mergeCharFormat(format);
    }
    m_pendingFormat.merge(format);
    update();
}

void PagedTextWidget::setCurrentFont(const QFont &font) {
    QTextCharFormat fmt;
    fmt.setFontFamilies({font.family()});
    mergeCurrentCharFormat(fmt);
}

void PagedTextWidget::setFontPointSize(qreal size) {
    QTextCharFormat fmt;
    fmt.setFontPointSize(size);
    mergeCurrentCharFormat(fmt);
}

void PagedTextWidget::setAlignment(Qt::Alignment alignment) {
    QTextBlockFormat blockFmt;
    blockFmt.setAlignment(alignment);
    mergeCurrentBlockFormat(blockFmt);
}

void PagedTextWidget::mergeCurrentBlockFormat(const QTextBlockFormat &format) {
    m_cursor.mergeBlockFormat(format);
    update();
}

void PagedTextWidget::copySelection() {
    if (m_cursor.hasSelection()) {
        QGuiApplication::clipboard()->setText(m_cursor.selectedText().replace(QChar(0x2029), '\n'));
    }
}

void PagedTextWidget::cutSelection() {
    if (m_cursor.hasSelection()) {
        copySelection();
        m_cursor.removeSelectedText();
        update();
    }
}

void PagedTextWidget::pasteClipboard() {
    QString text = QGuiApplication::clipboard()->text();
    if (text.isEmpty()) return;

    // Убираем повторяющиеся пустые строки внутри вставляемого текста —
    // одиночный перенос строки остаётся, а двойной (пустая строка) схлопывается в одиночный
    text.replace(QRegularExpression("\\n{2,}"), "\n");

    int startPos = m_cursor.position();
    m_cursor.insertText(text);
    int endPos = m_cursor.position();
    applyDefaultParagraphSpacing(m_cursor, startPos, endPos);
    update();
}

void PagedTextWidget::undoAction() {
    m_document->undo(&m_cursor);
    update();
}

void PagedTextWidget::redoAction() {
    m_document->redo(&m_cursor);
    update();
}

void PagedTextWidget::selectAll() {
    m_cursor.select(QTextCursor::Document);
    update();
}

void PagedTextWidget::contextMenuEvent(QContextMenuEvent *event) {
    QMenu menu(this);

    auto *cutAction = menu.addAction("Вырезать");
    connect(cutAction, &QAction::triggered, this, &PagedTextWidget::cutSelection);

    auto *copyAction = menu.addAction("Копировать");
    connect(copyAction, &QAction::triggered, this, &PagedTextWidget::copySelection);

    auto *pasteAction = menu.addAction("Вставить");
    connect(pasteAction, &QAction::triggered, this, &PagedTextWidget::pasteClipboard);

    menu.addSeparator();

    auto *selectAllAction = menu.addAction("Выделить всё");
    connect(selectAllAction, &QAction::triggered, this, &PagedTextWidget::selectAll);

    menu.exec(event->globalPos());
}

void PagedTextWidget::applyDefaultParagraphSpacing(QTextCursor &cursor, int fromPos, int toPos) {
    QTextCursor formatCursor(m_document);
    formatCursor.setPosition(fromPos);
    formatCursor.setPosition(toPos, QTextCursor::KeepAnchor);

    QTextBlockFormat blockFmt;
    blockFmt.setBottomMargin(10); // фиксированный отступ между абзацами в пикселях
    formatCursor.mergeBlockFormat(blockFmt);
}
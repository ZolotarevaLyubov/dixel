// minimapwidget.cpp
#include "minimapwidget.h"
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QPainter>
#include <QMouseEvent>

MinimapWidget::MinimapWidget(QTextDocument *document, QWidget *parent)
    : QWidget(parent), m_document(document) {
    int miniPageWidth = PAGE_WIDTH * SCALE;
    setFixedWidth(miniPageWidth + 10);

    connect(m_document, &QTextDocument::contentsChanged, this, [this]() {
        update();
    });
}

void MinimapWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.fillRect(rect(), QColor("#3c3f41"));

    int pageCount = m_document->pageCount();
    int miniPageWidth = PAGE_WIDTH * SCALE;
    int miniPageHeight = PAGE_HEIGHT * SCALE;
    int miniPageGap = PAGE_GAP * SCALE;

    for (int page = 0; page < pageCount; ++page) {
        int yOffset = page * (miniPageHeight + miniPageGap) + 5;

        painter.fillRect(QRect(5, yOffset, miniPageWidth, miniPageHeight), Qt::white);

        painter.save();
        painter.translate(5, yOffset);
        painter.scale(SCALE, SCALE);
        painter.setClipRect(QRectF(0, 0, PAGE_WIDTH, PAGE_HEIGHT));

        QAbstractTextDocumentLayout::PaintContext ctx;
        ctx.clip = QRectF(0, page * PAGE_HEIGHT, PAGE_WIDTH, PAGE_HEIGHT);

        QPalette palette;
        palette.setColor(QPalette::Text, Qt::black);
        ctx.palette = palette;

        painter.translate(0, -page * PAGE_HEIGHT);
        m_document->documentLayout()->draw(&painter, ctx);

        painter.restore();
    }

    int totalHeight = pageCount * (miniPageHeight + miniPageGap) + 5;
    setFixedHeight(totalHeight);
}

void MinimapWidget::mousePressEvent(QMouseEvent *event) {
    int miniPageHeight = PAGE_HEIGHT * SCALE;
    int miniPageGap = PAGE_GAP * SCALE;
    int pageWithGap = miniPageHeight + miniPageGap;

    int clickedPage = (event->pos().y() - 5) / pageWithGap;

    int pageCount = m_document->pageCount();
    if (clickedPage >= 0 && clickedPage < pageCount) {
        emit pageClicked(clickedPage);
    }
}
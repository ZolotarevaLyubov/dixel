// pagedtextwidget.h
#ifndef DIXEL_PAGEDTEXTWIDGET_H
#define DIXEL_PAGEDTEXTWIDGET_H

#include <QWidget>
#include <QTextCursor>

class QTextDocument;

class PagedTextWidget : public QWidget {
    Q_OBJECT
public:
    explicit PagedTextWidget(QTextDocument *document, QWidget *parent = nullptr);

    QTextCursor textCursor() const { return m_cursor; }
    void setTextCursor(const QTextCursor &cursor);
    QTextDocument *document() const { return m_document; }
    void mergeCurrentCharFormat(const QTextCharFormat &format);
    void setCurrentFont(const QFont &font);
    void setFontPointSize(qreal size);
    void setAlignment(Qt::Alignment alignment);
    int fontWeight() const { return m_pendingFormat.fontWeight(); }
    bool fontItalic() const { return m_pendingFormat.fontItalic(); }
    bool fontUnderline() const { return m_pendingFormat.fontUnderline(); }
    QString toPlainText() const { return m_document->toPlainText(); }
    void mergeCurrentBlockFormat(const QTextBlockFormat &format);
    int pageCount() const { return m_document->pageCount(); }


protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    int hitTestToDocPos(const QPoint &widgetPos);
    void copySelection();
    void cutSelection();
    void pasteClipboard();
    void undoAction();
    void redoAction();
    void selectAll();
    void applyDefaultParagraphSpacing(QTextCursor &cursor, int fromPos, int toPos);

    QTextDocument *m_document;
    QTextCursor m_cursor;
    static const int PAGE_WIDTH = 794;
    static const int PAGE_HEIGHT = 1123;
    static const int PAGE_GAP = 20;
    QTextCharFormat m_pendingFormat;

    signals:
    void saveRequested();
    void pageCountChanged(int count);
};

#endif //DIXEL_PAGEDTEXTWIDGET_H
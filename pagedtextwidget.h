// pagedtextwidget.h
#ifndef DIXEL_PAGEDTEXTWIDGET_H
#define DIXEL_PAGEDTEXTWIDGET_H

#include <QWidget>
#include <QTextCursor>
#include <QTextListFormat>

class QTextDocument;

class PagedTextWidget : public QWidget {
    Q_OBJECT
    struct NoteData {
        QTextCursor cursor;
        QString text;
        QColor color = Qt::black;
        int fontSize = 11;
        bool glow = false;
    };
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
    void setZoom(qreal zoom);
    qreal zoom() const { return m_zoom; }
    void toggleList(QTextListFormat::Style style);
    void stopList();
    void addNoteAtSelection();
    QString getNoteText(int index) const { return m_notes[index].text; }
    void setNoteText(int index, const QString &text) { m_notes[index].text = text; update(); }
    QColor getNoteColor(int index) const { return m_notes[index].color; }
    void setNoteColor(int index, const QColor &color) { m_notes[index].color = color; }
    int getNoteFontSize(int index) const { return m_notes[index].fontSize; }
    void setNoteFontSize(int index, int size) { m_notes[index].fontSize = size; }
    bool getNoteGlow(int index) const { return m_notes[index].glow; }
    void setNoteGlow(int index, bool glow) { m_notes[index].glow = glow; }
    QRect noteMarkerScreenRect(int index) const;
    int noteCount() const { return m_notes.size(); }
    QStringList notesTexts() const;
    void loadNotes(const QStringList &texts);

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
    void updateWidgetSize();
    qreal m_zoom = 1.0;
    void sortNotesByPosition();

    QTextDocument *m_document;
    QTextCursor m_cursor;
    static const int PAGE_WIDTH = 794;
    static const int PAGE_HEIGHT = 1123;
    static const int PAGE_GAP = 20;
    QTextCharFormat m_pendingFormat;
    QVector<NoteData> m_notes;
    QVector<QRect> m_noteMarkerRects;
    int m_hoveredNoteIndex = -1;

    signals:
    void saveRequested();
    void pageCountChanged(int count);
    void noteMarkerInserted(int index);
    void noteHovered(int index, QPoint globalPos);
    void noteHoverEnded();
    void noteClicked(int index, QPoint globalPos);
};

#endif //DIXEL_PAGEDTEXTWIDGET_H
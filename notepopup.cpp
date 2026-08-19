// notepopup.cpp
#include "notepopup.h"
#include <QTextEdit>
#include <QVBoxLayout>

NotePopup::NotePopup(QWidget *parent) : QWidget(parent) {
    setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
    setFixedSize(220, 140);
    setStyleSheet(
        "NotePopup { background-color: #fff8dc; border: 1px solid #d4c896; border-radius: 6px; }"
    );

    m_textEdit = new QTextEdit(this);
    m_textEdit->setStyleSheet(
        "QTextEdit { background-color: transparent; border: none; color: #333; font-family: 'Segoe UI'; font-size: 11pt; }"
    );

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->addWidget(m_textEdit);

    connect(m_textEdit, &QTextEdit::textChanged, this, &NotePopup::onTextChanged);
}

void NotePopup::setText(const QString &text) {
    m_textEdit->blockSignals(true);
    m_textEdit->setPlainText(text);
    m_textEdit->blockSignals(false);
}

QString NotePopup::text() const {
    return m_textEdit->toPlainText();
}

void NotePopup::setEditable(bool editable) {
    m_textEdit->setReadOnly(!editable);
}

void NotePopup::onTextChanged() {
    QString text = m_textEdit->toPlainText();
    if (text.length() > MAX_CHARS) {
        text = text.left(MAX_CHARS);
        m_textEdit->blockSignals(true);
        m_textEdit->setPlainText(text);
        QTextCursor cursor = m_textEdit->textCursor();
        cursor.movePosition(QTextCursor::End);
        m_textEdit->setTextCursor(cursor);
        m_textEdit->blockSignals(false);
    }
    emit textChanged(text);
}
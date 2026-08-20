// notepopup.cpp
#include "notepopup.h"
#include <QTextEdit>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QColorDialog>
#include <QGraphicsDropShadowEffect>

NotePopup::NotePopup(QWidget *parent) : QWidget(parent) {
    setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_ShowWithoutActivating);
    setFixedSize(220, 175);
    setStyleSheet(
    "NotePopup { background-color: #e8e8e8; border: 1px solid #c5c5c5; border-radius: 6px; }"
    );

    m_textEdit = new QTextEdit(this);
    m_textEdit->setStyleSheet(
        "QTextEdit { background-color: transparent; border: none; font-family: 'Segoe UI'; }"
    );

    m_glowEffect = new QGraphicsDropShadowEffect(this);
    m_glowEffect->setBlurRadius(0);
    m_glowEffect->setColor(QColor(255, 230, 100));
    m_glowEffect->setOffset(0, 0);
    m_textEdit->setGraphicsEffect(m_glowEffect);

    // Панель с ручкой (всегда видна)
    m_penButton = new QPushButton("🖊", this);
    m_penButton->setFixedSize(28, 28);
    m_penButton->setStyleSheet(
        "QPushButton { background-color: #d0d0d0; border: 1px solid #b0b0b0; border-radius: 14px; font-size: 16px; color: black; }"
        "QPushButton:hover { background-color: #c0c0c0; }"
    );
    connect(m_penButton, &QPushButton::clicked, this, &NotePopup::togglePenPanel);

    // Скрытая панель с настройками (появляется по клику на ручку)
    m_penPanel = new QWidget(this);
    m_penPanel->setVisible(false);
    auto *penLayout = new QHBoxLayout(m_penPanel);
    penLayout->setContentsMargins(0, 0, 0, 0);
    penLayout->setSpacing(4);

    m_colorButton = new QPushButton(this);
    m_colorButton->setFixedSize(24, 24);
    m_colorButton->setStyleSheet("QPushButton { background-color: black; border-radius: 4px; }");
    connect(m_colorButton, &QPushButton::clicked, this, &NotePopup::pickColor);

    m_sizeCombo = new QComboBox(this);
    for (int s : {8, 9, 10, 11, 12, 14, 16, 18, 20}) m_sizeCombo->addItem(QString::number(s), s);
    m_sizeCombo->setCurrentText("11");
    m_sizeCombo->setFixedWidth(50);
    connect(m_sizeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &NotePopup::changeFontSize);

    m_glowButton = new QPushButton("✨", this);
    m_glowButton->setFixedSize(24, 24);
    m_glowButton->setCheckable(true);
    m_glowButton->setStyleSheet(
        "QPushButton { background-color: #d0d0d0; border: 1px solid #b0b0b0; border-radius: 4px; font-size: 14px; color: black; }"
        "QPushButton:checked { background-color: rgba(255,200,0,150); }"
    );
    connect(m_glowButton, &QPushButton::clicked, this, &NotePopup::toggleGlow);

    penLayout->addWidget(m_colorButton);
    penLayout->addWidget(m_sizeCombo);
    penLayout->addWidget(m_glowButton);

    auto *bottomLayout = new QHBoxLayout();
    bottomLayout->addWidget(m_penButton);
    bottomLayout->addWidget(m_penPanel);
    bottomLayout->addStretch();

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 8, 8, 6);
    mainLayout->addWidget(m_textEdit);
    mainLayout->addLayout(bottomLayout);

    connect(m_textEdit, &QTextEdit::textChanged, this, &NotePopup::onTextChanged);
    m_penButton->raise();
}

void NotePopup::togglePenPanel() {
    qDebug() << "Pen button clicked, panel visible before:" << m_penPanel->isVisible();
    m_penPanel->setVisible(!m_penPanel->isVisible());
    qDebug() << "Panel visible after:" << m_penPanel->isVisible();
}

void NotePopup::pickColor() {
    QColor color = QColorDialog::getColor(m_currentColor, this, "Цвет текста заметки");
    if (color.isValid()) {
        m_currentColor = color;
        m_colorButton->setStyleSheet(QString("QPushButton { background-color: %1; border-radius: 4px; }").arg(color.name()));
        applyStyle();
        emit colorChanged(color);
    }
}

void NotePopup::changeFontSize(int index) {
    m_currentFontSize = m_sizeCombo->itemData(index).toInt();
    applyStyle();
    emit fontSizeChanged(m_currentFontSize);
}

void NotePopup::toggleGlow() {
    m_glowEnabled = m_glowButton->isChecked();
    applyStyle();
    emit glowChanged(m_glowEnabled);
}

void NotePopup::applyStyle() {
    QString style = QString("QTextEdit { background-color: transparent; border: none; font-family: 'Segoe UI'; font-size: %1pt; color: %2; }")
        .arg(m_currentFontSize).arg(m_currentColor.name());
    m_textEdit->setStyleSheet(style);

    if (m_glowEnabled) {
        m_glowEffect->setBlurRadius(20);
        m_glowEffect->setColor(m_currentColor.lighter(150));
    } else {
        m_glowEffect->setBlurRadius(0);
    }
}

void NotePopup::setNoteColor(const QColor &color) {
    m_currentColor = color;
    m_colorButton->setStyleSheet(QString("QPushButton { background-color: %1; border-radius: 4px; }").arg(color.name()));
    applyStyle();
}

void NotePopup::setNoteFontSize(int size) {
    m_currentFontSize = size;
    m_sizeCombo->setCurrentText(QString::number(size));
    applyStyle();
}

void NotePopup::setNoteGlow(bool glow) {
    m_glowEnabled = glow;
    m_glowButton->setChecked(glow);
    applyStyle();
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
    m_penButton->setVisible(editable);
    if (!editable) m_penPanel->setVisible(false);
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
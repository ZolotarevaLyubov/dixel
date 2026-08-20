// notepopup.h
#ifndef DIXEL_NOTEPOPUP_H
#define DIXEL_NOTEPOPUP_H

#include <QWidget>
#include <QColor>

class QTextEdit;
class QPushButton;
class QComboBox;
class QWidget;
class QGraphicsDropShadowEffect;

class NotePopup : public QWidget {
    Q_OBJECT
public:
    explicit NotePopup(QWidget *parent = nullptr);

    void setText(const QString &text);
    QString text() const;
    void setEditable(bool editable);

    void setNoteColor(const QColor &color);
    void setNoteFontSize(int size);
    void setNoteGlow(bool glow);

    signals:
        void textChanged(const QString &text);
    void colorChanged(const QColor &color);
    void fontSizeChanged(int size);
    void glowChanged(bool glow);

private slots:
    void onTextChanged();
    void togglePenPanel();
    void pickColor();
    void changeFontSize(int index);
    void toggleGlow();

private:
    void applyStyle();

    QTextEdit *m_textEdit;
    QWidget *m_penPanel;
    QPushButton *m_penButton;
    QPushButton *m_colorButton;
    QComboBox *m_sizeCombo;
    QPushButton *m_glowButton;
    QGraphicsDropShadowEffect *m_glowEffect;

    QColor m_currentColor = Qt::black;
    int m_currentFontSize = 11;
    bool m_glowEnabled = false;

    static const int MAX_CHARS = 200;
};

#endif //DIXEL_NOTEPOPUP_H
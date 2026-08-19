// notepopup.h
#ifndef DIXEL_NOTEPOPUP_H
#define DIXEL_NOTEPOPUP_H

#include <QWidget>

class QTextEdit;
class QPushButton;

class NotePopup : public QWidget {
    Q_OBJECT
public:
    explicit NotePopup(QWidget *parent = nullptr);

    void setText(const QString &text);
    QString text() const;
    void setEditable(bool editable);

    signals:
        void textChanged(const QString &text);

private:
    QTextEdit *m_textEdit;
    static const int MAX_CHARS = 200;

private slots:
    void onTextChanged();
};

#endif //DIXEL_NOTEPOPUP_H
// minimapwidget.h
#ifndef DIXEL_MINIMAPWIDGET_H
#define DIXEL_MINIMAPWIDGET_H

#include <QWidget>

class QTextDocument;

class MinimapWidget : public QWidget {
    Q_OBJECT
public:
    explicit MinimapWidget(QTextDocument *document, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QTextDocument *m_document;
    static const int PAGE_WIDTH = 794;
    static const int PAGE_HEIGHT = 1123;
    static const int PAGE_GAP = 20;
    static constexpr double SCALE = 0.12; // масштаб миниатюры


    signals:
        void pageClicked(int pageIndex);
};

#endif //DIXEL_MINIMAPWIDGET_H
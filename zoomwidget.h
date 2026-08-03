// zoomwidget.h
#ifndef DIXEL_ZOOMWIDGET_H
#define DIXEL_ZOOMWIDGET_H

#include <QWidget>

class QPushButton;
class QLabel;

class ZoomWidget : public QWidget {
    Q_OBJECT
public:
    explicit ZoomWidget(QWidget *parent = nullptr);

    signals:
        void zoomInRequested();
    void zoomOutRequested();

private:
    QPushButton *m_zoomInBtn;
    QPushButton *m_zoomOutBtn;
    QLabel *m_iconLabel;
};

#endif //DIXEL_ZOOMWIDGET_H
// zoomwidget.cpp
#include "zoomwidget.h"
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>

ZoomWidget::ZoomWidget(QWidget *parent) : QWidget(parent) {
    setFixedSize(110, 36);
    setStyleSheet(
        "ZoomWidget { background-color: rgba(240, 240, 240, 230); border-radius: 8px; }"
        "QPushButton { background-color: transparent; color: black; border: none; font-size: 20px; font-weight: bold; }"
        "QPushButton:hover { background-color: rgba(0, 0, 0, 30); border-radius: 4px; }"
        "QLabel { color: black; font-size: 20px; }"
    );

    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(8, 2, 8, 2);
    layout->setSpacing(6);

    m_zoomOutBtn = new QPushButton("−", this);
    m_zoomOutBtn->setFixedSize(28, 28);
    connect(m_zoomOutBtn, &QPushButton::clicked, this, &ZoomWidget::zoomOutRequested);

    m_iconLabel = new QLabel("🔍", this);

    m_zoomInBtn = new QPushButton("+", this);
    m_zoomInBtn->setFixedSize(28, 28);
    connect(m_zoomInBtn, &QPushButton::clicked, this, &ZoomWidget::zoomInRequested);

    layout->addWidget(m_zoomOutBtn);
    layout->addWidget(m_iconLabel);
    layout->addWidget(m_zoomInBtn);
}
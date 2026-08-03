// zoomwidget.cpp
#include "zoomwidget.h"
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>

ZoomWidget::ZoomWidget(QWidget *parent) : QWidget(parent) {
    setFixedSize(90, 30);
    setStyleSheet(
        "ZoomWidget { background-color: rgba(60, 60, 60, 200); border-radius: 6px; }"
        "QPushButton { background-color: transparent; color: white; border: none; font-size: 16px; font-weight: bold; }"
        "QPushButton:hover { background-color: rgba(255, 255, 255, 40); border-radius: 4px; }"
        "QLabel { color: white; font-size: 14px; }"
    );

    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(6, 2, 6, 2);
    layout->setSpacing(4);

    m_zoomOutBtn = new QPushButton("−", this);
    m_zoomOutBtn->setFixedSize(24, 24);
    connect(m_zoomOutBtn, &QPushButton::clicked, this, &ZoomWidget::zoomOutRequested);

    m_iconLabel = new QLabel("🔍", this);

    m_zoomInBtn = new QPushButton("+", this);
    m_zoomInBtn->setFixedSize(24, 24);
    connect(m_zoomInBtn, &QPushButton::clicked, this, &ZoomWidget::zoomInRequested);

    layout->addWidget(m_zoomOutBtn);
    layout->addWidget(m_iconLabel);
    layout->addWidget(m_zoomInBtn);
}
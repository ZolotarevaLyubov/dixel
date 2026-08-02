// thememanager.cpp
#include "thememanager.h"
#include <QApplication>
#include <QSettings>

static AppTheme g_currentTheme = AppTheme::Light;

void applyTheme(AppTheme theme) {
    g_currentTheme = theme;

    if (theme == AppTheme::Dark) {
        qApp->setStyleSheet(
        "QMainWindow { background-color: #2b2b2b; }"
           "QMenuBar { background-color: #2b2b2b; color: #e0e0e0; }"
           "QMenuBar::item:selected { background-color: #3c3f41; }"
           "QMenu { background-color: #2b2b2b; color: #e0e0e0; }"
           "QMenu::item:selected { background-color: #3c3f41; }"
           "QToolBar { background-color: #2b2b2b; }"
           "QComboBox { background-color: #3c3f41; color: #e0e0e0; }"
           "QToolButton { background-color: #3c3f41; color: #e0e0e0; border: 1px solid #4a4a4a; border-radius: 3px; padding: 4px; }"
           "QToolButton:hover { background-color: #4a4d4f; }"
           "QToolButton:pressed, QToolButton:checked { background-color: #2a6099; }"
           "QWidget#pageContainer { background-color: #525659; }"
           "QScrollArea#pageScrollArea { background-color: #525659; }"
        );
    } else {
        qApp->setStyleSheet(
            "QMainWindow { background-color: #f0f0f0; }"
            "QMenuBar { background-color: #f0f0f0; color: #000000; }"
            "QMenuBar::item:selected { background-color: #d0d0d0; }"
            "QMenu { background-color: #ffffff; color: #000000; }"
            "QMenu::item:selected { background-color: #d0d0d0; }"
            "QToolBar { background-color: #f0f0f0; }"
            "QComboBox { background-color: #ffffff; color: #000000; }"
            "QWidget#pageContainer { background-color: #d0d0d0; }"
            "QScrollArea#pageScrollArea { background-color: #d0d0d0; }"
            "QToolButton { background-color: #e8e8e8; color: #000000; border: 1px solid #c0c0c0; border-radius: 3px; padding: 4px; }"
            "QToolButton:hover { background-color: #d8d8d8; }"
            "QToolButton:pressed, QToolButton:checked { background-color: #b0d0f0; }"
        );
    }
    QSettings settings("Dixel", "DixelApp");
    settings.setValue("theme", theme == AppTheme::Dark ? "dark" : "light");
}

AppTheme currentTheme() {
    return g_currentTheme;
}

void loadSavedTheme() {
    QSettings settings("Dixel", "DixelApp");
    QString saved = settings.value("theme", "light").toString();
    applyTheme(saved == "dark" ? AppTheme::Dark : AppTheme::Light);
}
// thememanager.h
#ifndef DIXEL_THEMEMANAGER_H
#define DIXEL_THEMEMANAGER_H

enum class AppTheme { Light, Dark };

void applyTheme(AppTheme theme);
AppTheme currentTheme();
void loadSavedTheme();

#endif //DIXEL_THEMEMANAGER_H
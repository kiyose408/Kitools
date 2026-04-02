#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QSettings>
#include <QColor>
#include <QSize>

class SettingsManager : public QObject
{
    Q_OBJECT

public:
    static SettingsManager* instance();

    int defaultHours() const;
    int defaultMinutes() const;
    int defaultSeconds() const;
    
    int workDuration() const;
    int breakDuration() const;
    int cycleCount() const;
    
    int overlayWidth() const;
    int overlayHeight() const;
    int fontSize() const;
    QColor backgroundColor() const;
    QColor textColor() const;
    int backgroundOpacity() const;
    int textOpacity() const;
    bool soundEnabled() const;
    bool flashEnabled() const;

    void setDefaultTime(int hours, int minutes, int seconds);
    void setWorkDuration(int minutes);
    void setBreakDuration(int minutes);
    void setCycleCount(int count);
    void setOverlaySize(int width, int height);
    void setFontSize(int size);
    void setBackgroundColor(const QColor &color);
    void setTextColor(const QColor &color);
    void setBackgroundOpacity(int opacity);
    void setTextOpacity(int opacity);
    void setSoundEnabled(bool enabled);
    void setFlashEnabled(bool enabled);

    bool autoStartEnabled() const;
    void setAutoStartEnabled(bool enabled);

    bool clipboardAutoStart() const;
    void setClipboardAutoStart(bool enabled);

    bool timeTrackerAutoStart() const;
    void setTimeTrackerAutoStart(bool enabled);

    void saveAll();

private:
    explicit SettingsManager(QObject *parent = nullptr);
    ~SettingsManager();
    SettingsManager(const SettingsManager&) = delete;
    SettingsManager& operator=(const SettingsManager&) = delete;

    QSettings *m_settings;
};

#endif

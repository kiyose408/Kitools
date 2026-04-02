#include "settingsmanager.h"

SettingsManager* SettingsManager::instance()
{
    static SettingsManager instance;
    return &instance;
}

SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent)
    , m_settings(new QSettings("Kitools", "PCProductivityToolbox", this))
{
}

SettingsManager::~SettingsManager()
{
    saveAll();
}

int SettingsManager::defaultHours() const
{
    return m_settings->value("timer/defaultHours", 0).toInt();
}

int SettingsManager::defaultMinutes() const
{
    return m_settings->value("timer/defaultMinutes", 25).toInt();
}

int SettingsManager::defaultSeconds() const
{
    return m_settings->value("timer/defaultSeconds", 0).toInt();
}

int SettingsManager::workDuration() const
{
    return m_settings->value("timer/workDuration", 25).toInt();
}

int SettingsManager::breakDuration() const
{
    return m_settings->value("timer/breakDuration", 5).toInt();
}

int SettingsManager::cycleCount() const
{
    return m_settings->value("timer/cycleCount", 4).toInt();
}

int SettingsManager::overlayWidth() const
{
    return m_settings->value("overlay/width", 200).toInt();
}

int SettingsManager::overlayHeight() const
{
    return m_settings->value("overlay/height", 80).toInt();
}

int SettingsManager::fontSize() const
{
    return m_settings->value("overlay/fontSize", 32).toInt();
}

QColor SettingsManager::backgroundColor() const
{
    return m_settings->value("overlay/backgroundColor", QColor(40, 40, 40)).value<QColor>();
}

QColor SettingsManager::textColor() const
{
    return m_settings->value("overlay/textColor", QColor(255, 255, 255)).value<QColor>();
}

int SettingsManager::backgroundOpacity() const
{
    return m_settings->value("overlay/backgroundOpacity", 200).toInt();
}

int SettingsManager::textOpacity() const
{
    return m_settings->value("overlay/textOpacity", 255).toInt();
}

bool SettingsManager::soundEnabled() const
{
    return m_settings->value("alert/soundEnabled", true).toBool();
}

bool SettingsManager::flashEnabled() const
{
    return m_settings->value("alert/flashEnabled", true).toBool();
}

void SettingsManager::setDefaultTime(int hours, int minutes, int seconds)
{
    m_settings->setValue("timer/defaultHours", hours);
    m_settings->setValue("timer/defaultMinutes", minutes);
    m_settings->setValue("timer/defaultSeconds", seconds);
}

void SettingsManager::setWorkDuration(int minutes)
{
    m_settings->setValue("timer/workDuration", minutes);
}

void SettingsManager::setBreakDuration(int minutes)
{
    m_settings->setValue("timer/breakDuration", minutes);
}

void SettingsManager::setCycleCount(int count)
{
    m_settings->setValue("timer/cycleCount", count);
}

void SettingsManager::setOverlaySize(int width, int height)
{
    m_settings->setValue("overlay/width", width);
    m_settings->setValue("overlay/height", height);
}

void SettingsManager::setFontSize(int size)
{
    m_settings->setValue("overlay/fontSize", size);
}

void SettingsManager::setBackgroundColor(const QColor &color)
{
    m_settings->setValue("overlay/backgroundColor", color);
}

void SettingsManager::setTextColor(const QColor &color)
{
    m_settings->setValue("overlay/textColor", color);
}

void SettingsManager::setBackgroundOpacity(int opacity)
{
    m_settings->setValue("overlay/backgroundOpacity", opacity);
}

void SettingsManager::setTextOpacity(int opacity)
{
    m_settings->setValue("overlay/textOpacity", opacity);
}

void SettingsManager::setSoundEnabled(bool enabled)
{
    m_settings->setValue("alert/soundEnabled", enabled);
}

void SettingsManager::setFlashEnabled(bool enabled)
{
    m_settings->setValue("alert/flashEnabled", enabled);
}

bool SettingsManager::autoStartEnabled() const
{
    return m_settings->value("autostart/enabled", false).toBool();
}

void SettingsManager::setAutoStartEnabled(bool enabled)
{
    m_settings->setValue("autostart/enabled", enabled);
}

bool SettingsManager::clipboardAutoStart() const
{
    return m_settings->value("autostart/clipboardAutoStart", true).toBool();
}

void SettingsManager::setClipboardAutoStart(bool enabled)
{
    m_settings->setValue("autostart/clipboardAutoStart", enabled);
}

bool SettingsManager::timeTrackerAutoStart() const
{
    return m_settings->value("autostart/timeTrackerAutoStart", true).toBool();
}

void SettingsManager::setTimeTrackerAutoStart(bool enabled)
{
    m_settings->setValue("autostart/timeTrackerAutoStart", enabled);
}

void SettingsManager::saveAll()
{
    m_settings->sync();
}

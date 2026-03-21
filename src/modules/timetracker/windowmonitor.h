#ifndef WINDOWMONITOR_H
#define WINDOWMONITOR_H

#include <QObject>
#include <QString>
#include <QTimer>
#include <windows.h>

#include "timetrackerdatatypes.h"

struct WindowInfo {
    QString processName;
    QString windowTitle;
    QString filePath;
    HWND hwnd;
    DWORD processId;
};

class WindowMonitor : public QObject {
    Q_OBJECT

public:
    static WindowMonitor* instance(QObject* parent = nullptr);

    void startMonitoring(int intervalMs = 1000);
    void stopMonitoring();
    bool isMonitoring() const;

    WindowInfo getCurrentWindow() const;
    WindowInfo getActiveWindow();

signals:
    void windowChanged(const WindowInfo& oldWindow, const WindowInfo& newWindow);
    void activityRecorded(const ActivityRecord& record);

private slots:
    void onTimerTick();

private:
    explicit WindowMonitor(QObject* parent = nullptr);
    ~WindowMonitor();

    WindowInfo getForegroundWindowInfo();
    QString getWindowTitle(HWND hwnd);
    QString getProcessName(DWORD processId);
    QString getProcessPath(DWORD processId);

    static WindowMonitor* m_instance;

    QTimer* m_timer;
    WindowInfo m_currentWindow;
    QDateTime m_windowStartTime;
    bool m_isMonitoring;
};

#endif

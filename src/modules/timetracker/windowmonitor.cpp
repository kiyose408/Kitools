#include "windowmonitor.h"
#include <QDebug>
#include <QDateTime>

#ifdef Q_OS_WIN
#include <psapi.h>
#endif

WindowMonitor* WindowMonitor::m_instance = nullptr;

WindowMonitor::WindowMonitor(QObject* parent)
    : QObject(parent)
    , m_timer(nullptr)
    , m_isMonitoring(false)
{
    qDebug() << "WindowMonitor 初始化";
    
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &WindowMonitor::onTimerTick);
}

WindowMonitor::~WindowMonitor() {
    stopMonitoring();
}

WindowMonitor* WindowMonitor::instance(QObject* parent) {
    if (!m_instance) {
        m_instance = new WindowMonitor(parent);
    }
    return m_instance;
}

void WindowMonitor::startMonitoring(int intervalMs) {
    if (m_isMonitoring) {
        return;
    }
    
    qDebug() << "开始窗口监控，间隔:" << intervalMs << "ms";
    m_isMonitoring = true;
    m_windowStartTime = QDateTime::currentDateTime();
    m_currentWindow = getActiveWindow();
    
    m_timer->start(intervalMs);
}

void WindowMonitor::stopMonitoring() {
    if (!m_isMonitoring) {
        return;
    }
    
    qDebug() << "停止窗口监控";
    m_timer->stop();
    m_isMonitoring = false;
}

bool WindowMonitor::isMonitoring() const {
    return m_isMonitoring;
}

WindowInfo WindowMonitor::getCurrentWindow() const {
    return m_currentWindow;
}

WindowInfo WindowMonitor::getActiveWindow() {
    return getForegroundWindowInfo();
}

void WindowMonitor::onTimerTick() {
    WindowInfo newWindow = getForegroundWindowInfo();
    
    if (newWindow.processName != m_currentWindow.processName ||
        newWindow.windowTitle != m_currentWindow.windowTitle) {
        
        if (!m_currentWindow.processName.isEmpty()) {
            ActivityRecord record;
            record.processName = m_currentWindow.processName;
            record.windowTitle = m_currentWindow.windowTitle;
            record.filePath = m_currentWindow.filePath;
            record.startTime = m_windowStartTime;
            record.endTime = QDateTime::currentDateTime();
            record.durationSeconds = m_windowStartTime.secsTo(record.endTime);
            
            emit activityRecorded(record);
        }
        
        emit windowChanged(m_currentWindow, newWindow);
        
        m_currentWindow = newWindow;
        m_windowStartTime = QDateTime::currentDateTime();
    }
}

WindowInfo WindowMonitor::getForegroundWindowInfo() {
    WindowInfo info;
    
#ifdef Q_OS_WIN
    HWND hwnd = GetForegroundWindow();
    if (!hwnd) {
        return info;
    }
    
    info.hwnd = hwnd;
    info.windowTitle = getWindowTitle(hwnd);
    
    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);
    info.processId = processId;
    
    if (processId > 0) {
        info.processName = getProcessName(processId);
        info.filePath = getProcessPath(processId);
    }
#endif
    
    return info;
}

QString WindowMonitor::getWindowTitle(HWND hwnd) {
#ifdef Q_OS_WIN
    int length = GetWindowTextLengthW(hwnd);
    if (length <= 0) {
        return QString();
    }
    
    wchar_t* buffer = new wchar_t[length + 1];
    GetWindowTextW(hwnd, buffer, length + 1);
    QString title = QString::fromWCharArray(buffer);
    delete[] buffer;
    return title;
#else
    Q_UNUSED(hwnd);
    return QString();
#endif
}

QString WindowMonitor::getProcessName(DWORD processId) {
#ifdef Q_OS_WIN
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (!hProcess) {
        return QString();
    }
    
    wchar_t processName[MAX_PATH];
    if (GetModuleBaseNameW(hProcess, nullptr, processName, MAX_PATH)) {
        CloseHandle(hProcess);
        return QString::fromWCharArray(processName);
    }
    
    CloseHandle(hProcess);
    return QString();
#else
    Q_UNUSED(processId);
    return QString();
#endif
}

QString WindowMonitor::getProcessPath(DWORD processId) {
#ifdef Q_OS_WIN
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (!hProcess) {
        return QString();
    }
    
    wchar_t processPath[MAX_PATH];
    if (GetModuleFileNameExW(hProcess, nullptr, processPath, MAX_PATH)) {
        CloseHandle(hProcess);
        return QString::fromWCharArray(processPath);
    }
    
    CloseHandle(hProcess);
    return QString();
#else
    Q_UNUSED(processId);
    return QString();
#endif
}

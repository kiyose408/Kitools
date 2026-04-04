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
    qDebug() << "========== WindowMonitor 初始化 ==========";
    
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &WindowMonitor::onTimerTick);
    
    qDebug() << "WindowMonitor 初始化完成";
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
        qDebug() << "窗口监控已经在运行中";
        return;
    }
    
    qDebug() << "========== 开始窗口监控 ==========";
    qDebug() << "监控间隔:" << intervalMs << "ms";
    
    m_isMonitoring = true;
    m_windowStartTime = QDateTime::currentDateTime();
    m_currentWindow = getActiveWindow();
    
    qDebug() << "初始窗口信息:";
    qDebug() << "  进程名:" << m_currentWindow.processName;
    qDebug() << "  窗口标题:" << m_currentWindow.windowTitle;
    qDebug() << "  文件路径:" << m_currentWindow.filePath;
    
    m_timer->start(intervalMs);
    
    qDebug() << "定时器已启动";
}

void WindowMonitor::stopMonitoring() {
    if (!m_isMonitoring) {
        qDebug() << "窗口监控未在运行";
        return;
    }
    
    qDebug() << "========== 停止窗口监控 ==========";
    m_timer->stop();
    m_isMonitoring = false;
    qDebug() << "监控已停止";
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
    
    bool processChanged = (newWindow.processName != m_currentWindow.processName);
    bool titleChanged = (newWindow.windowTitle != m_currentWindow.windowTitle);
    
    if (processChanged || titleChanged) {
        qDebug() << "";
        qDebug() << "==== 窗口切换 ====";
        
        if (processChanged) {
            qDebug() << "进程:" << m_currentWindow.processName << "->" << newWindow.processName;
        }
        
        if (!m_currentWindow.processName.isEmpty()) {
            ActivityRecord record;
            record.processName = m_currentWindow.processName;
            record.windowTitle = m_currentWindow.windowTitle;
            record.filePath = m_currentWindow.filePath;
            record.startTime = m_windowStartTime;
            record.endTime = QDateTime::currentDateTime();
            record.durationSeconds = m_windowStartTime.secsTo(record.endTime);
            
            if (record.durationSeconds >= 1) {
                qDebug() << "记录:" << record.processName 
                         << "| 时长:" << record.durationSeconds << "秒"
                         << "| 标题:" << record.windowTitle.left(50);
                emit activityRecorded(record);
            }
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
        qDebug() << "获取前台窗口句柄失败";
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
    
    qDebug() << "获取前台窗口信息成功:" << info.processName;
#else
    qDebug() << "非Windows平台，无法获取窗口信息";
#endif
    
    return info;
}

QString WindowMonitor::getWindowTitle(HWND hwnd) {
#ifdef Q_OS_WIN
    int length = GetWindowTextLengthW(hwnd);
    if (length > 0) {
        wchar_t* buffer = new wchar_t[length + 1];
        GetWindowTextW(hwnd, buffer, length + 1);
        QString title = QString::fromWCharArray(buffer);
        delete[] buffer;
        if (!title.isEmpty()) {
            return title;
        }
    }
    
    wchar_t className[256];
    if (GetClassNameW(hwnd, className, 256) > 0) {
        QString classNameStr = QString::fromWCharArray(className);
        qDebug() << "窗口标题为空，使用类名:" << classNameStr;
        return QString("[Window] %1").arg(classNameStr);
    }
    
    qDebug() << "无法获取窗口标题或类名";
    return QString("[Unknown Window]");
#else
    Q_UNUSED(hwnd);
    return QString();
#endif
}

QString WindowMonitor::getProcessName(DWORD processId) {
#ifdef Q_OS_WIN
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (!hProcess) {
        hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    }
    
    if (!hProcess) {
        qDebug() << "打开进程失败，PID:" << processId << "错误码:" << GetLastError();
        return QString("PID_%1").arg(processId);
    }
    
    wchar_t processName[MAX_PATH];
    if (GetModuleBaseNameW(hProcess, nullptr, processName, MAX_PATH)) {
        CloseHandle(hProcess);
        return QString::fromWCharArray(processName);
    }
    
    DWORD size = MAX_PATH;
    if (QueryFullProcessImageNameW(hProcess, 0, processName, &size)) {
        CloseHandle(hProcess);
        QString fullPath = QString::fromWCharArray(processName);
        int lastSlash = fullPath.lastIndexOf('\\');
        if (lastSlash >= 0) {
            return fullPath.mid(lastSlash + 1);
        }
        return fullPath;
    }
    
    qDebug() << "获取进程名失败，PID:" << processId << "错误码:" << GetLastError();
    CloseHandle(hProcess);
    return QString("PID_%1").arg(processId);
#else
    Q_UNUSED(processId);
    return QString();
#endif
}

QString WindowMonitor::getProcessPath(DWORD processId) {
#ifdef Q_OS_WIN
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (!hProcess) {
        hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    }
    
    if (!hProcess) {
        qDebug() << "获取进程路径失败，PID:" << processId << "错误码:" << GetLastError();
        return QString();
    }
    
    wchar_t processPath[MAX_PATH];
    if (GetModuleFileNameExW(hProcess, nullptr, processPath, MAX_PATH)) {
        CloseHandle(hProcess);
        return QString::fromWCharArray(processPath);
    }
    
    DWORD size = MAX_PATH;
    if (QueryFullProcessImageNameW(hProcess, 0, processPath, &size)) {
        CloseHandle(hProcess);
        return QString::fromWCharArray(processPath);
    }
    
    qDebug() << "获取进程路径失败，PID:" << processId << "错误码:" << GetLastError();
    CloseHandle(hProcess);
    return QString();
#else
    Q_UNUSED(processId);
    return QString();
#endif
}

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
    
    qDebug() << "----------------------------------------";
    qDebug() << "定时器触发 - 检查窗口变化";
    qDebug() << "当前窗口:" << m_currentWindow.processName << "-" << m_currentWindow.windowTitle;
    qDebug() << "新窗口:" << newWindow.processName << "-" << newWindow.windowTitle;
    
    if (newWindow.processName != m_currentWindow.processName ||
        newWindow.windowTitle != m_currentWindow.windowTitle) {
        
        qDebug() << ">>> 检测到窗口变化! <<<";
        
        if (!m_currentWindow.processName.isEmpty()) {
            ActivityRecord record;
            record.processName = m_currentWindow.processName;
            record.windowTitle = m_currentWindow.windowTitle;
            record.filePath = m_currentWindow.filePath;
            record.startTime = m_windowStartTime;
            record.endTime = QDateTime::currentDateTime();
            record.durationSeconds = m_windowStartTime.secsTo(record.endTime);
            
            qDebug() << "生成活动记录:";
            qDebug() << "  进程:" << record.processName;
            qDebug() << "  标题:" << record.windowTitle;
            qDebug() << "  开始时间:" << record.startTime.toString("yyyy-MM-dd hh:mm:ss");
            qDebug() << "  结束时间:" << record.endTime.toString("yyyy-MM-dd hh:mm:ss");
            qDebug() << "  持续时间:" << record.durationSeconds << "秒";
            
            emit activityRecorded(record);
            qDebug() << "activityRecorded 信号已发送";
        }
        
        emit windowChanged(m_currentWindow, newWindow);
        qDebug() << "windowChanged 信号已发送";
        
        m_currentWindow = newWindow;
        m_windowStartTime = QDateTime::currentDateTime();
        
        qDebug() << "已更新当前窗口信息";
    } else {
        qDebug() << "窗口未变化，继续监控...";
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
        qDebug() << "打开进程失败，PID:" << processId << "错误码:" << GetLastError();
        return QString();
    }
    
    wchar_t processName[MAX_PATH];
    if (GetModuleBaseNameW(hProcess, nullptr, processName, MAX_PATH)) {
        CloseHandle(hProcess);
        return QString::fromWCharArray(processName);
    }
    
    qDebug() << "获取进程名失败，PID:" << processId << "错误码:" << GetLastError();
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

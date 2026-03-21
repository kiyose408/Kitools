#include "timetrackercontroller.h"
#include <QDebug>

TimeTrackerController::TimeTrackerController(QObject* parent)
    : QObject(parent)
    , m_monitor(nullptr)
    , m_manager(nullptr)
    , m_settingsPanel(nullptr)
{
    qDebug() << "TimeTrackerController 初始化";
    
    m_monitor = WindowMonitor::instance(this);
    m_manager = ActivityManager::instance(this);
    m_settingsPanel = new TimeTrackerSettingsPanel();
    
    if (m_settingsPanel) {
        connect(m_settingsPanel, &TimeTrackerSettingsPanel::backRequested, this, &TimeTrackerController::deactivated);
        connect(m_settingsPanel, &TimeTrackerSettingsPanel::startTrackingRequested, this, &TimeTrackerController::trackingStarted);
        connect(m_settingsPanel, &TimeTrackerSettingsPanel::stopTrackingRequested, this, &TimeTrackerController::trackingStopped);
    }
    
    qDebug() << "TimeTrackerController 初始化完成";
}

TimeTrackerController::~TimeTrackerController() {
    qDebug() << "TimeTrackerController 析构";
    if (m_settingsPanel) {
        m_settingsPanel->deleteLater();
    }
}

QWidget* TimeTrackerController::getSettingsPanel() {
    return m_settingsPanel;
}

void TimeTrackerController::startTracking() {
    if (m_monitor) {
        m_monitor->startMonitoring(1000);
        emit trackingStarted();
    }
}

void TimeTrackerController::stopTracking() {
    if (m_monitor) {
        m_monitor->stopMonitoring();
        emit trackingStopped();
    }
}

bool TimeTrackerController::isTracking() const {
    return m_monitor ? m_monitor->isMonitoring() : false;
}

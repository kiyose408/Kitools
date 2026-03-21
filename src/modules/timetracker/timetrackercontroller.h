#ifndef TIMETRACKERCONTROLLER_H
#define TIMETRACKERCONTROLLER_H

#include <QObject>
#include <QWidget>

#include "windowmonitor.h"
#include "activitymanager.h"
#include "timetrackersettingspanel.h"

class TimeTrackerController : public QObject {
    Q_OBJECT

public:
    explicit TimeTrackerController(QObject* parent = nullptr);
    ~TimeTrackerController();

    QWidget* getSettingsPanel();
    void startTracking();
    void stopTracking();
    bool isTracking() const;

signals:
    void activated();
    void deactivated();
    void trackingStarted();
    void trackingStopped();

private:
    WindowMonitor* m_monitor;
    ActivityManager* m_manager;
    TimeTrackerSettingsPanel* m_settingsPanel;
};

#endif

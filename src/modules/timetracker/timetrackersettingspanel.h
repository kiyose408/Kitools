#ifndef TIMETRACKERSETTINGSPANEL_H
#define TIMETRACKERSETTINGSPANEL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QDateEdit>

#include "timetrackerdatatypes.h"

class TimeTrackerSettingsPanel : public QWidget {
    Q_OBJECT

public:
    explicit TimeTrackerSettingsPanel(QWidget* parent = nullptr);
    ~TimeTrackerSettingsPanel();

signals:
    void backRequested();
    void startTrackingRequested();
    void stopTrackingRequested();

private slots:
    void onStartStopClicked();
    void onClearTodayClicked();
    void onClearAllClicked();
    void onDateChanged(const QDate& date);
    void onTrackingStateChanged(bool isTracking);
    void onActivityRecorded(const ActivityRecord& record);

private:
    void setupUI();
    void updateTrackingButton();
    void updateTodaySummary();
    QString formatDuration(int seconds) const;

    class WindowMonitor* m_monitor;
    class ActivityManager* m_manager;
    
    QPushButton* m_startStopBtn;
    QLabel* m_statusLabel;
    QLabel* m_todayTotalLabel;
    QDateEdit* m_dateEdit;
    QCheckBox* m_autoStartCheck;
    QSpinBox* m_intervalSpinBox;
};

#endif

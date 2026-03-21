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
#include <QListWidget>
#include <QProgressBar>
#include <QTabWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QDialog>

#include "timetrackerdatatypes.h"

class AddRuleDialog : public QDialog {
    Q_OBJECT

public:
    explicit AddRuleDialog(QWidget* parent = nullptr);
    ~AddRuleDialog();

    QString getRuleName() const;
    ActivityCategory getCategory() const;
    QString getProcessPattern() const;

private:
    QLineEdit* m_nameEdit;
    QComboBox* m_categoryCombo;
    QLineEdit* m_processEdit;
};

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
    void onTabChanged(int index);
    void onAddRuleClicked();
    void onDeleteRuleClicked();
    void onRuleSelectionChanged();

private:
    void setupUI();
    void setupTrackingTab();
    void setupStatsTab();
    void setupCategoryTab();
    void updateTrackingButton();
    void updateTodaySummary();
    void updateCategoryStats();
    void updateAppRanking();
    void refreshRuleList();
    QString formatDuration(int seconds) const;

    class WindowMonitor* m_monitor;
    class ActivityManager* m_manager;
    
    QTabWidget* m_tabWidget;
    
    QPushButton* m_startStopBtn;
    QLabel* m_statusLabel;
    QLabel* m_todayTotalLabel;
    QDateEdit* m_dateEdit;
    QCheckBox* m_autoStartCheck;
    QSpinBox* m_intervalSpinBox;
    
    QLabel* m_categoryStatsLabel;
    QListWidget* m_appRankingList;
    QProgressBar* m_goalProgress;
    QLabel* m_goalLabel;
    
    QListWidget* m_ruleList;
    QPushButton* m_addRuleBtn;
    QPushButton* m_deleteRuleBtn;
};

#endif

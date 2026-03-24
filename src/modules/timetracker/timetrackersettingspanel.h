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
#include <QTextEdit>

#include "timetrackerdatatypes.h"

class RuleEditDialog : public QDialog {
    Q_OBJECT

public:
    explicit RuleEditDialog(bool isAppRule, QWidget* parent = nullptr);
    ~RuleEditDialog();

    QString getRuleName() const;
    ActivityCategory getAppCategory() const;
    BrowserSubCategory getBrowserCategory() const;
    QString getProcessPatterns() const;
    QString getTitlePatterns() const;
    QString getDomains() const;
    QString getKeywords() const;

    void setRuleName(const QString& name);
    void setAppCategory(ActivityCategory category);
    void setBrowserCategory(BrowserSubCategory category);
    void setProcessPatterns(const QStringList& patterns);
    void setTitlePatterns(const QStringList& patterns);
    void setDomains(const QStringList& domains);
    void setKeywords(const QStringList& keywords);

private:
    bool m_isAppRule;
    QLineEdit* m_nameEdit;
    QComboBox* m_categoryCombo;
    QTextEdit* m_patternsEdit;
    QStringList m_currentPatterns;
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
    void onAddAppRuleClicked();
    void onEditAppRuleClicked();
    void onDeleteAppRuleClicked();
    void onAppRuleSelectionChanged();
    void onAddBrowserRuleClicked();
    void onEditBrowserRuleClicked();
    void onDeleteBrowserRuleClicked();
    void onBrowserRuleSelectionChanged();

private:
    void setupUI();
    void setupTrackingTab();
    void setupStatsTab();
    void setupCategoryTab();
    void updateTrackingButton();
    void updateTodaySummary();
    void updateCategoryStats();
    void updateAppRanking();
    void refreshAppRuleList();
    void refreshBrowserRuleList();
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
    
    QListWidget* m_appRuleList;
    QPushButton* m_addAppRuleBtn;
    QPushButton* m_editAppRuleBtn;
    QPushButton* m_deleteAppRuleBtn;
    
    QListWidget* m_browserRuleList;
    QPushButton* m_addBrowserRuleBtn;
    QPushButton* m_editBrowserRuleBtn;
    QPushButton* m_deleteBrowserRuleBtn;
};

#endif

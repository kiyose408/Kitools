#include "timetrackersettingspanel.h"
#include "windowmonitor.h"
#include "activitymanager.h"
#include <QDebug>
#include <QMessageBox>
#include <QCalendarWidget>

TimeTrackerSettingsPanel::TimeTrackerSettingsPanel(QWidget* parent)
    : QWidget(parent)
    , m_monitor(nullptr)
    , m_manager(nullptr)
    , m_startStopBtn(nullptr)
    , m_statusLabel(nullptr)
    , m_todayTotalLabel(nullptr)
    , m_dateEdit(nullptr)
    , m_autoStartCheck(nullptr)
    , m_intervalSpinBox(nullptr)
{
    qDebug() << "TimeTrackerSettingsPanel 初始化";
    
    m_monitor = WindowMonitor::instance(this);
    m_manager = ActivityManager::instance(this);
    
    setupUI();
    
    connect(m_monitor, &WindowMonitor::activityRecorded, this, &TimeTrackerSettingsPanel::onActivityRecorded);
    connect(m_monitor, &WindowMonitor::windowChanged, this, [this](const WindowInfo& old, const WindowInfo& newWindow) {
        Q_UNUSED(old);
        Q_UNUSED(newWindow);
        updateTrackingButton();
    });
}

TimeTrackerSettingsPanel::~TimeTrackerSettingsPanel() {
}

void TimeTrackerSettingsPanel::setupUI() {
    setWindowTitle("时间追踪器设置");
    setMinimumSize(500, 600);
    setStyleSheet("QWidget { background-color: #f5f6fa; }");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    QLabel* titleLabel = new QLabel("时间追踪器", this);
    titleLabel->setStyleSheet("QLabel { font-size: 18px; font-weight: bold; color: #2c3e50; }");
    mainLayout->addWidget(titleLabel);

    QGroupBox* statusGroup = new QGroupBox("追踪状态", this);
    statusGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #2c3e50; }");
    
    QVBoxLayout* statusLayout = new QVBoxLayout(statusGroup);
    
    QHBoxLayout* btnLayout = new QHBoxLayout();
    m_startStopBtn = new QPushButton("开始追踪", this);
    m_startStopBtn->setStyleSheet("QPushButton { padding: 10px 20px; background-color: #27ae60; color: white; border: none; border-radius: 5px; }");
    connect(m_startStopBtn, &QPushButton::clicked, this, &TimeTrackerSettingsPanel::onStartStopClicked);
    btnLayout->addWidget(m_startStopBtn);
    
    m_statusLabel = new QLabel("状态: 未启动", this);
    m_statusLabel->setStyleSheet("QLabel { color: #7f8c8d; }");
    btnLayout->addWidget(m_statusLabel);
    statusLayout->addLayout(btnLayout);
    
    QHBoxLayout* intervalLayout = new QHBoxLayout();
    QLabel* intervalLabel = new QLabel("监控间隔(秒):", this);
    m_intervalSpinBox = new QSpinBox(this);
    m_intervalSpinBox->setRange(1, 10);
    m_intervalSpinBox->setValue(1);
    intervalLayout->addWidget(intervalLabel);
    intervalLayout->addWidget(m_intervalSpinBox);
    statusLayout->addLayout(intervalLayout);
    
    m_autoStartCheck = new QCheckBox("开机自动启动追踪", this);
    statusLayout->addWidget(m_autoStartCheck);
    
    mainLayout->addWidget(statusGroup);

    QGroupBox* summaryGroup = new QGroupBox("今日统计", this);
    summaryGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #2c3e50; }");
    
    QVBoxLayout* summaryLayout = new QVBoxLayout(summaryGroup);
    
    m_todayTotalLabel = new QLabel("今日总时长: 0秒", this);
    m_todayTotalLabel->setStyleSheet("QLabel { color: #2c3e50; }");
    summaryLayout->addWidget(m_todayTotalLabel);
    
    QHBoxLayout* dateLayout = new QHBoxLayout();
    QLabel* dateLabel = new QLabel("查看日期:", this);
    m_dateEdit = new QDateEdit(this);
    m_dateEdit->setDate(QDate::currentDate());
    m_dateEdit->setCalendarPopup(true);
    connect(m_dateEdit, &QDateEdit::dateChanged, this, &TimeTrackerSettingsPanel::onDateChanged);
    dateLayout->addWidget(dateLabel);
    dateLayout->addWidget(m_dateEdit);
    summaryLayout->addLayout(dateLayout);
    
    mainLayout->addWidget(summaryGroup);

    QGroupBox* dataGroup = new QGroupBox("数据管理", this);
    dataGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #2c3e50; }");
    
    QVBoxLayout* dataLayout = new QVBoxLayout(dataGroup);
    
    QPushButton* clearTodayBtn = new QPushButton("清除今日数据", this);
    clearTodayBtn->setStyleSheet("QPushButton { padding: 8px 15px; background-color: #e74c3c; color: white; border: none; border-radius: 5px; }");
    connect(clearTodayBtn, &QPushButton::clicked, this, &TimeTrackerSettingsPanel::onClearTodayClicked);
    dataLayout->addWidget(clearTodayBtn);
    
    QPushButton* clearAllBtn = new QPushButton("清除所有数据", this);
    clearAllBtn->setStyleSheet("QPushButton { padding: 8px 15px; background-color: #c0392b; color: white; border: none; border-radius: 5px; }");
    connect(clearAllBtn, &QPushButton::clicked, this, &TimeTrackerSettingsPanel::onClearAllClicked);
    dataLayout->addWidget(clearAllBtn);
    
    mainLayout->addWidget(dataGroup);

    mainLayout->addStretch();

    QPushButton* backBtn = new QPushButton("返回", this);
    backBtn->setStyleSheet("QPushButton { padding: 10px 20px; background-color: #95a5a6; color: white; border: none; border-radius: 5px; }");
    connect(backBtn, &QPushButton::clicked, this, &TimeTrackerSettingsPanel::backRequested);
    mainLayout->addWidget(backBtn);

    updateTrackingButton();
    updateTodaySummary();
}

void TimeTrackerSettingsPanel::onStartStopClicked() {
    if (m_monitor->isMonitoring()) {
        m_monitor->stopMonitoring();
        emit stopTrackingRequested();
    } else {
        int interval = m_intervalSpinBox->value();
        m_monitor->startMonitoring(interval * 1000);
        emit startTrackingRequested();
    }
    updateTrackingButton();
}

void TimeTrackerSettingsPanel::onClearTodayClicked() {
    QMessageBox::StandardButton reply = QMessageBox::question(this, "确认清除", "确定要清除今日的所有追踪数据吗？", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        m_manager->clearRecords(QDate::currentDate());
        updateTodaySummary();
    }
}

void TimeTrackerSettingsPanel::onClearAllClicked() {
    QMessageBox::StandardButton reply = QMessageBox::question(this, "确认清除", "确定要清除所有追踪数据吗？此操作不可恢复！", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        m_manager->clearAllRecords();
        updateTodaySummary();
    }
}

void TimeTrackerSettingsPanel::onDateChanged(const QDate& date) {
    Q_UNUSED(date);
    updateTodaySummary();
}

void TimeTrackerSettingsPanel::onTrackingStateChanged(bool isTracking) {
    Q_UNUSED(isTracking);
    updateTrackingButton();
}

void TimeTrackerSettingsPanel::onActivityRecorded(const ActivityRecord& record) {
    Q_UNUSED(record);
    updateTodaySummary();
}

void TimeTrackerSettingsPanel::updateTrackingButton() {
    if (!m_startStopBtn || !m_monitor) return;
    
    if (m_monitor->isMonitoring()) {
        m_startStopBtn->setText("停止追踪");
        m_startStopBtn->setStyleSheet("QPushButton { padding: 10px 20px; background-color: #e74c3c; color: white; border: none; border-radius: 5px; }");
        m_statusLabel->setText("状态: 追踪中...");
        m_statusLabel->setStyleSheet("QLabel { color: #27ae60; }");
    } else {
        m_startStopBtn->setText("开始追踪");
        m_startStopBtn->setStyleSheet("QPushButton { padding: 10px 20px; background-color: #27ae60; color: white; border: none; border-radius: 5px; }");
        m_statusLabel->setText("状态: 未启动");
        m_statusLabel->setStyleSheet("QLabel { color: #7f8c8d; }");
    }
}

void TimeTrackerSettingsPanel::updateTodaySummary() {
    if (!m_todayTotalLabel || !m_manager) return;
    
    QDate date = m_dateEdit ? m_dateEdit->date() : QDate::currentDate();
    qint64 totalSeconds = m_manager->getTotalDurationSeconds(date);
    
    m_todayTotalLabel->setText(QString("今日总时长: %1").arg(formatDuration(totalSeconds)));
}

QString TimeTrackerSettingsPanel::formatDuration(int seconds) const {
    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    int secs = seconds % 60;
    
    if (hours > 0) {
        return QString("%1小时%2分钟").arg(hours).arg(minutes);
    } else if (minutes > 0) {
        return QString("%1分钟%2秒").arg(minutes).arg(secs);
    } else {
        return QString("%1秒").arg(secs);
    }
}

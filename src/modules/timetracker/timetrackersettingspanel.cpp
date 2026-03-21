#include "timetrackersettingspanel.h"
#include "windowmonitor.h"
#include "activitymanager.h"
#include <QDebug>
#include <QMessageBox>
#include <QCalendarWidget>
#include <QDialogButtonBox>
#include <QFormLayout>

AddRuleDialog::AddRuleDialog(QWidget* parent)
    : QDialog(parent)
    , m_nameEdit(nullptr)
    , m_categoryCombo(nullptr)
    , m_processEdit(nullptr)
{
    setWindowTitle("添加分类规则");
    setMinimumWidth(400);
    
    QFormLayout* layout = new QFormLayout(this);
    
    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setPlaceholderText("输入规则名称");
    layout->addRow("规则名称:", m_nameEdit);
    
    m_categoryCombo = new QComboBox(this);
    m_categoryCombo->addItem("开发工具", static_cast<int>(ActivityCategory::Development));
    m_categoryCombo->addItem("浏览器", static_cast<int>(ActivityCategory::Browser));
    m_categoryCombo->addItem("办公软件", static_cast<int>(ActivityCategory::Office));
    m_categoryCombo->addItem("社交通讯", static_cast<int>(ActivityCategory::Communication));
    m_categoryCombo->addItem("多媒体", static_cast<int>(ActivityCategory::Media));
    m_categoryCombo->addItem("游戏", static_cast<int>(ActivityCategory::Game));
    m_categoryCombo->addItem("系统工具", static_cast<int>(ActivityCategory::System));
    m_categoryCombo->addItem("其他", static_cast<int>(ActivityCategory::Other));
    layout->addRow("分类:", m_categoryCombo);
    
    m_processEdit = new QLineEdit(this);
    m_processEdit->setPlaceholderText("输入进程名关键字，如: chrome, wechat");
    layout->addRow("进程名:", m_processEdit);
    
    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addRow(buttons);
}

AddRuleDialog::~AddRuleDialog() {
}

QString AddRuleDialog::getRuleName() const {
    return m_nameEdit ? m_nameEdit->text().trimmed() : QString();
}

ActivityCategory AddRuleDialog::getCategory() const {
    return m_categoryCombo ? static_cast<ActivityCategory>(m_categoryCombo->currentData().toInt()) : ActivityCategory::Other;
}

QString AddRuleDialog::getProcessPattern() const {
    return m_processEdit ? m_processEdit->text().trimmed().toLower() : QString();
}

TimeTrackerSettingsPanel::TimeTrackerSettingsPanel(QWidget* parent)
    : QWidget(parent)
    , m_monitor(nullptr)
    , m_manager(nullptr)
    , m_tabWidget(nullptr)
    , m_startStopBtn(nullptr)
    , m_statusLabel(nullptr)
    , m_todayTotalLabel(nullptr)
    , m_dateEdit(nullptr)
    , m_autoStartCheck(nullptr)
    , m_intervalSpinBox(nullptr)
    , m_categoryStatsLabel(nullptr)
    , m_appRankingList(nullptr)
    , m_goalProgress(nullptr)
    , m_goalLabel(nullptr)
    , m_ruleList(nullptr)
    , m_addRuleBtn(nullptr)
    , m_deleteRuleBtn(nullptr)
{
    qDebug() << "========== TimeTrackerSettingsPanel 初始化 ==========";
    
    m_monitor = WindowMonitor::instance(this);
    m_manager = ActivityManager::instance(this);
    
    if (!m_monitor) {
        qCritical() << "WindowMonitor 初始化失败！";
    }
    if (!m_manager) {
        qCritical() << "ActivityManager 初始化失败！";
    }
    
    setupUI();
    
    if (m_monitor) {
        connect(m_monitor, &WindowMonitor::activityRecorded, this, &TimeTrackerSettingsPanel::onActivityRecorded);
        qDebug() << "已连接 WindowMonitor 信号";
    } else {
        qCritical() << "无法连接 WindowMonitor 信号，监控功能将不可用！";
    }
    
    qDebug() << "TimeTrackerSettingsPanel 初始化完成";
}

TimeTrackerSettingsPanel::~TimeTrackerSettingsPanel() {
}

void TimeTrackerSettingsPanel::setupUI() {
    setWindowTitle("时间追踪器设置");
    setMinimumSize(600, 700);
    setStyleSheet("QWidget { background-color: #f5f6fa; }");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    QLabel* titleLabel = new QLabel("时间追踪器", this);
    titleLabel->setStyleSheet("QLabel { font-size: 18px; font-weight: bold; color: #2c3e50; }");
    mainLayout->addWidget(titleLabel);

    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setStyleSheet("QTabWidget::pane { border: 1px solid #dcdde1; border-radius: 5px; background-color: white; }");
    connect(m_tabWidget, &QTabWidget::currentChanged, this, &TimeTrackerSettingsPanel::onTabChanged);
    
    setupTrackingTab();
    setupStatsTab();
    setupCategoryTab();
    
    mainLayout->addWidget(m_tabWidget);

    QPushButton* backBtn = new QPushButton("返回", this);
    backBtn->setStyleSheet("QPushButton { padding: 10px 20px; background-color: #95a5a6; color: white; border: none; border-radius: 5px; }");
    connect(backBtn, &QPushButton::clicked, this, &TimeTrackerSettingsPanel::backRequested);
    mainLayout->addWidget(backBtn);

    updateTrackingButton();
    updateTodaySummary();
}

void TimeTrackerSettingsPanel::setupTrackingTab() {
    QWidget* trackingTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(trackingTab);
    layout->setSpacing(15);

    QGroupBox* statusGroup = new QGroupBox("追踪状态", trackingTab);
    statusGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #2c3e50; }");
    
    QVBoxLayout* statusLayout = new QVBoxLayout(statusGroup);
    
    QHBoxLayout* btnLayout = new QHBoxLayout();
    m_startStopBtn = new QPushButton("开始追踪", statusGroup);
    m_startStopBtn->setStyleSheet("QPushButton { padding: 10px 20px; background-color: #27ae60; color: white; border: none; border-radius: 5px; }");
    connect(m_startStopBtn, &QPushButton::clicked, this, &TimeTrackerSettingsPanel::onStartStopClicked);
    btnLayout->addWidget(m_startStopBtn);
    
    m_statusLabel = new QLabel("状态: 未启动", statusGroup);
    m_statusLabel->setStyleSheet("QLabel { color: #7f8c8d; }");
    btnLayout->addWidget(m_statusLabel);
    statusLayout->addLayout(btnLayout);
    
    QHBoxLayout* intervalLayout = new QHBoxLayout();
    QLabel* intervalLabel = new QLabel("监控间隔(秒):", statusGroup);
    m_intervalSpinBox = new QSpinBox(statusGroup);
    m_intervalSpinBox->setRange(1, 10);
    m_intervalSpinBox->setValue(1);
    intervalLayout->addWidget(intervalLabel);
    intervalLayout->addWidget(m_intervalSpinBox);
    statusLayout->addLayout(intervalLayout);
    
    m_autoStartCheck = new QCheckBox("开机自动启动追踪", statusGroup);
    statusLayout->addWidget(m_autoStartCheck);
    
    layout->addWidget(statusGroup);

    QGroupBox* summaryGroup = new QGroupBox("今日统计", trackingTab);
    summaryGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #2c3e50; }");
    
    QVBoxLayout* summaryLayout = new QVBoxLayout(summaryGroup);
    
    m_todayTotalLabel = new QLabel("今日总时长: 0秒", summaryGroup);
    m_todayTotalLabel->setStyleSheet("QLabel { color: #2c3e50; font-size: 14px; }");
    summaryLayout->addWidget(m_todayTotalLabel);
    
    QHBoxLayout* dateLayout = new QHBoxLayout();
    QLabel* dateLabel = new QLabel("查看日期:", summaryGroup);
    m_dateEdit = new QDateEdit(summaryGroup);
    m_dateEdit->setDate(QDate::currentDate());
    m_dateEdit->setCalendarPopup(true);
    connect(m_dateEdit, &QDateEdit::dateChanged, this, &TimeTrackerSettingsPanel::onDateChanged);
    dateLayout->addWidget(dateLabel);
    dateLayout->addWidget(m_dateEdit);
    summaryLayout->addLayout(dateLayout);
    
    layout->addWidget(summaryGroup);

    QGroupBox* goalGroup = new QGroupBox("每日目标", trackingTab);
    goalGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #2c3e50; }");
    
    QVBoxLayout* goalLayout = new QVBoxLayout(goalGroup);
    m_goalLabel = new QLabel("目标: 8小时", goalGroup);
    goalLayout->addWidget(m_goalLabel);
    
    m_goalProgress = new QProgressBar(goalGroup);
    m_goalProgress->setRange(0, 100);
    m_goalProgress->setValue(0);
    m_goalProgress->setFormat("%p%");
    m_goalProgress->setStyleSheet("QProgressBar { border: 1px solid #dcdde1; border-radius: 5px; text-align: center; }");
    goalLayout->addWidget(m_goalProgress);
    
    layout->addWidget(goalGroup);

    QGroupBox* dataGroup = new QGroupBox("数据管理", trackingTab);
    dataGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #2c3e50; }");
    
    QVBoxLayout* dataLayout = new QVBoxLayout(dataGroup);
    
    QPushButton* clearTodayBtn = new QPushButton("清除今日数据", dataGroup);
    clearTodayBtn->setStyleSheet("QPushButton { padding: 8px 15px; background-color: #e74c3c; color: white; border: none; border-radius: 5px; }");
    connect(clearTodayBtn, &QPushButton::clicked, this, &TimeTrackerSettingsPanel::onClearTodayClicked);
    dataLayout->addWidget(clearTodayBtn);
    
    QPushButton* clearAllBtn = new QPushButton("清除所有数据", dataGroup);
    clearAllBtn->setStyleSheet("QPushButton { padding: 8px 15px; background-color: #c0392b; color: white; border: none; border-radius: 5px; }");
    connect(clearAllBtn, &QPushButton::clicked, this, &TimeTrackerSettingsPanel::onClearAllClicked);
    dataLayout->addWidget(clearAllBtn);
    
    layout->addWidget(dataGroup);
    layout->addStretch();

    m_tabWidget->addTab(trackingTab, "追踪控制");
}

void TimeTrackerSettingsPanel::setupStatsTab() {
    QWidget* statsTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(statsTab);
    layout->setSpacing(15);

    QGroupBox* categoryGroup = new QGroupBox("分类统计", statsTab);
    categoryGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #2c3e50; }");
    
    QVBoxLayout* categoryLayout = new QVBoxLayout(categoryGroup);
    m_categoryStatsLabel = new QLabel("暂无数据", categoryGroup);
    m_categoryStatsLabel->setStyleSheet("QLabel { color: #2c3e50; }");
    m_categoryStatsLabel->setWordWrap(true);
    categoryLayout->addWidget(m_categoryStatsLabel);
    
    layout->addWidget(categoryGroup);

    QGroupBox* rankingGroup = new QGroupBox("应用使用排行", statsTab);
    rankingGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #2c3e50; }");
    
    QVBoxLayout* rankingLayout = new QVBoxLayout(rankingGroup);
    m_appRankingList = new QListWidget(rankingGroup);
    m_appRankingList->setStyleSheet("QListWidget { background-color: white; border: 1px solid #dcdde1; border-radius: 5px; }");
    rankingLayout->addWidget(m_appRankingList);
    
    layout->addWidget(rankingGroup);
    layout->addStretch();

    m_tabWidget->addTab(statsTab, "统计报告");
}

void TimeTrackerSettingsPanel::setupCategoryTab() {
    QWidget* categoryTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(categoryTab);
    layout->setSpacing(15);

    QLabel* infoLabel = new QLabel("应用分类规则", categoryTab);
    infoLabel->setStyleSheet("QLabel { font-size: 14px; font-weight: bold; color: #2c3e50; }");
    layout->addWidget(infoLabel);

    QLabel* descLabel = new QLabel("可以添加自定义规则来分类应用程序：", categoryTab);
    descLabel->setStyleSheet("QLabel { color: #7f8c8d; }");
    descLabel->setWordWrap(true);
    layout->addWidget(descLabel);

    QGroupBox* customGroup = new QGroupBox("自定义规则", categoryTab);
    customGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #2c3e50; }");
    
    QVBoxLayout* customLayout = new QVBoxLayout(customGroup);
    
    m_ruleList = new QListWidget(customGroup);
    m_ruleList->setStyleSheet("QListWidget { background-color: white; border: 1px solid #dcdde1; border-radius: 5px; min-height: 150px; }");
    connect(m_ruleList, &QListWidget::itemSelectionChanged, this, &TimeTrackerSettingsPanel::onRuleSelectionChanged);
    customLayout->addWidget(m_ruleList);
    
    QHBoxLayout* btnLayout = new QHBoxLayout();
    m_addRuleBtn = new QPushButton("添加规则", customGroup);
    m_addRuleBtn->setStyleSheet("QPushButton { padding: 8px 15px; background-color: #3498db; color: white; border: none; border-radius: 5px; }");
    connect(m_addRuleBtn, &QPushButton::clicked, this, &TimeTrackerSettingsPanel::onAddRuleClicked);
    btnLayout->addWidget(m_addRuleBtn);
    
    m_deleteRuleBtn = new QPushButton("删除规则", customGroup);
    m_deleteRuleBtn->setStyleSheet("QPushButton { padding: 8px 15px; background-color: #e74c3c; color: white; border: none; border-radius: 5px; }");
    m_deleteRuleBtn->setEnabled(false);
    connect(m_deleteRuleBtn, &QPushButton::clicked, this, &TimeTrackerSettingsPanel::onDeleteRuleClicked);
    btnLayout->addWidget(m_deleteRuleBtn);
    
    customLayout->addLayout(btnLayout);
    layout->addWidget(customGroup);

    QGroupBox* presetGroup = new QGroupBox("预设规则（只读）", categoryTab);
    presetGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #2c3e50; }");
    
    QVBoxLayout* presetLayout = new QVBoxLayout(presetGroup);
    
    if (m_manager) {
        QList<AppCategoryRule> rules = m_manager->getCategoryRules();
        for (const AppCategoryRule& rule : rules) {
            QString patterns;
            for (const QString& p : rule.processPatterns) {
                if (!patterns.isEmpty()) patterns += ", ";
                patterns += p;
            }
            
            QLabel* ruleLabel = new QLabel(QString("%1: %2").arg(categoryToString(rule.category)).arg(patterns), presetGroup);
            ruleLabel->setStyleSheet("QLabel { color: #7f8c8d; font-size: 11px; }");
            ruleLabel->setWordWrap(true);
            presetLayout->addWidget(ruleLabel);
        }
    }
    
    layout->addWidget(presetGroup);

    layout->addStretch();
    m_tabWidget->addTab(categoryTab, "分类规则");
    
    refreshRuleList();
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
    updateCategoryStats();
    updateAppRanking();
}

void TimeTrackerSettingsPanel::onTrackingStateChanged(bool isTracking) {
    Q_UNUSED(isTracking);
    updateTrackingButton();
}

void TimeTrackerSettingsPanel::onActivityRecorded(const ActivityRecord& record) {
    qDebug() << "========== TimeTrackerSettingsPanel 收到活动记录 ==========";
    qDebug() << "进程:" << record.processName;
    qDebug() << "标题:" << record.windowTitle;
    qDebug() << "持续时间:" << record.durationSeconds << "秒";
    
    if (!m_manager) {
        qCritical() << "ActivityManager 为空，尝试重新获取实例...";
        m_manager = ActivityManager::instance(this);
    }
    
    if (m_manager) {
        qDebug() << "将记录添加到 ActivityManager...";
        m_manager->addActivityRecord(record);
        qDebug() << "记录已保存到 ActivityManager";
    } else {
        qCritical() << "无法保存活动记录！ActivityManager 初始化失败！";
        qCritical() << "丢失的记录 - 进程:" << record.processName << "持续时间:" << record.durationSeconds << "秒";
        return;
    }
    
    updateTodaySummary();
    if (m_tabWidget && m_tabWidget->currentIndex() == 1) {
        updateCategoryStats();
        updateAppRanking();
    }
    
    qDebug() << "UI 已更新";
}

void TimeTrackerSettingsPanel::onTabChanged(int index) {
    if (index == 1) {
        updateCategoryStats();
        updateAppRanking();
    }
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
    
    if (m_goalProgress) {
        int goalSeconds = 8 * 3600;
        int progress = qMin(100, static_cast<int>(totalSeconds * 100 / goalSeconds));
        m_goalProgress->setValue(progress);
    }
}

void TimeTrackerSettingsPanel::updateCategoryStats() {
    if (!m_categoryStatsLabel || !m_manager) return;
    
    QDate date = m_dateEdit ? m_dateEdit->date() : QDate::currentDate();
    DailySummary summary = m_manager->getDailySummary(date);
    
    QString statsText;
    statsText += QString("总时长: %1\n\n").arg(formatDuration(summary.totalSeconds));
    
    statsText += "分类统计:\n";
    for (auto it = summary.categorySeconds.begin(); it != summary.categorySeconds.end(); ++it) {
        if (it.value() > 0) {
            statsText += QString("  %1: %2\n").arg(categoryToString(it.key())).arg(formatDuration(it.value()));
        }
    }
    
    m_categoryStatsLabel->setText(statsText);
}

void TimeTrackerSettingsPanel::updateAppRanking() {
    if (!m_appRankingList || !m_manager) return;
    
    m_appRankingList->clear();
    
    QDate date = m_dateEdit ? m_dateEdit->date() : QDate::currentDate();
    DailySummary summary = m_manager->getDailySummary(date);
    
    QList<QPair<QString, int>> appList;
    for (auto it = summary.appSeconds.begin(); it != summary.appSeconds.end(); ++it) {
        appList.append(qMakePair(it.key(), it.value()));
    }
    
    std::sort(appList.begin(), appList.end(), [](const QPair<QString, int>& a, const QPair<QString, int>& b) {
        return a.second > b.second;
    });
    
    int rank = 1;
    for (const auto& item : appList) {
        if (rank > 10) break;
        
        QString displayText = QString("%1. %2 - %3").arg(rank).arg(item.first).arg(formatDuration(item.second));
        QListWidgetItem* listItem = new QListWidgetItem(displayText, m_appRankingList);
        listItem->setData(Qt::UserRole, item.first);
        m_appRankingList->addItem(listItem);
        rank++;
    }
    
    if (appList.isEmpty()) {
        m_appRankingList->addItem("暂无数据");
    }
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

void TimeTrackerSettingsPanel::onAddRuleClicked() {
    AddRuleDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QString name = dialog.getRuleName();
        ActivityCategory category = dialog.getCategory();
        QString pattern = dialog.getProcessPattern();
        
        if (name.isEmpty() || pattern.isEmpty()) {
            QMessageBox::warning(this, "输入错误", "请填写规则名称和进程名");
            return;
        }
        
        if (m_manager) {
            QList<AppCategoryRule> existingRules = m_manager->getCategoryRules();
            for (const AppCategoryRule& existingRule : existingRules) {
                if (existingRule.name == name) {
                    QMessageBox::StandardButton reply = QMessageBox::question(this, "规则已存在", 
                        QString("规则 \"%1\" 已存在，是否覆盖？").arg(name), 
                        QMessageBox::Yes | QMessageBox::No);
                    if (reply != QMessageBox::Yes) {
                        return;
                    }
                    break;
                }
            }
            
            AppCategoryRule rule;
            rule.name = name;
            rule.category = category;
            rule.processPatterns.append(pattern);
            m_manager->setCategoryRule(rule);
            refreshRuleList();
            QMessageBox::information(this, "成功", "规则添加成功！");
        }
    }
}

void TimeTrackerSettingsPanel::onDeleteRuleClicked() {
    if (!m_ruleList || !m_manager) return;
    
    QListWidgetItem* item = m_ruleList->currentItem();
    if (!item) return;
    
    QString ruleName = item->data(Qt::UserRole).toString();
    
    QMessageBox::StandardButton reply = QMessageBox::question(this, "确认删除", 
        QString("确定要删除规则 \"%1\" 吗？").arg(ruleName), 
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        m_manager->removeCategoryRule(ruleName);
        refreshRuleList();
    }
}

void TimeTrackerSettingsPanel::onRuleSelectionChanged() {
    if (m_deleteRuleBtn && m_ruleList) {
        m_deleteRuleBtn->setEnabled(m_ruleList->currentItem() != nullptr);
    }
}

void TimeTrackerSettingsPanel::refreshRuleList() {
    if (!m_ruleList || !m_manager) return;
    
    m_ruleList->clear();
    
    QList<AppCategoryRule> rules = m_manager->getCategoryRules();
    for (const AppCategoryRule& rule : rules) {
        QString patterns;
        for (const QString& p : rule.processPatterns) {
            if (!patterns.isEmpty()) patterns += ", ";
            patterns += p;
        }
        
        QString displayText = QString("%1 [%2]: %3").arg(rule.name).arg(categoryToString(rule.category)).arg(patterns);
        QListWidgetItem* item = new QListWidgetItem(displayText, m_ruleList);
        item->setData(Qt::UserRole, rule.name);
        m_ruleList->addItem(item);
    }
    
    if (rules.isEmpty()) {
        m_ruleList->addItem("暂无自定义规则");
    }
}

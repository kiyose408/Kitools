#include "todosettingspanel.h"
#include "desktoptodowidget.h"
#include "taskmanager.h"
#include "logmanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QColorDialog>
#include <QFontDatabase>
#include <QMessageBox>

TodoSettingsPanel::TodoSettingsPanel(QWidget *parent)
    : QWidget(parent)
    , m_todoWidget(nullptr)
    , m_showBtn(nullptr)
    , m_hideBtn(nullptr)
    , m_modeCombo(nullptr)
    , m_clearCompletedBtn(nullptr)
    , m_exportJsonBtn(nullptr)
    , m_exportCsvBtn(nullptr)
    , m_clearLogsBtn(nullptr)
    , m_backBtn(nullptr)
    , m_statsLabel(nullptr)
    , m_logCountLabel(nullptr)
    , m_bgColorBtn(nullptr)
    , m_bgOpacitySlider(nullptr)
    , m_bgOpacityLabel(nullptr)
    , m_fontCombo(nullptr)
    , m_backgroundColor(255, 255, 255)
{
    setupUi();
    setupConnections();
}

TodoSettingsPanel::~TodoSettingsPanel()
{
}

void TodoSettingsPanel::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(15);
    
    QHBoxLayout *headerLayout = new QHBoxLayout();
    m_backBtn = new QPushButton("← 返回", this);
    m_backBtn->setStyleSheet("QPushButton { background: none; border: none; color: #3498db; font-size: 14px; } QPushButton:hover { color: #2980b9; }");
    headerLayout->addWidget(m_backBtn);
    
    QLabel *titleLabel = new QLabel("桌面待办事项", this);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50;");
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    mainLayout->addLayout(headerLayout);
    
    QHBoxLayout *controlLayout = new QHBoxLayout();
    controlLayout->setSpacing(10);
    
    m_showBtn = new QPushButton("显示待办窗口", this);
    m_showBtn->setStyleSheet(
        "QPushButton { background-color: #27ae60; color: white; border: none; padding: 10px 20px; "
        "font-size: 14px; border-radius: 5px; }"
        "QPushButton:hover { background-color: #2ecc71; }"
    );
    controlLayout->addWidget(m_showBtn);
    
    m_hideBtn = new QPushButton("隐藏待办窗口", this);
    m_hideBtn->setStyleSheet(
        "QPushButton { background-color: #95a5a6; color: white; border: none; padding: 10px 20px; "
        "font-size: 14px; border-radius: 5px; }"
        "QPushButton:hover { background-color: #7f8c8d; }"
    );
    controlLayout->addWidget(m_hideBtn);
    
    controlLayout->addStretch();
    mainLayout->addLayout(controlLayout);
    
    QGroupBox *modeGroup = new QGroupBox("显示模式", this);
    modeGroup->setStyleSheet("QGroupBox { font-weight: bold; border: 1px solid #ddd; border-radius: 5px; margin-top: 10px; padding-top: 10px; }");
    QHBoxLayout *modeLayout = new QHBoxLayout(modeGroup);
    
    modeLayout->addWidget(new QLabel("窗口模式:", this));
    m_modeCombo = new QComboBox(this);
    m_modeCombo->addItem("置顶模式", static_cast<int>(DesktopTodoWidget::DisplayMode::AlwaysOnTop));
    m_modeCombo->addItem("桌面固定模式", static_cast<int>(DesktopTodoWidget::DisplayMode::DesktopFixed));
    m_modeCombo->setStyleSheet("QComboBox { padding: 5px; border: 1px solid #ddd; border-radius: 3px; }");
    modeLayout->addWidget(m_modeCombo);
    modeLayout->addStretch();
    mainLayout->addWidget(modeGroup);
    
    QGroupBox *appearanceGroup = new QGroupBox("外观设置", this);
    appearanceGroup->setStyleSheet("QGroupBox { font-weight: bold; border: 1px solid #ddd; border-radius: 5px; margin-top: 10px; padding-top: 10px; }");
    QGridLayout *appearanceLayout = new QGridLayout(appearanceGroup);
    appearanceLayout->setSpacing(10);
    
    appearanceLayout->addWidget(new QLabel("背景颜色:", this), 0, 0);
    m_bgColorBtn = new QPushButton(this);
    m_bgColorBtn->setFixedSize(60, 25);
    m_bgColorBtn->setStyleSheet(QString("background-color: %1; border: 1px solid #ccc; border-radius: 3px;").arg(m_backgroundColor.name()));
    appearanceLayout->addWidget(m_bgColorBtn, 0, 1);
    
    appearanceLayout->addWidget(new QLabel("背景透明度:", this), 1, 0);
    QHBoxLayout *opacityLayout = new QHBoxLayout();
    m_bgOpacitySlider = new QSlider(Qt::Horizontal, this);
    m_bgOpacitySlider->setRange(0, 255);
    m_bgOpacitySlider->setValue(230);
    m_bgOpacitySlider->setFixedWidth(100);
    opacityLayout->addWidget(m_bgOpacitySlider);
    m_bgOpacityLabel = new QLabel("230", this);
    m_bgOpacityLabel->setFixedWidth(30);
    opacityLayout->addWidget(m_bgOpacityLabel);
    appearanceLayout->addLayout(opacityLayout, 1, 1);
    
    appearanceLayout->addWidget(new QLabel("字体:", this), 2, 0);
    m_fontCombo = new QComboBox(this);
    m_fontCombo->addItem("默认");
    QFontDatabase fontDb;
    QStringList fonts = fontDb.families();
    for (const QString &font : fonts) {
        m_fontCombo->addItem(font);
    }
    m_fontCombo->setStyleSheet("QComboBox { padding: 5px; border: 1px solid #ddd; border-radius: 3px; min-width: 150px; }");
    appearanceLayout->addWidget(m_fontCombo, 2, 1);
    
    mainLayout->addWidget(appearanceGroup);
    
    QGroupBox *statsGroup = new QGroupBox("统计信息", this);
    statsGroup->setStyleSheet("QGroupBox { font-weight: bold; border: 1px solid #ddd; border-radius: 5px; margin-top: 10px; padding-top: 10px; }");
    QVBoxLayout *statsLayout = new QVBoxLayout(statsGroup);
    
    m_statsLabel = new QLabel("共 0 项任务，已完成 0 项", this);
    m_statsLabel->setStyleSheet("font-size: 14px; color: #2c3e50;");
    statsLayout->addWidget(m_statsLabel);
    
    m_logCountLabel = new QLabel("日志记录: 0 条", this);
    m_logCountLabel->setStyleSheet("font-size: 14px; color: #7f8c8d;");
    statsLayout->addWidget(m_logCountLabel);
    
    mainLayout->addWidget(statsGroup);
    
    QGroupBox *actionGroup = new QGroupBox("操作", this);
    actionGroup->setStyleSheet("QGroupBox { font-weight: bold; border: 1px solid #ddd; border-radius: 5px; margin-top: 10px; padding-top: 10px; }");
    QVBoxLayout *actionLayout = new QVBoxLayout(actionGroup);
    actionLayout->setSpacing(10);
    
    m_clearCompletedBtn = new QPushButton("清除已完成任务", this);
    m_clearCompletedBtn->setStyleSheet(
        "QPushButton { background-color: #e74c3c; color: white; border: none; padding: 8px 15px; "
        "font-size: 13px; border-radius: 5px; }"
        "QPushButton:hover { background-color: #c0392b; }"
    );
    actionLayout->addWidget(m_clearCompletedBtn);
    
    QHBoxLayout *exportLayout = new QHBoxLayout();
    m_exportJsonBtn = new QPushButton("导出日志(JSON)", this);
    m_exportJsonBtn->setStyleSheet(
        "QPushButton { background-color: #3498db; color: white; border: none; padding: 8px 15px; "
        "font-size: 13px; border-radius: 5px; }"
        "QPushButton:hover { background-color: #2980b9; }"
    );
    exportLayout->addWidget(m_exportJsonBtn);
    
    m_exportCsvBtn = new QPushButton("导出日志(CSV)", this);
    m_exportCsvBtn->setStyleSheet(
        "QPushButton { background-color: #3498db; color: white; border: none; padding: 8px 15px; "
        "font-size: 13px; border-radius: 5px; }"
        "QPushButton:hover { background-color: #2980b9; }"
    );
    exportLayout->addWidget(m_exportCsvBtn);
    exportLayout->addStretch();
    actionLayout->addLayout(exportLayout);
    
    m_clearLogsBtn = new QPushButton("重置日志", this);
    m_clearLogsBtn->setStyleSheet(
        "QPushButton { background-color: #e67e22; color: white; border: none; padding: 8px 15px; "
        "font-size: 13px; border-radius: 5px; }"
        "QPushButton:hover { background-color: #d35400; }"
    );
    actionLayout->addWidget(m_clearLogsBtn);
    
    mainLayout->addWidget(actionGroup);
    mainLayout->addStretch();
}

void TodoSettingsPanel::setupConnections()
{
    connect(m_showBtn, &QPushButton::clicked, this, &TodoSettingsPanel::onShowTodoClicked);
    connect(m_hideBtn, &QPushButton::clicked, this, &TodoSettingsPanel::onHideTodoClicked);
    connect(m_modeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TodoSettingsPanel::onModeChanged);
    connect(m_clearCompletedBtn, &QPushButton::clicked, this, &TodoSettingsPanel::onClearCompletedClicked);
    connect(m_exportJsonBtn, &QPushButton::clicked, this, &TodoSettingsPanel::onExportJsonClicked);
    connect(m_exportCsvBtn, &QPushButton::clicked, this, &TodoSettingsPanel::onExportCsvClicked);
    connect(m_clearLogsBtn, &QPushButton::clicked, this, &TodoSettingsPanel::onClearLogsClicked);
    connect(m_backBtn, &QPushButton::clicked, this, &TodoSettingsPanel::onBackClicked);
    connect(m_bgColorBtn, &QPushButton::clicked, this, &TodoSettingsPanel::onBackgroundColorClicked);
    connect(m_bgOpacitySlider, &QSlider::valueChanged, this, &TodoSettingsPanel::onBackgroundOpacityChanged);
    connect(m_fontCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TodoSettingsPanel::onFontChanged);
    
    LogManager *lm = LogManager::instance();
    connect(lm, &LogManager::logAdded, this, &TodoSettingsPanel::updateStats);
    connect(lm, &LogManager::logsCleared, this, &TodoSettingsPanel::updateStats);
    
    TaskManager *tm = TaskManager::instance();
    connect(tm, &TaskManager::taskAdded, this, [this](const TaskData &) { updateStats(); });
    connect(tm, &TaskManager::taskRemoved, this, [this](int) { updateStats(); });
    connect(tm, &TaskManager::taskCompleted, this, [this](int, bool) { updateStats(); });
    connect(tm, &TaskManager::tasksCleared, this, [this]() { updateStats(); });
}

void TodoSettingsPanel::setTodoWidget(DesktopTodoWidget *widget)
{
    m_todoWidget = widget;
    updateStats();
}

void TodoSettingsPanel::updateStats()
{
    TaskManager *tm = TaskManager::instance();
    int total = tm->taskCount();
    int completed = tm->completedCount();
    m_statsLabel->setText(QString("共 %1 项任务，已完成 %2 项").arg(total).arg(completed));
    
    LogManager *lm = LogManager::instance();
    int logCount = lm->logCount();
    m_logCountLabel->setText(QString("日志记录: %1 条").arg(logCount));
}

void TodoSettingsPanel::onShowTodoClicked()
{
    if (m_todoWidget) {
        m_todoWidget->show();
        m_todoWidget->raise();
        m_todoWidget->activateWindow();
    }
    emit showTodoRequested();
}

void TodoSettingsPanel::onHideTodoClicked()
{
    if (m_todoWidget) {
        m_todoWidget->hide();
    }
    emit hideTodoRequested();
}

void TodoSettingsPanel::onModeChanged(int index)
{
    if (m_todoWidget) {
        int modeValue = m_modeCombo->itemData(index).toInt();
        m_todoWidget->setDisplayMode(static_cast<DesktopTodoWidget::DisplayMode>(modeValue));
        m_todoWidget->show();
    }
}

void TodoSettingsPanel::onClearCompletedClicked()
{
    TaskManager::instance()->clearCompletedTasks();
    updateStats();
}

void TodoSettingsPanel::onExportJsonClicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, "导出日志", "", "JSON文件 (*.json)");
    if (!filePath.isEmpty()) {
        LogManager::instance()->exportToJson(filePath);
    }
}

void TodoSettingsPanel::onExportCsvClicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, "导出日志", "", "CSV文件 (*.csv)");
    if (!filePath.isEmpty()) {
        LogManager::instance()->exportToCsv(filePath);
    }
}

void TodoSettingsPanel::onClearLogsClicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "确认重置",
        "确定要清空所有日志记录吗？\n此操作不可撤销。",
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        LogManager::instance()->clearLogs();
        updateStats();
    }
}

void TodoSettingsPanel::onBackClicked()
{
    emit backClicked();
}

void TodoSettingsPanel::onBackgroundColorClicked()
{
    QColor color = QColorDialog::getColor(m_backgroundColor, this, "选择背景颜色");
    if (color.isValid()) {
        m_backgroundColor = color;
        m_bgColorBtn->setStyleSheet(QString("background-color: %1; border: 1px solid #ccc; border-radius: 3px;").arg(color.name()));
        if (m_todoWidget) {
            m_todoWidget->setBackgroundColor(color);
        }
    }
}

void TodoSettingsPanel::onBackgroundOpacityChanged(int value)
{
    m_bgOpacityLabel->setText(QString::number(value));
    if (m_todoWidget) {
        m_todoWidget->setBackgroundOpacity(value);
    }
}

void TodoSettingsPanel::onFontChanged(int index)
{
    if (m_todoWidget && index > 0) {
        QString fontFamily = m_fontCombo->itemText(index);
        QFont font(fontFamily);
        m_todoWidget->setContentFont(font);
    } else if (m_todoWidget) {
        QFont font;
        m_todoWidget->setContentFont(font);
    }
}

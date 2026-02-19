#include "timersettingspanel.h"
#include "settingsmanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QColorDialog>
#include <QSpacerItem>

TimerSettingsPanel::TimerSettingsPanel(QWidget *parent)
    : QWidget(parent)
    , m_modeTabWidget(nullptr)
    , m_singleModeWidget(nullptr)
    , m_singleHoursSpin(nullptr)
    , m_singleMinutesSpin(nullptr)
    , m_singleSecondsSpin(nullptr)
    , m_cycleModeWidget(nullptr)
    , m_workMinutesSpin(nullptr)
    , m_breakMinutesSpin(nullptr)
    , m_cycleCountSpin(nullptr)
    , m_startBtn(nullptr)
    , m_pauseBtn(nullptr)
    , m_stopBtn(nullptr)
    , m_backBtn(nullptr)
    , m_widthSpin(nullptr)
    , m_heightSpin(nullptr)
    , m_fontSizeSlider(nullptr)
    , m_fontSizeLabel(nullptr)
    , m_bgColorBtn(nullptr)
    , m_textColorBtn(nullptr)
    , m_bgOpacitySlider(nullptr)
    , m_textOpacitySlider(nullptr)
    , m_soundCheckBox(nullptr)
    , m_flashCheckBox(nullptr)
    , m_isRunning(false)
{
    setupUi();
    setupConnections();
    loadSettings();
}

TimerSettingsPanel::~TimerSettingsPanel()
{
    saveSettings();
}

void TimerSettingsPanel::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(15);

    QHBoxLayout *headerLayout = new QHBoxLayout();
    m_backBtn = new QPushButton("← 返回", this);
    m_backBtn->setStyleSheet("QPushButton { background: none; border: none; color: #3498db; font-size: 14px; } QPushButton:hover { color: #2980b9; }");
    headerLayout->addWidget(m_backBtn);
    
    QLabel *titleLabel = new QLabel("桌面置顶计时器", this);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50;");
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    mainLayout->addLayout(headerLayout);

    m_modeTabWidget = new QTabWidget(this);
    m_modeTabWidget->setStyleSheet(
        "QTabWidget::pane { border: 1px solid #ddd; border-radius: 5px; }"
        "QTabBar::tab { background: #ecf0f1; padding: 8px 20px; margin-right: 2px; }"
        "QTabBar::tab:selected { background: #3498db; color: white; }"
    );

    m_singleModeWidget = new QWidget();
    QGridLayout *singleLayout = new QGridLayout(m_singleModeWidget);
    singleLayout->setSpacing(10);
    
    singleLayout->addWidget(new QLabel("时:", m_singleModeWidget), 0, 0);
    m_singleHoursSpin = new QSpinBox(m_singleModeWidget);
    m_singleHoursSpin->setRange(0, 23);
    m_singleHoursSpin->setFixedWidth(60);
    singleLayout->addWidget(m_singleHoursSpin, 0, 1);
    
    singleLayout->addWidget(new QLabel("分:", m_singleModeWidget), 0, 2);
    m_singleMinutesSpin = new QSpinBox(m_singleModeWidget);
    m_singleMinutesSpin->setRange(0, 59);
    m_singleMinutesSpin->setFixedWidth(60);
    singleLayout->addWidget(m_singleMinutesSpin, 0, 3);
    
    singleLayout->addWidget(new QLabel("秒:", m_singleModeWidget), 0, 4);
    m_singleSecondsSpin = new QSpinBox(m_singleModeWidget);
    m_singleSecondsSpin->setRange(0, 59);
    m_singleSecondsSpin->setFixedWidth(60);
    singleLayout->addWidget(m_singleSecondsSpin, 0, 5);
    
    singleLayout->setColumnStretch(6, 1);
    m_modeTabWidget->addTab(m_singleModeWidget, "单次计时");

    m_cycleModeWidget = new QWidget();
    QGridLayout *cycleLayout = new QGridLayout(m_cycleModeWidget);
    cycleLayout->setSpacing(10);
    
    cycleLayout->addWidget(new QLabel("工作时长(分):", m_cycleModeWidget), 0, 0);
    m_workMinutesSpin = new QSpinBox(m_cycleModeWidget);
    m_workMinutesSpin->setRange(1, 120);
    m_workMinutesSpin->setFixedWidth(60);
    cycleLayout->addWidget(m_workMinutesSpin, 0, 1);
    
    cycleLayout->addWidget(new QLabel("休息时长(分):", m_cycleModeWidget), 0, 2);
    m_breakMinutesSpin = new QSpinBox(m_cycleModeWidget);
    m_breakMinutesSpin->setRange(1, 60);
    m_breakMinutesSpin->setFixedWidth(60);
    cycleLayout->addWidget(m_breakMinutesSpin, 0, 3);
    
    cycleLayout->addWidget(new QLabel("循环次数:", m_cycleModeWidget), 0, 4);
    m_cycleCountSpin = new QSpinBox(m_cycleModeWidget);
    m_cycleCountSpin->setRange(1, 20);
    m_cycleCountSpin->setFixedWidth(60);
    cycleLayout->addWidget(m_cycleCountSpin, 0, 5);
    
    cycleLayout->setColumnStretch(6, 1);
    m_modeTabWidget->addTab(m_cycleModeWidget, "番茄钟模式");

    mainLayout->addWidget(m_modeTabWidget);

    QHBoxLayout *controlLayout = new QHBoxLayout();
    controlLayout->setSpacing(10);
    
    m_startBtn = new QPushButton("▶ 开始", this);
    m_startBtn->setStyleSheet(
        "QPushButton { background-color: #27ae60; color: white; border: none; padding: 10px 30px; "
        "font-size: 14px; border-radius: 5px; }"
        "QPushButton:hover { background-color: #2ecc71; }"
        "QPushButton:disabled { background-color: #95a5a6; }"
    );
    controlLayout->addWidget(m_startBtn);
    
    m_pauseBtn = new QPushButton("⏸ 暂停", this);
    m_pauseBtn->setEnabled(false);
    m_pauseBtn->setStyleSheet(
        "QPushButton { background-color: #f39c12; color: white; border: none; padding: 10px 30px; "
        "font-size: 14px; border-radius: 5px; }"
        "QPushButton:hover { background-color: #e67e22; }"
        "QPushButton:disabled { background-color: #95a5a6; }"
    );
    controlLayout->addWidget(m_pauseBtn);
    
    m_stopBtn = new QPushButton("⏹ 停止", this);
    m_stopBtn->setEnabled(false);
    m_stopBtn->setStyleSheet(
        "QPushButton { background-color: #e74c3c; color: white; border: none; padding: 10px 30px; "
        "font-size: 14px; border-radius: 5px; }"
        "QPushButton:hover { background-color: #c0392b; }"
        "QPushButton:disabled { background-color: #95a5a6; }"
    );
    controlLayout->addWidget(m_stopBtn);
    
    controlLayout->addStretch();
    mainLayout->addLayout(controlLayout);

    QGroupBox *appearanceGroup = new QGroupBox("外观设置", this);
    appearanceGroup->setStyleSheet("QGroupBox { font-weight: bold; border: 1px solid #ddd; border-radius: 5px; margin-top: 10px; padding-top: 10px; }");
    QGridLayout *appearanceLayout = new QGridLayout(appearanceGroup);
    appearanceLayout->setSpacing(10);
    
    appearanceLayout->addWidget(new QLabel("窗口宽度:", this), 0, 0);
    m_widthSpin = new QSpinBox(this);
    m_widthSpin->setRange(100, 500);
    m_widthSpin->setFixedWidth(70);
    appearanceLayout->addWidget(m_widthSpin, 0, 1);
    
    appearanceLayout->addWidget(new QLabel("窗口高度:", this), 0, 2);
    m_heightSpin = new QSpinBox(this);
    m_heightSpin->setRange(50, 300);
    m_heightSpin->setFixedWidth(70);
    appearanceLayout->addWidget(m_heightSpin, 0, 3);
    
    appearanceLayout->addWidget(new QLabel("字体大小:", this), 1, 0);
    m_fontSizeSlider = new QSlider(Qt::Horizontal, this);
    m_fontSizeSlider->setRange(16, 72);
    m_fontSizeSlider->setFixedWidth(100);
    appearanceLayout->addWidget(m_fontSizeSlider, 1, 1);
    m_fontSizeLabel = new QLabel("32", this);
    m_fontSizeLabel->setFixedWidth(30);
    appearanceLayout->addWidget(m_fontSizeLabel, 1, 2);
    
    appearanceLayout->addWidget(new QLabel("背景颜色:", this), 2, 0);
    m_bgColorBtn = new QPushButton(this);
    m_bgColorBtn->setFixedSize(60, 25);
    appearanceLayout->addWidget(m_bgColorBtn, 2, 1);
    
    appearanceLayout->addWidget(new QLabel("文字颜色:", this), 2, 2);
    m_textColorBtn = new QPushButton(this);
    m_textColorBtn->setFixedSize(60, 25);
    appearanceLayout->addWidget(m_textColorBtn, 2, 3);
    
    appearanceLayout->addWidget(new QLabel("背景透明度:", this), 3, 0);
    m_bgOpacitySlider = new QSlider(Qt::Horizontal, this);
    m_bgOpacitySlider->setRange(0, 255);
    m_bgOpacitySlider->setFixedWidth(100);
    appearanceLayout->addWidget(m_bgOpacitySlider, 3, 1);
    
    appearanceLayout->addWidget(new QLabel("文字透明度:", this), 3, 2);
    m_textOpacitySlider = new QSlider(Qt::Horizontal, this);
    m_textOpacitySlider->setRange(0, 255);
    m_textOpacitySlider->setFixedWidth(100);
    appearanceLayout->addWidget(m_textOpacitySlider, 3, 3);
    
    mainLayout->addWidget(appearanceGroup);

    QGroupBox *alertGroup = new QGroupBox("提示设置", this);
    alertGroup->setStyleSheet("QGroupBox { font-weight: bold; border: 1px solid #ddd; border-radius: 5px; margin-top: 10px; padding-top: 10px; }");
    QHBoxLayout *alertLayout = new QHBoxLayout(alertGroup);
    
    m_soundCheckBox = new QCheckBox("启用声音提示", this);
    m_soundCheckBox->setChecked(true);
    alertLayout->addWidget(m_soundCheckBox);
    
    m_flashCheckBox = new QCheckBox("启用闪烁提示", this);
    m_flashCheckBox->setChecked(true);
    alertLayout->addWidget(m_flashCheckBox);
    
    alertLayout->addStretch();
    mainLayout->addWidget(alertGroup);

    mainLayout->addStretch();
}

void TimerSettingsPanel::setupConnections()
{
    connect(m_startBtn, &QPushButton::clicked, this, &TimerSettingsPanel::onStartClicked);
    connect(m_pauseBtn, &QPushButton::clicked, this, &TimerSettingsPanel::onPauseClicked);
    connect(m_stopBtn, &QPushButton::clicked, this, &TimerSettingsPanel::onStopClicked);
    connect(m_backBtn, &QPushButton::clicked, this, &TimerSettingsPanel::onBackClicked);
    
    connect(m_modeTabWidget, &QTabWidget::currentChanged, this, &TimerSettingsPanel::onModeChanged);
    
    connect(m_fontSizeSlider, &QSlider::valueChanged, this, [this](int value) {
        m_fontSizeLabel->setText(QString::number(value));
        emit settingsChanged();
    });
    
    connect(m_bgColorBtn, &QPushButton::clicked, this, &TimerSettingsPanel::onBackgroundColorClicked);
    connect(m_textColorBtn, &QPushButton::clicked, this, &TimerSettingsPanel::onTextColorClicked);
    
    connect(m_widthSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &TimerSettingsPanel::settingsChanged);
    connect(m_heightSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &TimerSettingsPanel::settingsChanged);
    connect(m_bgOpacitySlider, &QSlider::valueChanged, this, &TimerSettingsPanel::settingsChanged);
    connect(m_textOpacitySlider, &QSlider::valueChanged, this, &TimerSettingsPanel::settingsChanged);
    connect(m_soundCheckBox, &QCheckBox::toggled, this, &TimerSettingsPanel::settingsChanged);
    connect(m_flashCheckBox, &QCheckBox::toggled, this, &TimerSettingsPanel::settingsChanged);
}

void TimerSettingsPanel::loadSettings()
{
    SettingsManager *settings = SettingsManager::instance();
    
    setSingleTime(settings->defaultHours(), settings->defaultMinutes(), settings->defaultSeconds());
    setWorkMinutes(settings->workDuration());
    setBreakMinutes(settings->breakDuration());
    setCycleCount(settings->cycleCount());
    
    setOverlaySize(settings->overlayWidth(), settings->overlayHeight());
    setFontSize(settings->fontSize());
    setBackgroundColor(settings->backgroundColor());
    setTextColor(settings->textColor());
    setBackgroundOpacity(settings->backgroundOpacity());
    setTextOpacity(settings->textOpacity());
    setSoundEnabled(settings->soundEnabled());
    setFlashEnabled(settings->flashEnabled());
}

void TimerSettingsPanel::saveSettings()
{
    SettingsManager *settings = SettingsManager::instance();
    
    settings->setDefaultTime(singleHours(), singleMinutes(), singleSeconds());
    settings->setWorkDuration(workMinutes());
    settings->setBreakDuration(breakMinutes());
    settings->setCycleCount(cycleCount());
    
    settings->setOverlaySize(overlayWidth(), overlayHeight());
    settings->setFontSize(fontSize());
    settings->setBackgroundColor(backgroundColor());
    settings->setTextColor(textColor());
    settings->setBackgroundOpacity(backgroundOpacity());
    settings->setTextOpacity(textOpacity());
    settings->setSoundEnabled(soundEnabled());
    settings->setFlashEnabled(flashEnabled());
    
    settings->saveAll();
}

int TimerSettingsPanel::singleHours() const
{
    return m_singleHoursSpin->value();
}

int TimerSettingsPanel::singleMinutes() const
{
    return m_singleMinutesSpin->value();
}

int TimerSettingsPanel::singleSeconds() const
{
    return m_singleSecondsSpin->value();
}

int TimerSettingsPanel::workMinutes() const
{
    return m_workMinutesSpin->value();
}

int TimerSettingsPanel::breakMinutes() const
{
    return m_breakMinutesSpin->value();
}

int TimerSettingsPanel::cycleCount() const
{
    return m_cycleCountSpin->value();
}

bool TimerSettingsPanel::isCycleMode() const
{
    return m_modeTabWidget->currentIndex() == 1;
}

int TimerSettingsPanel::overlayWidth() const
{
    return m_widthSpin->value();
}

int TimerSettingsPanel::overlayHeight() const
{
    return m_heightSpin->value();
}

int TimerSettingsPanel::fontSize() const
{
    return m_fontSizeSlider->value();
}

QColor TimerSettingsPanel::backgroundColor() const
{
    return m_backgroundColor;
}

QColor TimerSettingsPanel::textColor() const
{
    return m_textColor;
}

int TimerSettingsPanel::backgroundOpacity() const
{
    return m_bgOpacitySlider->value();
}

int TimerSettingsPanel::textOpacity() const
{
    return m_textOpacitySlider->value();
}

bool TimerSettingsPanel::soundEnabled() const
{
    return m_soundCheckBox->isChecked();
}

bool TimerSettingsPanel::flashEnabled() const
{
    return m_flashCheckBox->isChecked();
}

void TimerSettingsPanel::setSingleTime(int hours, int minutes, int seconds)
{
    m_singleHoursSpin->setValue(hours);
    m_singleMinutesSpin->setValue(minutes);
    m_singleSecondsSpin->setValue(seconds);
}

void TimerSettingsPanel::setWorkMinutes(int minutes)
{
    m_workMinutesSpin->setValue(minutes);
}

void TimerSettingsPanel::setBreakMinutes(int minutes)
{
    m_breakMinutesSpin->setValue(minutes);
}

void TimerSettingsPanel::setCycleCount(int count)
{
    m_cycleCountSpin->setValue(count);
}

void TimerSettingsPanel::setCycleMode(bool isCycle)
{
    m_modeTabWidget->setCurrentIndex(isCycle ? 1 : 0);
}

void TimerSettingsPanel::setOverlaySize(int width, int height)
{
    m_widthSpin->setValue(width);
    m_heightSpin->setValue(height);
}

void TimerSettingsPanel::setFontSize(int size)
{
    m_fontSizeSlider->setValue(size);
    m_fontSizeLabel->setText(QString::number(size));
}

void TimerSettingsPanel::setBackgroundColor(const QColor &color)
{
    m_backgroundColor = color;
    m_bgColorBtn->setStyleSheet(QString("background-color: %1; border: 1px solid #ccc; border-radius: 3px;").arg(color.name()));
}

void TimerSettingsPanel::setTextColor(const QColor &color)
{
    m_textColor = color;
    m_textColorBtn->setStyleSheet(QString("background-color: %1; border: 1px solid #ccc; border-radius: 3px;").arg(color.name()));
}

void TimerSettingsPanel::setBackgroundOpacity(int opacity)
{
    m_bgOpacitySlider->setValue(opacity);
}

void TimerSettingsPanel::setTextOpacity(int opacity)
{
    m_textOpacitySlider->setValue(opacity);
}

void TimerSettingsPanel::setSoundEnabled(bool enabled)
{
    m_soundCheckBox->setChecked(enabled);
}

void TimerSettingsPanel::setFlashEnabled(bool enabled)
{
    m_flashCheckBox->setChecked(enabled);
}

void TimerSettingsPanel::resetControls()
{
    m_isRunning = false;
    m_startBtn->setEnabled(true);
    m_pauseBtn->setEnabled(false);
    m_stopBtn->setEnabled(false);
    m_modeTabWidget->setEnabled(true);
}

void TimerSettingsPanel::onStartClicked()
{
    m_isRunning = true;
    m_startBtn->setEnabled(false);
    m_pauseBtn->setEnabled(true);
    m_stopBtn->setEnabled(true);
    m_modeTabWidget->setEnabled(false);
    emit startClicked();
}

void TimerSettingsPanel::onPauseClicked()
{
    m_isRunning = false;
    m_startBtn->setEnabled(true);
    m_pauseBtn->setEnabled(false);
    emit pauseClicked();
}

void TimerSettingsPanel::onStopClicked()
{
    m_isRunning = false;
    m_startBtn->setEnabled(true);
    m_pauseBtn->setEnabled(false);
    m_stopBtn->setEnabled(false);
    m_modeTabWidget->setEnabled(true);
    emit stopClicked();
}

void TimerSettingsPanel::onModeChanged(int index)
{
    Q_UNUSED(index);
}

void TimerSettingsPanel::onBackgroundColorClicked()
{
    QColor color = QColorDialog::getColor(m_backgroundColor, this, "选择背景颜色");
    if (color.isValid()) {
        setBackgroundColor(color);
        emit settingsChanged();
    }
}

void TimerSettingsPanel::onTextColorClicked()
{
    QColor color = QColorDialog::getColor(m_textColor, this, "选择文字颜色");
    if (color.isValid()) {
        setTextColor(color);
        emit settingsChanged();
    }
}

void TimerSettingsPanel::onBackClicked()
{
    saveSettings();
    emit backClicked();
}

#include "clipboardsettingspanel.h"
#include <QDebug>

ClipboardSettingsPanel::ClipboardSettingsPanel(QWidget* parent)
    : QWidget(parent)
    , m_manager(nullptr)
    , m_maxItemsSpin(nullptr)
    , m_autoStartCheck(nullptr)
{
    qDebug() << "剪贴板设置面板构造函数开始";
    
    m_manager = ClipboardManager::instance(this);
    
    setupUI();
    
    qDebug() << "剪贴板设置面板构造函数完成";
}

ClipboardSettingsPanel::~ClipboardSettingsPanel() {
}

void ClipboardSettingsPanel::setupUI() {
    setWindowTitle("剪贴板设置");
    setMinimumSize(300, 200);
    setStyleSheet("QWidget { background-color: #f5f6fa; }");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    QLabel* titleLabel = new QLabel("剪贴板历史管理", this);
    titleLabel->setStyleSheet("QLabel { font-size: 18px; font-weight: bold; color: #2c3e50; }");
    mainLayout->addWidget(titleLabel);

    QGroupBox* settingsGroup = new QGroupBox("设置", this);
    settingsGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #2c3e50; }");

    QVBoxLayout* groupLayout = new QVBoxLayout(settingsGroup);

    QHBoxLayout* maxItemsLayout = new QHBoxLayout();
    QLabel* maxItemsLabel = new QLabel("最大历史条数:", this);
    m_maxItemsSpin = new QSpinBox(this);
    m_maxItemsSpin->setRange(100, 5000);
    m_maxItemsSpin->setValue(1000);
    m_maxItemsSpin->setStyleSheet("QSpinBox { padding: 5px; border: 1px solid #dcdde1; border-radius: 3px; }");
    maxItemsLayout->addWidget(maxItemsLabel);
    maxItemsLayout->addWidget(m_maxItemsSpin);
    groupLayout->addLayout(maxItemsLayout);

    m_autoStartCheck = new QCheckBox("启动时自动开始监控", this);
    m_autoStartCheck->setStyleSheet("QCheckBox { color: #2c3e50; }");
    groupLayout->addWidget(m_autoStartCheck);

    mainLayout->addWidget(settingsGroup);

    QGroupBox* actionsGroup = new QGroupBox("操作", this);
    actionsGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #2c3e50; }");

    QVBoxLayout* actionLayout = new QVBoxLayout(actionsGroup);

    QPushButton* openBtn = new QPushButton("打开剪贴板管理器", this);
    openBtn->setStyleSheet("QPushButton { padding: 10px; background-color: #3498db; color: white; border: none; border-radius: 5px; font-weight: bold; }");
    openBtn->setCursor(Qt::PointingHandCursor);
    connect(openBtn, &QPushButton::clicked, this, &ClipboardSettingsPanel::onOpenClicked);
    actionLayout->addWidget(openBtn);

    QPushButton* clearBtn = new QPushButton("清空历史", this);
    clearBtn->setStyleSheet("QPushButton { padding: 10px; background-color: #e74c3c; color: white; border: none; border-radius: 5px; }");
    clearBtn->setCursor(Qt::PointingHandCursor);
    connect(clearBtn, &QPushButton::clicked, this, &ClipboardSettingsPanel::onClearHistory);
    actionLayout->addWidget(clearBtn);

    mainLayout->addWidget(actionsGroup);

    int count = m_manager ? m_manager->getTotalCount() : 0;
    QLabel* statsLabel = new QLabel(QString("当前记录数: %1").arg(count), this);
    statsLabel->setStyleSheet("QLabel { color: #7f8c8d; font-size: 12px; }");
    mainLayout->addWidget(statsLabel);

    mainLayout->addStretch();

    QPushButton* backBtn = new QPushButton("返回", this);
    backBtn->setStyleSheet("QPushButton { padding: 10px 20px; background-color: #95a5a6; color: white; border: none; border-radius: 5px; }");
    backBtn->setCursor(Qt::PointingHandCursor);
    connect(backBtn, &QPushButton::clicked, this, &ClipboardSettingsPanel::backRequested);
    mainLayout->addWidget(backBtn);
}

void ClipboardSettingsPanel::onOpenClicked() {
    qDebug() << "打开剪贴板管理器按钮被点击";
    emit openClipboardManager();
}

void ClipboardSettingsPanel::onClearHistory() {
    if (m_manager) {
        m_manager->clearAll();
    }
}

void ClipboardSettingsPanel::onMaxItemsChanged(int value) {
    qDebug() << "最大历史条数改为:" << value;
}

void ClipboardSettingsPanel::onAutoStartChanged(bool checked) {
    qDebug() << "自动启动监控改为:" << checked;
}

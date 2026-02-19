#include "mainwindow.h"
#include "modules/timer/timercontroller.h"
#include <QLabel>
#include <QScrollArea>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_homeWidget(nullptr)
    , m_stackedWidget(nullptr)
    , m_timerModuleBtn(nullptr)
    , m_timerController(nullptr)
    , m_timerPanel(nullptr)
{
    setupUi();
    setupConnections();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUi()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);

    QLabel *titleLabel = new QLabel("PC效率工具箱", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #2c3e50; padding: 10px;");
    mainLayout->addWidget(titleLabel);

    m_stackedWidget = new QStackedWidget(this);
    mainLayout->addWidget(m_stackedWidget);

    m_homeWidget = new QWidget(this);
    QVBoxLayout *homeLayout = new QVBoxLayout(m_homeWidget);
    homeLayout->setSpacing(15);
    homeLayout->setContentsMargins(20, 20, 20, 20);

    QLabel *selectLabel = new QLabel("选择工具模块:", m_homeWidget);
    selectLabel->setStyleSheet("font-size: 14px; color: #7f8c8d;");
    homeLayout->addWidget(selectLabel);

    m_timerModuleBtn = new QPushButton("⏱ 桌面置顶计时器", m_homeWidget);
    m_timerModuleBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #3498db;"
        "  color: white;"
        "  border: none;"
        "  padding: 20px;"
        "  font-size: 16px;"
        "  border-radius: 8px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #2980b9;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #21618c;"
        "}"
    );
    m_timerModuleBtn->setCursor(Qt::PointingHandCursor);
    homeLayout->addWidget(m_timerModuleBtn);

    QPushButton *todoModuleBtn = new QPushButton("📋 桌面待办事项 (开发中)", m_homeWidget);
    todoModuleBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #95a5a6;"
        "  color: white;"
        "  border: none;"
        "  padding: 20px;"
        "  font-size: 16px;"
        "  border-radius: 8px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #7f8c8d;"
        "}"
    );
    todoModuleBtn->setEnabled(false);
    homeLayout->addWidget(todoModuleBtn);

    homeLayout->addStretch();

    m_stackedWidget->addWidget(m_homeWidget);

    m_timerController = new TimerController(this);
    m_timerPanel = m_timerController->settingsPanel();
    m_stackedWidget->addWidget(m_timerPanel);

    m_stackedWidget->setCurrentWidget(m_homeWidget);
}

void MainWindow::setupConnections()
{
    connect(m_timerModuleBtn, &QPushButton::clicked, this, &MainWindow::onTimerModuleClicked);
}

void MainWindow::onTimerModuleClicked()
{
    m_stackedWidget->setCurrentWidget(m_timerPanel);
    m_timerController->showOverlay();
}

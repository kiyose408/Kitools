#include "mainwindow.h"
#include "modules/timer/timercontroller.h"
#include <QLabel>
#include <QScrollArea>
#include <QApplication>
#include <QStyle>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_homeWidget(nullptr)
    , m_stackedWidget(nullptr)
    , m_timerModuleBtn(nullptr)
    , m_timerController(nullptr)
    , m_timerPanel(nullptr)
    , m_trayIcon(nullptr)
    , m_trayMenu(nullptr)
    , m_showAction(nullptr)
    , m_quitAction(nullptr)
    , m_forceQuit(false)
{
    setupUi();
    setupConnections();
    setupTrayIcon();
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

void MainWindow::setupTrayIcon()
{
    createTrayMenu();
    
    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setContextMenu(m_trayMenu);
    
    QIcon icon = QApplication::style()->standardIcon(QStyle::SP_MediaPlay);
    m_trayIcon->setIcon(icon);
    m_trayIcon->setToolTip("PC效率工具箱");
    
    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::onTrayIconActivated);
    
    m_trayIcon->show();
}

void MainWindow::createTrayMenu()
{
    m_trayMenu = new QMenu(this);
    
    m_showAction = new QAction("显示主窗口", this);
    m_showAction->setIcon(QApplication::style()->standardIcon(QStyle::SP_ComputerIcon));
    connect(m_showAction, &QAction::triggered, this, &MainWindow::onShowWindow);
    m_trayMenu->addAction(m_showAction);
    
    m_trayMenu->addSeparator();
    
    m_quitAction = new QAction("退出", this);
    m_quitAction->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogCloseButton));
    connect(m_quitAction, &QAction::triggered, this, &MainWindow::onQuitApplication);
    m_trayMenu->addAction(m_quitAction);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_forceQuit) {
        event->accept();
        return;
    }
    
    if (m_trayIcon && m_trayIcon->isVisible()) {
        hide();
        m_trayIcon->showMessage(
            "PC效率工具箱",
            "程序已最小化到系统托盘，继续在后台运行。\n双击托盘图标可恢复窗口。",
            QSystemTrayIcon::Information,
            2000
        );
        event->ignore();
    } else {
        event->accept();
    }
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowStateChange) {
        if (isMinimized()) {
            if (m_trayIcon && m_trayIcon->isVisible()) {
                hide();
                event->ignore();
                return;
            }
        }
    }
    QMainWindow::changeEvent(event);
}

void MainWindow::onTimerModuleClicked()
{
    m_stackedWidget->setCurrentWidget(m_timerPanel);
    m_timerController->showOverlay();
}

void MainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
        case QSystemTrayIcon::DoubleClick:
        case QSystemTrayIcon::Trigger:
            onShowWindow();
            break;
        default:
            break;
    }
}

void MainWindow::onShowWindow()
{
    show();
    setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    raise();
    activateWindow();
}

void MainWindow::onQuitApplication()
{
    m_forceQuit = true;
    
    if (m_trayIcon) {
        m_trayIcon->hide();
    }
    
    QApplication::quit();
}

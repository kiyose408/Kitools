#include "mainwindow.h"
#include "modules/timer/timercontroller.h"
#include "modules/timer/timersettingspanel.h"
#include "modules/todo/todocontroller.h"
#include "modules/todo/todosettingspanel.h"
#include "modules/notes/notescontroller.h"
#include "modules/notes/notessettingspanel.h"
#include "modules/clipboard/clipboardcontroller.h"
#include "modules/clipboard/clipboardsettingspanel.h"
#include "modules/launcher/launchercontroller.h"
#include "modules/launcher/launchersettingspanel.h"
#include <QLabel>
#include <QScrollArea>
#include <QApplication>
#include <QStyle>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_homeWidget(nullptr)
    , m_stackedWidget(nullptr)
    , m_timerModuleBtn(nullptr)
    , m_todoModuleBtn(nullptr)
    , m_notesModuleBtn(nullptr)
    , m_clipboardModuleBtn(nullptr)
    , m_launcherModuleBtn(nullptr)
    , m_timerController(nullptr)
    , m_timerPanel(nullptr)
    , m_todoController(nullptr)
    , m_todoPanel(nullptr)
    , m_notesController(nullptr)
    , m_notesPanel(nullptr)
    , m_clipboardController(nullptr)
    , m_clipboardPanel(nullptr)
    , m_launcherController(nullptr)
    , m_launcherPanel(nullptr)
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

    m_todoModuleBtn = new QPushButton("📋 桌面待办事项", m_homeWidget);
    m_todoModuleBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #9b59b6;"
        "  color: white;"
        "  border: none;"
        "  padding: 20px;"
        "  font-size: 16px;"
        "  border-radius: 8px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #8e44ad;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #7d3c98;"
        "}"
    );
    m_todoModuleBtn->setCursor(Qt::PointingHandCursor);
    homeLayout->addWidget(m_todoModuleBtn);

    m_notesModuleBtn = new QPushButton("📝 桌面便签", m_homeWidget);
    m_notesModuleBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #e67e22;"
        "  color: white;"
        "  border: none;"
        "  padding: 20px;"
        "  font-size: 16px;"
        "  border-radius: 8px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #d35400;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #ba4a00;"
        "}"
    );
    m_notesModuleBtn->setCursor(Qt::PointingHandCursor);
    homeLayout->addWidget(m_notesModuleBtn);

    m_clipboardModuleBtn = new QPushButton("📋 剪贴板历史", m_homeWidget);
    m_clipboardModuleBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #27ae60;"
        "  color: white;"
        "  border: none;"
        "  padding: 20px;"
        "  font-size: 16px;"
        "  border-radius: 8px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #229954;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #1e8449;"
        "}"
    );
    m_clipboardModuleBtn->setCursor(Qt::PointingHandCursor);
    homeLayout->addWidget(m_clipboardModuleBtn);

    m_launcherModuleBtn = new QPushButton("🚀 快捷启动器", m_homeWidget);
    m_launcherModuleBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #e74c3c;"
        "  color: white;"
        "  border: none;"
        "  padding: 20px;"
        "  font-size: 16px;"
        "  border-radius: 8px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #c0392b;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #a93226;"
        "}"
    );
    m_launcherModuleBtn->setCursor(Qt::PointingHandCursor);
    homeLayout->addWidget(m_launcherModuleBtn);

    m_notesController = new NotesController(this);
    m_notesPanel = m_notesController->settingsPanel();
    m_stackedWidget->addWidget(m_notesPanel);

    m_clipboardController = new ClipboardController(this);
    m_clipboardPanel = m_clipboardController->getSettingsPanel();
    m_stackedWidget->addWidget(m_clipboardPanel);

    m_launcherController = new LauncherController(this);
    m_launcherPanel = m_launcherController->getSettingsPanel();
    m_stackedWidget->addWidget(m_launcherPanel);

    homeLayout->addStretch();

    m_stackedWidget->addWidget(m_homeWidget);

    m_timerController = new TimerController(this);
    m_timerPanel = m_timerController->settingsPanel();
    m_stackedWidget->addWidget(m_timerPanel);

    m_todoController = new TodoController(this);
    m_todoPanel = m_todoController->settingsPanel();
    m_stackedWidget->addWidget(m_todoPanel);

    m_stackedWidget->setCurrentWidget(m_homeWidget);
}

void MainWindow::setupConnections()
{
    qDebug() << "Setting up connections...";
    connect(m_timerModuleBtn, &QPushButton::clicked, this, &MainWindow::onTimerModuleClicked);
    connect(m_todoModuleBtn, &QPushButton::clicked, this, &MainWindow::onTodoModuleClicked);
    connect(m_notesModuleBtn, &QPushButton::clicked, this, &MainWindow::onNotesModuleClicked);
    connect(m_clipboardModuleBtn, &QPushButton::clicked, this, &MainWindow::onClipboardModuleClicked);
    connect(m_launcherModuleBtn, &QPushButton::clicked, this, &MainWindow::onLauncherModuleClicked);
    qDebug() << "Button connections done.";

    TimerSettingsPanel *timerPanel = qobject_cast<TimerSettingsPanel*>(m_timerPanel);
    if (timerPanel) {
        connect(timerPanel, &TimerSettingsPanel::backClicked, this, &MainWindow::onBackToHome);
    }
    
    TodoSettingsPanel *todoPanel = qobject_cast<TodoSettingsPanel*>(m_todoPanel);
    if (todoPanel) {
        connect(todoPanel, &TodoSettingsPanel::backClicked, this, &MainWindow::onBackToHome);
    }
    
    NotesSettingsPanel *notesPanel = qobject_cast<NotesSettingsPanel*>(m_notesPanel);
    if (notesPanel) {
        connect(notesPanel, &NotesSettingsPanel::backClicked, this, &MainWindow::onBackToHome);
    }

    ClipboardSettingsPanel *clipboardPanel = qobject_cast<ClipboardSettingsPanel*>(m_clipboardPanel);
    if (clipboardPanel) {
        connect(clipboardPanel, &ClipboardSettingsPanel::backRequested, this, &MainWindow::onBackToHome);
    }

    LauncherSettingsPanel *launcherPanel = qobject_cast<LauncherSettingsPanel*>(m_launcherPanel);
    if (launcherPanel) {
        connect(launcherPanel, &LauncherSettingsPanel::backRequested, this, &MainWindow::onBackToHome);
    }
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
    TimerSettingsPanel *timerPanel = qobject_cast<TimerSettingsPanel*>(m_timerPanel);
    if (timerPanel) {
        timerPanel->setRunningState(m_timerController->isTimerRunning(), m_timerController->isTimerPaused());
    }
    m_stackedWidget->setCurrentWidget(m_timerPanel);
    m_timerController->showOverlay();
}

void MainWindow::onTodoModuleClicked()
{
    m_stackedWidget->setCurrentWidget(m_todoPanel);
    m_todoController->showTodoWidget();
}

void MainWindow::onNotesModuleClicked()
{
    m_stackedWidget->setCurrentWidget(m_notesPanel);
    m_notesController->showAllNotes();
}

void MainWindow::onClipboardModuleClicked()
{
    qDebug() << "剪贴板模块被点击";
    m_stackedWidget->setCurrentWidget(m_clipboardPanel);
    qDebug() << "当前窗口已切换到剪贴板面板";
}

void MainWindow::onLauncherModuleClicked()
{
    qDebug() << "快捷启动器模块被点击";
    m_stackedWidget->setCurrentWidget(m_launcherPanel);
    m_launcherController->startIndexing();
    qDebug() << "当前窗口已切换到快捷启动器面板";
}

void MainWindow::onBackToHome()
{
    m_stackedWidget->setCurrentWidget(m_homeWidget);
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

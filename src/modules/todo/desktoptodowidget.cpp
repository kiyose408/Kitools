#include "desktoptodowidget.h"
#include "taskitemwidget.h"
#include "taskmanager.h"
#include "logmanager.h"
#include <QMouseEvent>
#include <QApplication>
#include <QScreen>
#include <QPainter>
#include <QDate>

DesktopTodoWidget::DesktopTodoWidget(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_headerWidget(nullptr)
    , m_titleWidget(nullptr)
    , m_dateLabel(nullptr)
    , m_clearCompletedBtn(nullptr)
    , m_taskInput(nullptr)
    , m_addBtn(nullptr)
    , m_scrollArea(nullptr)
    , m_taskContainer(nullptr)
    , m_taskLayout(nullptr)
    , m_countLabel(nullptr)
    , m_displayMode(DisplayMode::AlwaysOnTop)
    , m_isDragging(false)
    , m_backgroundColor(255, 255, 255)
    , m_backgroundOpacity(230)
    , m_contentFont()
{
    setupUi();
    setupConnections();
    applyWindowFlags();
    applyStyleSheet();
    refreshTasks();
}

DesktopTodoWidget::~DesktopTodoWidget()
{
}

void DesktopTodoWidget::setupUi()
{
    setWindowTitle("待办事项");
    setMinimumSize(280, 350);
    resize(300, 450);
    
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(10, 10, 10, 10);
    m_mainLayout->setSpacing(10);
    
    m_titleWidget = new QWidget(this);
    m_titleWidget->setAttribute(Qt::WA_TranslucentBackground);
    QHBoxLayout *titleLayout = new QHBoxLayout(m_titleWidget);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->setSpacing(5);
    
    m_dateLabel = new QLabel(m_titleWidget);
    m_dateLabel->setAlignment(Qt::AlignCenter);
    updateDateLabel();
    titleLayout->addWidget(m_dateLabel, 1);
    
    m_clearCompletedBtn = new QPushButton("清理", m_titleWidget);
    m_clearCompletedBtn->setFixedHeight(26);
    m_clearCompletedBtn->setToolTip("点击清理已完成待办");
    m_clearCompletedBtn->setStyleSheet(
        "QPushButton { background-color: #e74c3c; color: white; border: none; border-radius: 5px; "
        "font-size: 12px; padding: 2px 10px; }"
        "QPushButton:hover { background-color: #c0392b; }"
        "QPushButton:pressed { background-color: #a93226; }"
    );
    titleLayout->addWidget(m_clearCompletedBtn);
    
    m_mainLayout->addWidget(m_titleWidget);
    
    m_headerWidget = new QWidget(this);
    m_headerWidget->setAttribute(Qt::WA_TranslucentBackground);
    QHBoxLayout *headerLayout = new QHBoxLayout(m_headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(8);
    
    m_taskInput = new QLineEdit(m_headerWidget);
    m_taskInput->setPlaceholderText("输入新任务...");
    headerLayout->addWidget(m_taskInput, 1);
    
    m_addBtn = new QPushButton("+", m_headerWidget);
    m_addBtn->setFixedSize(36, 36);
    m_addBtn->setStyleSheet(
        "QPushButton { background-color: #27ae60; color: white; border: none; border-radius: 18px; font-size: 20px; font-weight: bold; }"
        "QPushButton:hover { background-color: #2ecc71; }"
        "QPushButton:pressed { background-color: #1e8449; }"
    );
    headerLayout->addWidget(m_addBtn);
    
    m_mainLayout->addWidget(m_headerWidget);
    
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setAttribute(Qt::WA_TranslucentBackground);
    m_scrollArea->viewport()->setAttribute(Qt::WA_TranslucentBackground);
    
    m_taskContainer = new QWidget();
    m_taskContainer->setAttribute(Qt::WA_TranslucentBackground);
    m_taskLayout = new QVBoxLayout(m_taskContainer);
    m_taskLayout->setContentsMargins(5, 5, 5, 5);
    m_taskLayout->setSpacing(5);
    m_taskLayout->addStretch();
    
    m_scrollArea->setWidget(m_taskContainer);
    m_mainLayout->addWidget(m_scrollArea, 1);
    
    m_countLabel = new QLabel("共 0 项任务，已完成 0 项", this);
    m_countLabel->setAlignment(Qt::AlignCenter);
    m_mainLayout->addWidget(m_countLabel);
}

void DesktopTodoWidget::setupConnections()
{
    connect(m_addBtn, &QPushButton::clicked, this, &DesktopTodoWidget::onAddButtonClicked);
    connect(m_clearCompletedBtn, &QPushButton::clicked, this, &DesktopTodoWidget::onClearCompletedClicked);
    connect(m_taskInput, &QLineEdit::returnPressed, this, &DesktopTodoWidget::onAddButtonClicked);
    
    TaskManager *tm = TaskManager::instance();
    connect(tm, &TaskManager::taskAdded, this, [this](const TaskData &task) {
        addTaskItem(task);
        updateTaskCount();
    });
    connect(tm, &TaskManager::taskRemoved, this, [this](int taskId) {
        removeTaskItem(taskId);
        updateTaskCount();
    });
    connect(tm, &TaskManager::tasksCleared, this, [this]() {
        clearAllTasks();
        updateTaskCount();
    });
}

void DesktopTodoWidget::setDisplayMode(DisplayMode mode)
{
    m_displayMode = mode;
    applyWindowFlags();
    show();
}

void DesktopTodoWidget::setBackgroundColor(const QColor &color)
{
    m_backgroundColor = color;
    update();
}

void DesktopTodoWidget::setBackgroundOpacity(int opacity)
{
    m_backgroundOpacity = opacity;
    
    if (opacity == 0) {
        setAttribute(Qt::WA_TranslucentBackground, true);
    } else {
        setAttribute(Qt::WA_TranslucentBackground, false);
    }
    
    update();
}

void DesktopTodoWidget::setContentFont(const QFont &font)
{
    m_contentFont = font;
    applyStyleSheet();
    
    for (auto it = m_taskWidgets.begin(); it != m_taskWidgets.end(); ++it) {
        it.value()->setFont(font);
    }
}

void DesktopTodoWidget::applyWindowFlags()
{
    Qt::WindowFlags flags;
    
    if (m_displayMode == DisplayMode::AlwaysOnTop) {
        flags = Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Tool;
    } else {
        flags = Qt::Tool | Qt::FramelessWindowHint;
    }
    
    setWindowFlags(flags);
    
    if (m_backgroundOpacity == 0) {
        setAttribute(Qt::WA_TranslucentBackground, true);
    } else {
        setAttribute(Qt::WA_TranslucentBackground, false);
    }
}

void DesktopTodoWidget::applyStyleSheet()
{
    QString fontStr;
    if (m_contentFont.family().isEmpty()) {
        fontStr = "font-family: 'Microsoft YaHei', 'SimHei', sans-serif;";
    } else {
        fontStr = QString("font-family: '%1';").arg(m_contentFont.family());
    }
    
    setStyleSheet(QString(
        "DesktopTodoWidget { background: transparent; }"
        "QLabel { background: transparent; %1 }"
        "QWidget { background: transparent; }"
    ).arg(fontStr));
    
    m_dateLabel->setStyleSheet(QString(
        "QLabel { background: transparent; font-size: 16px; font-weight: bold; color: #2c3e50; padding: 5px; %1 }"
    ).arg(fontStr));
    
    m_taskInput->setStyleSheet(QString(
        "QLineEdit { padding: 8px; border: 1px solid #ddd; border-radius: 5px; background: rgba(255,255,255,200); %1 font-size: 13px; }"
        "QLineEdit:focus { border-color: #9b59b6; }"
    ).arg(fontStr));
    
    m_scrollArea->setStyleSheet(QString(
        "QScrollArea { background: transparent; border: 1px solid #ddd; border-radius: 5px; }"
        "QScrollBar:vertical { width: 8px; background: rgba(236,240,241,200); }"
        "QScrollBar::handle:vertical { background: #bdc3c7; border-radius: 4px; min-height: 20px; }"
        "QScrollBar::handle:vertical:hover { background: #95a5a6; }"
    ));
    
    m_taskContainer->setStyleSheet("QWidget { background: transparent; }");
    
    m_countLabel->setStyleSheet(QString("background: transparent; color: #7f8c8d; font-size: 12px; %1").arg(fontStr));
}

void DesktopTodoWidget::updateDateLabel()
{
    QDate today = QDate::currentDate();
    m_dateLabel->setText(QString("%1月%2日").arg(today.month()).arg(today.day()));
}

void DesktopTodoWidget::refreshTasks()
{
    for (auto it = m_taskWidgets.begin(); it != m_taskWidgets.end(); ++it) {
        it.value()->deleteLater();
    }
    m_taskWidgets.clear();
    
    TaskManager *tm = TaskManager::instance();
    QList<TaskData> tasks = tm->tasks();
    
    for (const TaskData &task : tasks) {
        addTaskItem(task);
    }
    
    updateTaskCount();
    updateDateLabel();
}

void DesktopTodoWidget::clearAllTasks()
{
    for (auto it = m_taskWidgets.begin(); it != m_taskWidgets.end(); ++it) {
        it.value()->deleteLater();
    }
    m_taskWidgets.clear();
}

void DesktopTodoWidget::addTaskItem(const TaskData &task)
{
    TaskItemWidget *widget = new TaskItemWidget(task, this);
    widget->setAttribute(Qt::WA_TranslucentBackground);
    if (!m_contentFont.family().isEmpty()) {
        widget->setFont(m_contentFont);
    }
    
    connect(widget, &TaskItemWidget::completedChanged, this, &DesktopTodoWidget::onTaskCompletedChanged);
    connect(widget, &TaskItemWidget::scoreChanged, this, &DesktopTodoWidget::onTaskScoreChanged);
    connect(widget, &TaskItemWidget::deleteRequested, this, &DesktopTodoWidget::onTaskDeleteRequested);
    connect(widget, &TaskItemWidget::editRequested, this, &DesktopTodoWidget::onTaskEditRequested);
    
    m_taskLayout->insertWidget(0, widget);
    m_taskWidgets[task.id] = widget;
}

void DesktopTodoWidget::removeTaskItem(int taskId)
{
    if (m_taskWidgets.contains(taskId)) {
        TaskItemWidget *widget = m_taskWidgets.take(taskId);
        m_taskLayout->removeWidget(widget);
        widget->deleteLater();
    }
}

void DesktopTodoWidget::updateTaskCount()
{
    TaskManager *tm = TaskManager::instance();
    int total = tm->taskCount();
    int completed = tm->completedCount();
    m_countLabel->setText(QString("共 %1 项任务，已完成 %2 项").arg(total).arg(completed));
}

void DesktopTodoWidget::onAddButtonClicked()
{
    QString text = m_taskInput->text().trimmed();
    if (text.isEmpty()) {
        return;
    }
    
    TaskManager::instance()->addTask(text);
    LogManager::instance()->logCreate(text);
    
    m_taskInput->clear();
}

void DesktopTodoWidget::onClearCompletedClicked()
{
    TaskManager::instance()->clearCompletedTasks();
}

void DesktopTodoWidget::onTaskCompletedChanged(int taskId, bool completed)
{
    TaskManager *tm = TaskManager::instance();
    TaskData task = tm->task(taskId);
    
    tm->setTaskCompleted(taskId, completed);
    
    if (completed) {
        LogManager::instance()->logComplete(task.description);
    }
    
    updateTaskCount();
    emit taskCompleted(taskId, completed);
}

void DesktopTodoWidget::onTaskScoreChanged(int taskId, int score)
{
    TaskManager *tm = TaskManager::instance();
    TaskData task = tm->task(taskId);
    
    tm->setTaskScore(taskId, score);
    LogManager::instance()->logRate(task.description, score);
    
    emit taskScoreChanged(taskId, score);
}

void DesktopTodoWidget::onTaskDeleteRequested(int taskId)
{
    TaskManager *tm = TaskManager::instance();
    TaskData task = tm->task(taskId);
    
    tm->removeTask(taskId);
    LogManager::instance()->logDelete(task.description);
    
    emit taskDeleted(taskId);
}

void DesktopTodoWidget::onTaskEditRequested(int taskId, const QString &newText)
{
    TaskManager *tm = TaskManager::instance();
    TaskData task = tm->task(taskId);
    QString oldText = task.description;
    
    tm->updateTask(taskId, newText);
    LogManager::instance()->logEdit(oldText, newText);
    
    emit taskEdited(taskId, newText);
}

void DesktopTodoWidget::paintEvent(QPaintEvent *event)
{
    if (m_backgroundOpacity > 0) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        
        QColor bgColor = m_backgroundColor;
        bgColor.setAlpha(m_backgroundOpacity);
        
        painter.fillRect(rect(), bgColor);
    }
    
    QWidget::paintEvent(event);
}

void DesktopTodoWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = true;
        m_dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
    }
    QWidget::mousePressEvent(event);
}

void DesktopTodoWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isDragging && (event->buttons() & Qt::LeftButton)) {
        move(event->globalPosition().toPoint() - m_dragPosition);
    }
    QWidget::mouseMoveEvent(event);
}

void DesktopTodoWidget::mouseReleaseEvent(QMouseEvent *event)
{
    m_isDragging = false;
    QWidget::mouseReleaseEvent(event);
}

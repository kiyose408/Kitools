#include "todocontroller.h"
#include <QApplication>
#include <QScreen>

TodoController::TodoController(QObject *parent)
    : QObject(parent)
    , m_settingsPanel(nullptr)
    , m_todoWidget(nullptr)
{
    m_settingsPanel = new TodoSettingsPanel();
    m_todoWidget = new DesktopTodoWidget();
    
    m_settingsPanel->setTodoWidget(m_todoWidget);
    
    setupConnections();
}

TodoController::~TodoController()
{
    if (m_todoWidget) {
        m_todoWidget->close();
        m_todoWidget->deleteLater();
    }
    if (m_settingsPanel) {
        m_settingsPanel->deleteLater();
    }
}

void TodoController::setupConnections()
{
}

QWidget* TodoController::settingsPanel()
{
    return m_settingsPanel;
}

DesktopTodoWidget* TodoController::todoWidget()
{
    return m_todoWidget;
}

void TodoController::showTodoWidget()
{
    if (m_todoWidget) {
        QScreen *screen = QApplication::primaryScreen();
        QRect screenGeometry = screen->availableGeometry();
        int x = screenGeometry.width() - m_todoWidget->width() - 50;
        int y = 100;
        m_todoWidget->move(x, y);
        m_todoWidget->show();
        m_todoWidget->raise();
        m_todoWidget->activateWindow();
    }
}

void TodoController::hideTodoWidget()
{
    if (m_todoWidget) {
        m_todoWidget->hide();
    }
}

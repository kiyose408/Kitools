#include "clipboardcontroller.h"
#include <QDebug>

ClipboardController::ClipboardController(QObject* parent)
    : QObject(parent)
    , m_managerWidget(nullptr)
    , m_settingsPanel(nullptr)
{
    qDebug() << "剪贴板控制器构造函数开始";
    
    m_settingsPanel = new ClipboardSettingsPanel();
    qDebug() << "设置面板已创建:" << m_settingsPanel;
    
    if (m_settingsPanel) {
        connect(m_settingsPanel, &ClipboardSettingsPanel::openClipboardManager, this, &ClipboardController::showManager);
        connect(m_settingsPanel, &ClipboardSettingsPanel::backRequested, this, &ClipboardController::deactivated);
        qDebug() << "信号已连接";
    } else {
        qCritical() << "创建设置面板失败!";
    }

    qDebug() << "剪贴板控制器构造函数完成";
}

ClipboardController::~ClipboardController() {
    qDebug() << "剪贴板控制器析构函数";
    if (m_managerWidget) {
        m_managerWidget->deleteLater();
    }
    if (m_settingsPanel) {
        m_settingsPanel->deleteLater();
    }
}

QWidget* ClipboardController::getSettingsPanel() {
    qDebug() << "获取设置面板:" << m_settingsPanel;
    return m_settingsPanel;
}

void ClipboardController::showManager() {
    qDebug() << "显示剪贴板管理器";
    if (!m_managerWidget) {
        qDebug() << "创建新的剪贴板窗口";
        m_managerWidget = new ClipboardWidget();
        connect(m_managerWidget, &ClipboardWidget::closeRequested, this, &ClipboardController::onManagerCloseRequested);
    }
    m_managerWidget->show();
    m_managerWidget->raise();
    m_managerWidget->activateWindow();
    qDebug() << "剪贴板窗口已显示";
}

void ClipboardController::hideManager() {
    qDebug() << "隐藏剪贴板管理器";
    if (m_managerWidget) {
        m_managerWidget->hide();
    }
}

bool ClipboardController::isManagerVisible() const {
    return m_managerWidget && m_managerWidget->isVisible();
}

void ClipboardController::onManagerCloseRequested() {
    qDebug() << "剪贴板窗口关闭请求";
    hideManager();
    emit deactivated();
}

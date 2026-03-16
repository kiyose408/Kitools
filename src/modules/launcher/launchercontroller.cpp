#include "launchercontroller.h"
#include <QDebug>

LauncherController::LauncherController(QObject* parent)
    : QObject(parent)
    , m_indexer(nullptr)
    , m_settingsPanel(nullptr)
{
    qDebug() << "LauncherController 初始化开始";
    
    m_indexer = LauncherIndexer::instance(this);
    m_settingsPanel = new LauncherSettingsPanel();
    
    if (m_settingsPanel) {
        connect(m_settingsPanel, &LauncherSettingsPanel::backRequested, this, &LauncherController::deactivated);
        connect(m_settingsPanel, &LauncherSettingsPanel::refreshRequested, this, &LauncherController::refreshIndex);
    }
    
    qDebug() << "LauncherController 初始化完成";
}

LauncherController::~LauncherController() {
    qDebug() << "LauncherController 析构";
    if (m_settingsPanel) {
        m_settingsPanel->deleteLater();
    }
}

QWidget* LauncherController::getSettingsPanel() {
    return m_settingsPanel;
}

void LauncherController::startIndexing() {
    if (m_indexer) {
        m_indexer->startIndexing();
    }
}

void LauncherController::refreshIndex() {
    if (m_indexer) {
        m_indexer->refreshIndex();
    }
}

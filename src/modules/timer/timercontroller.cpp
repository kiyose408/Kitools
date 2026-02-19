#include "timercontroller.h"
#include "settingsmanager.h"
#include <QApplication>
#include <QScreen>

TimerController::TimerController(QObject *parent)
    : QObject(parent)
    , m_settingsPanel(nullptr)
    , m_overlayWidget(nullptr)
    , m_timer(nullptr)
    , m_remainingSeconds(0)
    , m_currentCycle(0)
    , m_isWorkingPhase(true)
    , m_isCycleMode(false)
    , m_isPaused(false)
{
    m_settingsPanel = new TimerSettingsPanel();
    m_overlayWidget = new OverlayWidget();
    m_timer = new QTimer(this);
    
    m_timer->setInterval(1000);
    
    setupConnections();
    applySettings();
}

TimerController::~TimerController()
{
    if (m_overlayWidget) {
        m_overlayWidget->close();
        m_overlayWidget->deleteLater();
    }
    if (m_settingsPanel) {
        m_settingsPanel->deleteLater();
    }
}

void TimerController::setupConnections()
{
    connect(m_timer, &QTimer::timeout, this, &TimerController::onTimerTick);
    
    connect(m_settingsPanel, &TimerSettingsPanel::startClicked, this, &TimerController::start);
    connect(m_settingsPanel, &TimerSettingsPanel::pauseClicked, this, &TimerController::pause);
    connect(m_settingsPanel, &TimerSettingsPanel::stopClicked, this, &TimerController::stop);
    connect(m_settingsPanel, &TimerSettingsPanel::settingsChanged, this, &TimerController::onSettingsChanged);
}

QWidget* TimerController::settingsPanel()
{
    return m_settingsPanel;
}

OverlayWidget* TimerController::overlayWidget()
{
    return m_overlayWidget;
}

void TimerController::showOverlay()
{
    if (m_overlayWidget) {
        applySettings();
        
        if (!m_timer->isActive() && m_remainingSeconds == 0) {
            QScreen *screen = QApplication::primaryScreen();
            QRect screenGeometry = screen->availableGeometry();
            int x = (screenGeometry.width() - m_overlayWidget->width()) / 2;
            int y = 50;
            m_overlayWidget->move(x, y);
            m_overlayWidget->setPhase(OverlayWidget::TimerPhase::Idle);
        }
        
        m_overlayWidget->show();
    }
}

void TimerController::hideOverlay()
{
    if (m_overlayWidget) {
        m_overlayWidget->hide();
    }
}

bool TimerController::isTimerRunning() const
{
    return m_timer->isActive();
}

bool TimerController::isTimerPaused() const
{
    return m_isPaused && !m_timer->isActive() && m_remainingSeconds > 0;
}

void TimerController::start()
{
    if (m_isPaused) {
        m_isPaused = false;
    } else {
        m_isCycleMode = m_settingsPanel->isCycleMode();
        
        if (m_isCycleMode) {
            m_currentCycle = 1;
            m_isWorkingPhase = true;
            m_remainingSeconds = m_settingsPanel->workMinutes() * 60;
            m_overlayWidget->setPhase(OverlayWidget::TimerPhase::Working);
        } else {
            m_remainingSeconds = calculateTotalSeconds();
            m_overlayWidget->setPhase(OverlayWidget::TimerPhase::Working);
        }
    }
    
    m_overlayWidget->stopFlashing();
    m_timer->start();
    updateOverlay();
}

void TimerController::pause()
{
    m_isPaused = true;
    m_timer->stop();
}

void TimerController::stop()
{
    m_timer->stop();
    m_isPaused = false;
    m_remainingSeconds = 0;
    m_currentCycle = 0;
    
    m_overlayWidget->stopFlashing();
    m_overlayWidget->setTime(0);
    m_overlayWidget->setPhase(OverlayWidget::TimerPhase::Idle);
}

void TimerController::reset()
{
    stop();
    if (m_isCycleMode) {
        m_remainingSeconds = m_settingsPanel->workMinutes() * 60;
    } else {
        m_remainingSeconds = calculateTotalSeconds();
    }
    updateOverlay();
}

void TimerController::onTimerTick()
{
    if (m_remainingSeconds > 0) {
        m_remainingSeconds--;
        updateOverlay();
    } else {
        onTimerComplete();
    }
}

void TimerController::onSettingsChanged()
{
    applySettings();
}

void TimerController::updateOverlay()
{
    if (m_overlayWidget) {
        m_overlayWidget->setTime(m_remainingSeconds);
    }
}

void TimerController::applySettings()
{
    if (m_overlayWidget && m_settingsPanel) {
        m_overlayWidget->setOverlaySize(m_settingsPanel->overlayWidth(), m_settingsPanel->overlayHeight());
        m_overlayWidget->setFontSize(m_settingsPanel->fontSize());
        m_overlayWidget->setBackgroundColor(m_settingsPanel->backgroundColor());
        m_overlayWidget->setTextColor(m_settingsPanel->textColor());
        m_overlayWidget->setBackgroundOpacity(m_settingsPanel->backgroundOpacity());
        m_overlayWidget->setTextOpacity(m_settingsPanel->textOpacity());
    }
}

int TimerController::calculateTotalSeconds() const
{
    return m_settingsPanel->singleHours() * 3600 + 
           m_settingsPanel->singleMinutes() * 60 + 
           m_settingsPanel->singleSeconds();
}

void TimerController::onTimerComplete()
{
    m_timer->stop();
    
    if (m_settingsPanel->soundEnabled()) {
        m_overlayWidget->playBeep();
    }
    
    if (m_settingsPanel->flashEnabled()) {
        m_overlayWidget->startFlashing();
    }
    
    if (m_isCycleMode) {
        startNextPhase();
    } else {
        m_settingsPanel->resetControls();
        emit m_settingsPanel->stopClicked();
    }
}

void TimerController::startNextPhase()
{
    if (m_isWorkingPhase) {
        if (m_currentCycle >= m_settingsPanel->cycleCount()) {
            m_settingsPanel->resetControls();
            emit m_settingsPanel->stopClicked();
            return;
        }
        
        m_isWorkingPhase = false;
        m_remainingSeconds = m_settingsPanel->breakMinutes() * 60;
        m_overlayWidget->setPhase(OverlayWidget::TimerPhase::Breaking);
    } else {
        m_currentCycle++;
        m_isWorkingPhase = true;
        m_remainingSeconds = m_settingsPanel->workMinutes() * 60;
        m_overlayWidget->setPhase(OverlayWidget::TimerPhase::Working);
    }
    
    m_overlayWidget->stopFlashing();
    updateOverlay();
    m_timer->start();
}

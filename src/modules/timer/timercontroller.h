#ifndef TIMERCONTROLLER_H
#define TIMERCONTROLLER_H

#include <QObject>
#include <QTimer>
#include "overlaywidget.h"
#include "timersettingspanel.h"

class TimerController : public QObject
{
    Q_OBJECT

public:
    explicit TimerController(QObject *parent = nullptr);
    ~TimerController();

    QWidget* settingsPanel();
    OverlayWidget* overlayWidget();
    
    void showOverlay();
    void hideOverlay();

public slots:
    void start();
    void pause();
    void stop();
    void reset();

private slots:
    void onTimerTick();
    void onSettingsChanged();

private:
    void setupConnections();
    void updateOverlay();
    void applySettings();
    int calculateTotalSeconds() const;
    void onTimerComplete();
    void startNextPhase();

    TimerSettingsPanel *m_settingsPanel;
    OverlayWidget *m_overlayWidget;
    QTimer *m_timer;
    
    int m_remainingSeconds;
    int m_currentCycle;
    bool m_isWorkingPhase;
    bool m_isCycleMode;
    bool m_isPaused;
};

#endif

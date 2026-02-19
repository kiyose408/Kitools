#ifndef TIMERSETTINGSPANEL_H
#define TIMERSETTINGSPANEL_H

#include <QWidget>
#include <QSpinBox>
#include <QPushButton>
#include <QComboBox>
#include <QSlider>
#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QTabWidget>

class TimerSettingsPanel : public QWidget
{
    Q_OBJECT

public:
    enum class TimerMode {
        SingleCountdown,
        Pomodoro,
        Stopwatch
    };

    explicit TimerSettingsPanel(QWidget *parent = nullptr);
    ~TimerSettingsPanel();

    int singleHours() const;
    int singleMinutes() const;
    int singleSeconds() const;
    
    int workMinutes() const;
    int breakMinutes() const;
    int cycleCount() const;
    
    TimerMode currentMode() const;
    bool isCycleMode() const;
    
    int overlayWidth() const;
    int overlayHeight() const;
    int fontSize() const;
    QColor backgroundColor() const;
    QColor textColor() const;
    int backgroundOpacity() const;
    int textOpacity() const;
    bool soundEnabled() const;
    bool flashEnabled() const;

    void setSingleTime(int hours, int minutes, int seconds);
    void setWorkMinutes(int minutes);
    void setBreakMinutes(int minutes);
    void setCycleCount(int count);
    void setCycleMode(bool isCycle);
    void setOverlaySize(int width, int height);
    void setFontSize(int size);
    void setBackgroundColor(const QColor &color);
    void setTextColor(const QColor &color);
    void setBackgroundOpacity(int opacity);
    void setTextOpacity(int opacity);
    void setSoundEnabled(bool enabled);
    void setFlashEnabled(bool enabled);
    
    void resetControls();
    void setRunningState(bool isRunning, bool isPaused = false);

signals:
    void startClicked();
    void pauseClicked();
    void stopClicked();
    void settingsChanged();
    void backClicked();

private slots:
    void onStartClicked();
    void onPauseClicked();
    void onStopClicked();
    void onModeChanged(int index);
    void onBackgroundColorClicked();
    void onTextColorClicked();
    void onBackClicked();

private:
    void setupUi();
    void setupConnections();
    void loadSettings();
    void saveSettings();

    QTabWidget *m_modeTabWidget;
    
    QWidget *m_singleModeWidget;
    QSpinBox *m_singleHoursSpin;
    QSpinBox *m_singleMinutesSpin;
    QSpinBox *m_singleSecondsSpin;
    
    QWidget *m_cycleModeWidget;
    QSpinBox *m_workMinutesSpin;
    QSpinBox *m_breakMinutesSpin;
    QSpinBox *m_cycleCountSpin;
    
    QWidget *m_stopwatchModeWidget;
    
    QPushButton *m_startBtn;
    QPushButton *m_pauseBtn;
    QPushButton *m_stopBtn;
    QPushButton *m_backBtn;
    
    QSpinBox *m_widthSpin;
    QSpinBox *m_heightSpin;
    QSlider *m_fontSizeSlider;
    QLabel *m_fontSizeLabel;
    QPushButton *m_bgColorBtn;
    QPushButton *m_textColorBtn;
    QSlider *m_bgOpacitySlider;
    QSlider *m_textOpacitySlider;
    QCheckBox *m_soundCheckBox;
    QCheckBox *m_flashCheckBox;
    
    QColor m_backgroundColor;
    QColor m_textColor;
    
    bool m_isRunning;
};

#endif

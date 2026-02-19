#ifndef OVERLAYWIDGET_H
#define OVERLAYWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QColor>
#include <QPoint>

class OverlayWidget : public QWidget
{
    Q_OBJECT

public:
    enum class TimerPhase {
        Idle,
        Working,
        Breaking,
        Stopwatch
    };

    explicit OverlayWidget(QWidget *parent = nullptr);
    ~OverlayWidget();

    void setTime(int seconds);
    void setPhase(TimerPhase phase);
    void setOverlaySize(int width, int height);
    void setFontSize(int size);
    void setBackgroundColor(const QColor &color);
    void setTextColor(const QColor &color);
    void setBackgroundOpacity(int opacity);
    void setTextOpacity(int opacity);
    void setStayOnTop(bool stayOnTop);

    void startFlashing();
    void stopFlashing();
    void playBeep();

signals:
    void dragStarted();
    void positionChanged(const QPoint &pos);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onFlashTimer();

private:
    void setupUi();
    void updateStyleSheet();
    QString formatTime(int totalSeconds) const;
    QString phaseText() const;
    QColor phaseColor() const;

    QLabel *m_timeLabel;
    QLabel *m_phaseLabel;
    
    int m_totalSeconds;
    TimerPhase m_phase;
    
    QColor m_backgroundColor;
    QColor m_textColor;
    int m_backgroundOpacity;
    int m_textOpacity;
    int m_fontSize;
    
    bool m_isDragging;
    QPoint m_dragPosition;
    
    QTimer *m_flashTimer;
    bool m_flashVisible;
    bool m_isFlashing;
};

#endif

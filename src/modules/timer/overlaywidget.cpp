#include "overlaywidget.h"
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QApplication>

OverlayWidget::OverlayWidget(QWidget *parent)
    : QWidget(parent)
    , m_timeLabel(nullptr)
    , m_phaseLabel(nullptr)
    , m_totalSeconds(0)
    , m_phase(TimerPhase::Idle)
    , m_backgroundColor(40, 40, 40)
    , m_textColor(255, 255, 255)
    , m_backgroundOpacity(200)
    , m_textOpacity(255)
    , m_fontSize(32)
    , m_isDragging(false)
    , m_flashTimer(nullptr)
    , m_flashVisible(true)
    , m_isFlashing(false)
{
    setupUi();
    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);
    
    m_flashTimer = new QTimer(this);
    connect(m_flashTimer, &QTimer::timeout, this, &OverlayWidget::onFlashTimer);
}

OverlayWidget::~OverlayWidget()
{
    if (m_flashTimer) {
        m_flashTimer->stop();
    }
}

void OverlayWidget::setupUi()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 5, 10, 5);
    layout->setSpacing(2);

    m_phaseLabel = new QLabel(this);
    m_phaseLabel->setAlignment(Qt::AlignCenter);
    m_phaseLabel->setStyleSheet("font-size: 12px;");
    layout->addWidget(m_phaseLabel);

    m_timeLabel = new QLabel(this);
    m_timeLabel->setAlignment(Qt::AlignCenter);
    m_timeLabel->setStyleSheet(QString("font-size: %1px; font-weight: bold;").arg(m_fontSize));
    layout->addWidget(m_timeLabel);

    setLayout(layout);
    setMinimumSize(150, 60);
}

void OverlayWidget::setTime(int seconds)
{
    m_totalSeconds = seconds;
    m_timeLabel->setText(formatTime(seconds));
}

void OverlayWidget::setPhase(TimerPhase phase)
{
    m_phase = phase;
    m_phaseLabel->setText(phaseText());
    m_phaseLabel->setStyleSheet(QString("font-size: 12px; color: %1;").arg(phaseColor().name()));
}

void OverlayWidget::setOverlaySize(int width, int height)
{
    setFixedSize(width, height);
}

void OverlayWidget::setFontSize(int size)
{
    m_fontSize = size;
    m_timeLabel->setStyleSheet(QString("font-size: %1px; font-weight: bold;").arg(size));
}

void OverlayWidget::setBackgroundColor(const QColor &color)
{
    m_backgroundColor = color;
    update();
}

void OverlayWidget::setTextColor(const QColor &color)
{
    m_textColor = color;
    updateStyleSheet();
}

void OverlayWidget::setBackgroundOpacity(int opacity)
{
    m_backgroundOpacity = opacity;
    update();
}

void OverlayWidget::setTextOpacity(int opacity)
{
    m_textOpacity = opacity;
    updateStyleSheet();
}

void OverlayWidget::setStayOnTop(bool stayOnTop)
{
    if (stayOnTop) {
        setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    } else {
        setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
    }
    show();
}

void OverlayWidget::startFlashing()
{
    if (!m_isFlashing) {
        m_isFlashing = true;
        m_flashVisible = true;
        m_flashTimer->start(500);
    }
}

void OverlayWidget::stopFlashing()
{
    m_isFlashing = false;
    m_flashTimer->stop();
    m_flashVisible = true;
    m_timeLabel->show();
    m_phaseLabel->show();
    update();
}

void OverlayWidget::playBeep()
{
    QApplication::beep();
}

void OverlayWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = true;
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        emit dragStarted();
    }
    QWidget::mousePressEvent(event);
}

void OverlayWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isDragging && (event->buttons() & Qt::LeftButton)) {
        move(event->globalPos() - m_dragPosition);
        emit positionChanged(pos());
    }
    QWidget::mouseMoveEvent(event);
}

void OverlayWidget::mouseReleaseEvent(QMouseEvent *event)
{
    m_isDragging = false;
    QWidget::mouseReleaseEvent(event);
}

void OverlayWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    QColor bgColor = m_backgroundColor;
    bgColor.setAlpha(m_backgroundOpacity);
    
    if (m_isFlashing && !m_flashVisible) {
        bgColor.setAlpha(0);
    }
    
    painter.fillRect(rect(), bgColor);
    
    QWidget::paintEvent(event);
}

void OverlayWidget::onFlashTimer()
{
    m_flashVisible = !m_flashVisible;
    m_timeLabel->setVisible(m_flashVisible);
    m_phaseLabel->setVisible(m_flashVisible);
    update();
}

void OverlayWidget::updateStyleSheet()
{
    QColor textColor = m_textColor;
    textColor.setAlpha(m_textOpacity);
    
    m_timeLabel->setStyleSheet(QString("font-size: %1px; font-weight: bold; color: %2;")
        .arg(m_fontSize)
        .arg(textColor.name()));
}

QString OverlayWidget::formatTime(int totalSeconds) const
{
    int hours = totalSeconds / 3600;
    int minutes = (totalSeconds % 3600) / 60;
    int seconds = totalSeconds % 60;
    
    if (hours > 0) {
        return QString("%1:%2:%3")
            .arg(hours, 2, 10, QChar('0'))
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0'));
    }
    return QString("%1:%2")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));
}

QString OverlayWidget::phaseText() const
{
    switch (m_phase) {
        case TimerPhase::Working:
            return tr("工作中");
        case TimerPhase::Breaking:
            return tr("休息中");
        case TimerPhase::Stopwatch:
            return tr("计时中");
        default:
            return tr("就绪");
    }
}

QColor OverlayWidget::phaseColor() const
{
    switch (m_phase) {
        case TimerPhase::Working:
            return QColor(231, 76, 60);
        case TimerPhase::Breaking:
            return QColor(46, 204, 113);
        case TimerPhase::Stopwatch:
            return QColor(52, 152, 219);
        default:
            return QColor(149, 165, 166);
    }
}

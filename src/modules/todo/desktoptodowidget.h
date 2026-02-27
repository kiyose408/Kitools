#ifndef DESKTOPTODOWIDGET_H
#define DESKTOPTODOWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QMap>
#include <QColor>
#include <QFont>
#include <QTimer>
#include "tododatatypes.h"

class TaskItemWidget;

class DesktopTodoWidget : public QWidget
{
    Q_OBJECT

public:
    enum class DisplayMode {
        AlwaysOnTop,
        DesktopFixed
    };

    explicit DesktopTodoWidget(QWidget *parent = nullptr);
    ~DesktopTodoWidget();
    
    void setDisplayMode(DisplayMode mode);
    DisplayMode displayMode() const { return m_displayMode; }
    
    void setBackgroundColor(const QColor &color);
    QColor backgroundColor() const { return m_backgroundColor; }
    
    void setDarkMode(bool enabled);
    bool isDarkMode() const { return m_isDarkMode; }
    
    void setContentFont(const QFont &font);
    QFont contentFont() const { return m_contentFont; }
    
    void refreshTasks();
    void clearAllTasks();
    
    void setLocked(bool locked);
    bool isLocked() const { return m_isLocked; }

signals:
    void taskAdded(const QString &description);
    void taskCompleted(int taskId, bool completed);
    void taskScoreChanged(int taskId, int score);
    void taskDeleted(int taskId);
    void taskEdited(int taskId, const QString &newText);
    void displayModeChanged(DisplayMode mode);
    void lockChanged(bool locked);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onAddButtonClicked();
    void onClearCompletedClicked();
    void onPinButtonClicked();
    void onModeButtonClicked();
    void onStayOnTop();
    void onTaskCompletedChanged(int taskId, bool completed);
    void onTaskScoreChanged(int taskId, int score);
    void onTaskDeleteRequested(int taskId);
    void onTaskEditRequested(int taskId, const QString &newText);

private:
    void setupUi();
    void setupConnections();
    void addTaskItem(const TaskData &task);
    void removeTaskItem(int taskId);
    void updateTaskCount();
    void applyWindowFlags();
    void updateDateLabel();
    void applyStyleSheet();
    void updatePinButtonStyle();
    void updateModeButtonStyle();
    void moveToTopRight();

    QVBoxLayout *m_mainLayout;
    QWidget *m_headerWidget;
    QWidget *m_titleWidget;
    QLabel *m_dateLabel;
    QPushButton *m_pinBtn;
    QPushButton *m_modeBtn;
    QPushButton *m_clearCompletedBtn;
    QLineEdit *m_taskInput;
    QPushButton *m_addBtn;
    QScrollArea *m_scrollArea;
    QWidget *m_taskContainer;
    QVBoxLayout *m_taskLayout;
    QLabel *m_countLabel;
    
    QMap<int, TaskItemWidget*> m_taskWidgets;
    
    DisplayMode m_displayMode;
    bool m_isDragging;
    QPoint m_dragPosition;
    bool m_isLocked;
    
    QColor m_backgroundColor;
    bool m_isDarkMode;
    QFont m_contentFont;
    
    QTimer *m_stayOnTopTimer;
};

#endif

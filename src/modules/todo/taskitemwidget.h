#ifndef TASKITEMWIDGET_H
#define TASKITEMWIDGET_H

#include <QWidget>
#include <QCheckBox>
#include <QLabel>
#include <QSlider>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLineEdit>
#include "tododatatypes.h"

class TaskItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TaskItemWidget(const TaskData &task, QWidget *parent = nullptr);
    ~TaskItemWidget();
    
    int taskId() const { return m_taskId; }
    void setTaskData(const TaskData &task);
    void setEditMode(bool editMode);
    void setDarkMode(bool enabled);

signals:
    void completedChanged(int taskId, bool completed);
    void scoreChanged(int taskId, int score);
    void deleteRequested(int taskId);
    void editRequested(int taskId, const QString &newText);

private slots:
    void onCheckBoxToggled(bool checked);
    void onSliderValueChanged(int value);
    void onSliderReleased();
    void onDeleteClicked();
    void onEditClicked();
    void onEditFinished();

private:
    void setupUi();
    void updateAppearance();

    int m_taskId;
    QString m_description;
    bool m_isCompleted;
    int m_score;
    bool m_editMode;
    int m_lastEmittedScore;
    bool m_isDarkMode;
    
    QCheckBox *m_checkBox;
    QLabel *m_descriptionLabel;
    QLineEdit *m_editLineEdit;
    QSlider *m_scoreSlider;
    QLabel *m_scoreLabel;
    QPushButton *m_editBtn;
    QPushButton *m_deleteBtn;
    QWidget *m_scoreWidget;
};

#endif

#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <QObject>
#include <QList>
#include "tododatatypes.h"

class TaskManager : public QObject
{
    Q_OBJECT

public:
    static TaskManager* instance();
    
    QList<TaskData> tasks() const;
    TaskData task(int id) const;
    int taskCount() const;
    int completedCount() const;
    
    int addTask(const QString &description);
    bool updateTask(int id, const QString &description);
    bool setTaskCompleted(int id, bool completed);
    bool setTaskScore(int id, int score);
    bool removeTask(int id);
    void clearCompletedTasks();
    void clearAllTasks();
    
    void loadTasks();
    void saveTasks();

signals:
    void taskAdded(const TaskData &task);
    void taskUpdated(int id);
    void taskRemoved(int id);
    void taskCompleted(int id, bool completed);
    void taskScoreChanged(int id, int score);
    void tasksCleared();

private:
    explicit TaskManager(QObject *parent = nullptr);
    ~TaskManager();
    TaskManager(const TaskManager&) = delete;
    TaskManager& operator=(const TaskManager&) = delete;
    
    int generateNewId();
    QString tasksFilePath() const;

    QList<TaskData> m_tasks;
    int m_nextId;
};

#endif

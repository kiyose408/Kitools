#include "taskmanager.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QStandardPaths>

TaskManager* TaskManager::instance()
{
    static TaskManager instance;
    return &instance;
}

TaskManager::TaskManager(QObject *parent)
    : QObject(parent)
    , m_nextId(1)
{
    loadTasks();
}

TaskManager::~TaskManager()
{
    saveTasks();
}

QList<TaskData> TaskManager::tasks() const
{
    return m_tasks;
}

TaskData TaskManager::task(int id) const
{
    for (const TaskData &t : m_tasks) {
        if (t.id == id) {
            return t;
        }
    }
    return TaskData();
}

int TaskManager::taskCount() const
{
    return m_tasks.size();
}

int TaskManager::completedCount() const
{
    int count = 0;
    for (const TaskData &t : m_tasks) {
        if (t.isCompleted) {
            count++;
        }
    }
    return count;
}

int TaskManager::addTask(const QString &description)
{
    TaskData newTask(m_nextId++, description);
    m_tasks.append(newTask);
    saveTasks();
    emit taskAdded(newTask);
    return newTask.id;
}

bool TaskManager::updateTask(int id, const QString &description)
{
    for (int i = 0; i < m_tasks.size(); ++i) {
        if (m_tasks[i].id == id) {
            m_tasks[i].description = description;
            saveTasks();
            emit taskUpdated(id);
            return true;
        }
    }
    return false;
}

bool TaskManager::setTaskCompleted(int id, bool completed)
{
    for (int i = 0; i < m_tasks.size(); ++i) {
        if (m_tasks[i].id == id) {
            if (m_tasks[i].isCompleted != completed) {
                m_tasks[i].isCompleted = completed;
                if (completed) {
                    m_tasks[i].completionTime = QDateTime::currentDateTime();
                } else {
                    m_tasks[i].completionTime = QDateTime();
                    m_tasks[i].completionScore = 0;
                }
                saveTasks();
                emit taskCompleted(id, completed);
            }
            return true;
        }
    }
    return false;
}

bool TaskManager::setTaskScore(int id, int score)
{
    for (int i = 0; i < m_tasks.size(); ++i) {
        if (m_tasks[i].id == id) {
            if (m_tasks[i].completionScore != score) {
                m_tasks[i].completionScore = score;
                saveTasks();
                emit taskScoreChanged(id, score);
            }
            return true;
        }
    }
    return false;
}

bool TaskManager::removeTask(int id)
{
    for (int i = 0; i < m_tasks.size(); ++i) {
        if (m_tasks[i].id == id) {
            m_tasks.removeAt(i);
            saveTasks();
            emit taskRemoved(id);
            return true;
        }
    }
    return false;
}

void TaskManager::clearCompletedTasks()
{
    for (int i = m_tasks.size() - 1; i >= 0; --i) {
        if (m_tasks[i].isCompleted) {
            int id = m_tasks[i].id;
            m_tasks.removeAt(i);
            emit taskRemoved(id);
        }
    }
    saveTasks();
}

void TaskManager::clearAllTasks()
{
    m_tasks.clear();
    saveTasks();
    emit tasksCleared();
}

void TaskManager::loadTasks()
{
    QString filePath = tasksFilePath();
    QFile file(filePath);
    
    if (!file.exists()) {
        return;
    }
    
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray()) {
        return;
    }
    
    m_tasks.clear();
    QJsonArray array = doc.array();
    
    for (const QJsonValue &value : array) {
        if (value.isObject()) {
            TaskData task = TaskData::fromJson(value.toObject());
            m_tasks.append(task);
            if (task.id >= m_nextId) {
                m_nextId = task.id + 1;
            }
        }
    }
}

void TaskManager::saveTasks()
{
    QString filePath = tasksFilePath();
    QDir dir = QFileInfo(filePath).dir();
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    
    QJsonArray array;
    for (const TaskData &task : m_tasks) {
        array.append(task.toJson());
    }
    
    QJsonDocument doc(array);
    
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}

int TaskManager::generateNewId()
{
    return m_nextId++;
}

QString TaskManager::tasksFilePath() const
{
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    return dataPath + "/tasks.json";
}

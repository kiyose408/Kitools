#include "notesreminder.h"
#include "notesmanager.h"
#include <QApplication>
#include <QDateTime>

NotesReminder* NotesReminder::instance()
{
    static NotesReminder instance;
    return &instance;
}

NotesReminder::NotesReminder(QObject *parent)
    : QObject(parent)
    , m_checkTimer(nullptr)
    , m_isMonitoring(false)
{
    m_checkTimer = new QTimer(this);
    connect(m_checkTimer, &QTimer::timeout, this, &NotesReminder::checkReminders);
}

NotesReminder::~NotesReminder()
{
    stopMonitoring();
}

void NotesReminder::startMonitoring()
{
    if (!m_isMonitoring) {
        m_isMonitoring = true;
        
        NotesManager *nm = NotesManager::instance();
        QList<NoteData> notes = nm->notesWithReminders();
        QDateTime now = QDateTime::currentDateTime();
        for (const NoteData &note : notes) {
            if (note.reminderTime <= now) {
                m_triggeredNotes.append(note.id);
            }
        }
        
        m_checkTimer->start(1000);
    }
}

void NotesReminder::stopMonitoring()
{
    if (m_isMonitoring) {
        m_isMonitoring = false;
        m_checkTimer->stop();
    }
}

void NotesReminder::refreshReminders()
{
    m_triggeredNotes.clear();
    checkReminders();
}

void NotesReminder::checkReminders()
{
    NotesManager *nm = NotesManager::instance();
    QList<NoteData> notes = nm->notesWithReminders();
    QDateTime now = QDateTime::currentDateTime();
    
    for (const NoteData &note : notes) {
        if (m_triggeredNotes.contains(note.id)) {
            continue;
        }
        
        if (!note.hasReminder) {
            continue;
        }
        
        if (!note.reminderTime.isValid()) {
            continue;
        }
        
        if (note.reminderTime <= now) {
            m_triggeredNotes.append(note.id);
            showNotification(note);
            emit reminderTriggered(note);
        }
    }
}

void NotesReminder::showNotification(const NoteData &note)
{
    QString title = note.title.isEmpty() ? "便签提醒" : note.title;
    QString message = note.content;
    if (message.length() > 100) {
        message = message.left(100) + "...";
    }
    
    QSystemTrayIcon *trayIcon = qApp->property("systemTrayIcon").value<QSystemTrayIcon*>();
    if (trayIcon) {
        trayIcon->showMessage(title, message, QSystemTrayIcon::Information, 5000);
    }
}

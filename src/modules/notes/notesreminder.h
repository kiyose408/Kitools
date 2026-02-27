#ifndef NOTESREMINDER_H
#define NOTESREMINDER_H

#include <QObject>
#include <QTimer>
#include <QSystemTrayIcon>
#include "notesdatatypes.h"

class NotesReminder : public QObject
{
    Q_OBJECT

public:
    static NotesReminder* instance();
    
    void startMonitoring();
    void stopMonitoring();
    bool isMonitoring() const { return m_isMonitoring; }
    
    void refreshReminders();

signals:
    void reminderTriggered(const NoteData &note);

private slots:
    void checkReminders();

private:
    explicit NotesReminder(QObject *parent = nullptr);
    ~NotesReminder();
    NotesReminder(const NotesReminder&) = delete;
    NotesReminder& operator=(const NotesReminder&) = delete;
    
    void showNotification(const NoteData &note);

    QTimer *m_checkTimer;
    bool m_isMonitoring;
    QList<int> m_triggeredNotes;
};

#endif

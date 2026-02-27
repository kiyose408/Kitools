#ifndef NOTESMANAGER_H
#define NOTESMANAGER_H

#include <QObject>
#include <QList>
#include "notesdatatypes.h"

class NotesManager : public QObject
{
    Q_OBJECT

public:
    static NotesManager* instance();
    
    QList<NoteData> notes() const;
    NoteData note(int id) const;
    int noteCount() const;
    
    void addNote(const NoteData &note);
    void updateNote(const NoteData &note);
    void removeNote(int id);
    void clearAllNotes();
    
    QList<NoteData> searchNotes(const QString &keyword) const;
    QList<NoteData> notesWithReminders() const;
    
    int nextId();

signals:
    void noteAdded(const NoteData &note);
    void noteUpdated(const NoteData &note);
    void noteRemoved(int id);
    void notesCleared();

private:
    explicit NotesManager(QObject *parent = nullptr);
    ~NotesManager();
    NotesManager(const NotesManager&) = delete;
    NotesManager& operator=(const NotesManager&) = delete;
    
    void loadNotes();
    void saveNotes();
    QString dataFilePath() const;

    QList<NoteData> m_notes;
    int m_nextId;
};

#endif

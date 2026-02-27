#include "notesmanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QStandardPaths>
#include <QDir>

NotesManager* NotesManager::instance()
{
    static NotesManager instance;
    return &instance;
}

NotesManager::NotesManager(QObject *parent)
    : QObject(parent)
    , m_nextId(1)
{
    loadNotes();
}

NotesManager::~NotesManager()
{
    saveNotes();
}

QString NotesManager::dataFilePath() const
{
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QDir dir(dataPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    return dataPath + "/notes.json";
}

void NotesManager::loadNotes()
{
    QString filePath = dataFilePath();
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
    
    QJsonArray array = doc.array();
    m_notes.clear();
    m_nextId = 1;
    
    for (const QJsonValue &value : array) {
        if (value.isObject()) {
            NoteData note = NoteData::fromJson(value.toObject());
            m_notes.append(note);
            if (note.id >= m_nextId) {
                m_nextId = note.id + 1;
            }
        }
    }
}

void NotesManager::saveNotes()
{
    QString filePath = dataFilePath();
    QFile file(filePath);
    
    if (!file.open(QIODevice::WriteOnly)) {
        return;
    }
    
    QJsonArray array;
    for (const NoteData &note : m_notes) {
        array.append(note.toJson());
    }
    
    QJsonDocument doc(array);
    file.write(doc.toJson());
    file.close();
}

QList<NoteData> NotesManager::notes() const
{
    return m_notes;
}

NoteData NotesManager::note(int id) const
{
    for (const NoteData &n : m_notes) {
        if (n.id == id) {
            return n;
        }
    }
    return NoteData();
}

int NotesManager::noteCount() const
{
    return m_notes.count();
}

int NotesManager::nextId()
{
    return m_nextId++;
}

void NotesManager::addNote(const NoteData &note)
{
    NoteData newNote = note;
    newNote.id = nextId();
    newNote.creationTime = QDateTime::currentDateTime();
    newNote.modificationTime = QDateTime::currentDateTime();
    m_notes.prepend(newNote);
    saveNotes();
    emit noteAdded(newNote);
}

void NotesManager::updateNote(const NoteData &note)
{
    for (int i = 0; i < m_notes.size(); ++i) {
        if (m_notes[i].id == note.id) {
            NoteData updatedNote = note;
            updatedNote.modificationTime = QDateTime::currentDateTime();
            m_notes[i] = updatedNote;
            saveNotes();
            emit noteUpdated(updatedNote);
            break;
        }
    }
}

void NotesManager::removeNote(int id)
{
    for (int i = 0; i < m_notes.size(); ++i) {
        if (m_notes[i].id == id) {
            m_notes.removeAt(i);
            saveNotes();
            emit noteRemoved(id);
            break;
        }
    }
}

void NotesManager::clearAllNotes()
{
    m_notes.clear();
    saveNotes();
    emit notesCleared();
}

QList<NoteData> NotesManager::searchNotes(const QString &keyword) const
{
    QList<NoteData> results;
    QString lowerKeyword = keyword.toLower();
    
    for (const NoteData &note : m_notes) {
        if (note.title.toLower().contains(lowerKeyword) ||
            note.content.toLower().contains(lowerKeyword)) {
            results.append(note);
        }
    }
    
    return results;
}

QList<NoteData> NotesManager::notesWithReminders() const
{
    QList<NoteData> results;
    
    for (const NoteData &note : m_notes) {
        if (note.hasReminder && note.reminderTime.isValid()) {
            results.append(note);
        }
    }
    
    return results;
}

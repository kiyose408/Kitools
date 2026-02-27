#ifndef NOTESCONTROLLER_H
#define NOTESCONTROLLER_H

#include <QObject>

class NotesWidget;
class NotesSettingsPanel;

class NotesController : public QObject
{
    Q_OBJECT

public:
    explicit NotesController(QObject *parent = nullptr);
    ~NotesController();
    
    QWidget* settingsPanel();
    NotesWidget* notesWidget();
    
    void showAllNotes();
    void hideAllNotes();

signals:
    void backToHomeRequested();

private:
    void setupConnections();

    NotesWidget *m_notesWidget;
    NotesSettingsPanel *m_settingsPanel;
};

#endif

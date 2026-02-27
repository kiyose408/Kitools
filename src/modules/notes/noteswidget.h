#ifndef NOTESWIDGET_H
#define NOTESWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QMap>
#include "notesdatatypes.h"

class NotesItemWidget;

class NotesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NotesWidget(QWidget *parent = nullptr);
    ~NotesWidget();
    
    void refreshNotes();
    void showAllNotes();
    void hideAllNotes();

signals:
    void backToHomeRequested();

private slots:
    void onAddButtonClicked();
    void onNoteDeleteRequested(int noteId);
    void onNoteDataChanged(int noteId);
    void onReminderTriggered(const NoteData &note);

private:
    void setupUi();
    void setupConnections();
    void createNoteWidget(const NoteData &note);
    void removeNoteWidget(int noteId);
    void updateNoteCount();
    void stopFlashing();

    QVBoxLayout *m_mainLayout;
    QLabel *m_countLabel;
    QPushButton *m_addBtn;
    QLineEdit *m_searchInput;
    
    QMap<int, NotesItemWidget*> m_noteWidgets;
};

#endif

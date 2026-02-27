#ifndef NOTESSETTINGSPANEL_H
#define NOTESSETTINGSPANEL_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>

class NotesWidget;

class NotesSettingsPanel : public QWidget
{
    Q_OBJECT

public:
    explicit NotesSettingsPanel(QWidget *parent = nullptr);
    ~NotesSettingsPanel();
    
    void setNotesWidget(NotesWidget *widget);
    
signals:
    void backClicked();
    void showNotesRequested();
    void hideNotesRequested();

private slots:
    void onShowNotesClicked();
    void onHideNotesClicked();
    void onAddNoteClicked();
    void onClearAllClicked();
    void onBackClicked();

private:
    void setupUi();
    void setupConnections();
    void updateStats();

    NotesWidget *m_notesWidget;
    
    QPushButton *m_addBtn;
    QPushButton *m_showBtn;
    QPushButton *m_hideBtn;
    QPushButton *m_clearAllBtn;
    QPushButton *m_backBtn;
    QLabel *m_statsLabel;
};

#endif

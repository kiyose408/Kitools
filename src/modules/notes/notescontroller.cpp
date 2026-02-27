#include "notescontroller.h"
#include "noteswidget.h"
#include "notessettingspanel.h"
#include "notesreminder.h"

NotesController::NotesController(QObject *parent)
    : QObject(parent)
    , m_notesWidget(nullptr)
    , m_settingsPanel(nullptr)
{
    m_notesWidget = new NotesWidget();
    m_settingsPanel = new NotesSettingsPanel();
    m_settingsPanel->setNotesWidget(m_notesWidget);
    
    setupConnections();
    
    NotesReminder::instance()->startMonitoring();
}

NotesController::~NotesController()
{
    if (m_notesWidget) {
        m_notesWidget->deleteLater();
    }
}

QWidget* NotesController::settingsPanel()
{
    return m_settingsPanel;
}

NotesWidget* NotesController::notesWidget()
{
    return m_notesWidget;
}

void NotesController::showAllNotes()
{
    if (m_notesWidget) {
        m_notesWidget->showAllNotes();
    }
}

void NotesController::hideAllNotes()
{
    if (m_notesWidget) {
        m_notesWidget->hideAllNotes();
    }
}

void NotesController::setupConnections()
{
    connect(m_settingsPanel, &NotesSettingsPanel::backClicked, this, &NotesController::backToHomeRequested);
}

#include "noteswidget.h"
#include "notesitemwidget.h"
#include "notesmanager.h"
#include "notesreminder.h"
#include <QHBoxLayout>
#include <QApplication>
#include <QScreen>
#include <QMessageBox>

NotesWidget::NotesWidget(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_countLabel(nullptr)
    , m_addBtn(nullptr)
    , m_searchInput(nullptr)
{
    setupUi();
    setupConnections();
    refreshNotes();
}

NotesWidget::~NotesWidget()
{
}

void NotesWidget::setupUi()
{
    setWindowTitle("便签管理");
    setMinimumSize(300, 200);
    resize(350, 250);
    
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(15, 15, 15, 15);
    m_mainLayout->setSpacing(15);
    
    QHBoxLayout *headerLayout = new QHBoxLayout();
    
    QLabel *titleLabel = new QLabel("桌面便签", this);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50;");
    headerLayout->addWidget(titleLabel);
    
    headerLayout->addStretch();
    
    m_addBtn = new QPushButton("+ 新建便签", this);
    m_addBtn->setStyleSheet(
        "QPushButton { background-color: #e67e22; color: white; border: none; padding: 8px 15px; "
        "font-size: 14px; border-radius: 5px; }"
        "QPushButton:hover { background-color: #d35400; }"
    );
    headerLayout->addWidget(m_addBtn);
    
    m_mainLayout->addLayout(headerLayout);
    
    m_searchInput = new QLineEdit(this);
    m_searchInput->setPlaceholderText("搜索便签...");
    m_searchInput->setStyleSheet(
        "QLineEdit { padding: 8px; border: 1px solid #ddd; border-radius: 5px; }"
        "QLineEdit:focus { border-color: #e67e22; }"
    );
    m_mainLayout->addWidget(m_searchInput);
    
    m_countLabel = new QLabel("共 0 条便签", this);
    m_countLabel->setStyleSheet("font-size: 14px; color: #7f8c8d;");
    m_countLabel->setAlignment(Qt::AlignCenter);
    m_mainLayout->addWidget(m_countLabel);
    
    QLabel *tipLabel = new QLabel("提示：每个便签都是独立窗口，可拖动到桌面任意位置", this);
    tipLabel->setStyleSheet("font-size: 12px; color: #95a5a6;");
    tipLabel->setAlignment(Qt::AlignCenter);
    tipLabel->setWordWrap(true);
    m_mainLayout->addWidget(tipLabel);
    
    m_mainLayout->addStretch();
}

void NotesWidget::setupConnections()
{
    connect(m_addBtn, &QPushButton::clicked, this, &NotesWidget::onAddButtonClicked);
    
    NotesManager *nm = NotesManager::instance();
    connect(nm, &NotesManager::noteAdded, this, [this](const NoteData &note) {
        createNoteWidget(note);
        updateNoteCount();
    });
    connect(nm, &NotesManager::noteRemoved, this, [this](int noteId) {
        removeNoteWidget(noteId);
        updateNoteCount();
    });
    connect(nm, &NotesManager::notesCleared, this, [this]() {
        for (auto it = m_noteWidgets.begin(); it != m_noteWidgets.end(); ++it) {
            it.value()->close();
            it.value()->deleteLater();
        }
        m_noteWidgets.clear();
        updateNoteCount();
    });
    
    NotesReminder *nr = NotesReminder::instance();
    connect(nr, &NotesReminder::reminderTriggered, this, &NotesWidget::onReminderTriggered);
}

void NotesWidget::refreshNotes()
{
    NotesManager *nm = NotesManager::instance();
    QList<NoteData> notes = nm->notes();
    
    for (const NoteData &note : notes) {
        if (!m_noteWidgets.contains(note.id)) {
            createNoteWidget(note);
        }
    }
    
    updateNoteCount();
}

void NotesWidget::showAllNotes()
{
    for (auto it = m_noteWidgets.begin(); it != m_noteWidgets.end(); ++it) {
        it.value()->show();
        it.value()->raise();
    }
}

void NotesWidget::hideAllNotes()
{
    for (auto it = m_noteWidgets.begin(); it != m_noteWidgets.end(); ++it) {
        it.value()->hide();
    }
}

void NotesWidget::createNoteWidget(const NoteData &note)
{
    NotesItemWidget *widget = new NotesItemWidget(note);
    
    connect(widget, &NotesItemWidget::deleteRequested, this, &NotesWidget::onNoteDeleteRequested);
    connect(widget, &NotesItemWidget::dataChanged, this, &NotesWidget::onNoteDataChanged);
    
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();
    int offsetX = (m_noteWidgets.size() % 5) * 50;
    int offsetY = (m_noteWidgets.size() % 5) * 50;
    widget->move(screenGeometry.width() - 380 - offsetX, 50 + offsetY);
    
    widget->show();
    widget->raise();
    widget->activateWindow();
    
    if (note.content.isEmpty()) {
        widget->enterEditMode();
    }
    
    m_noteWidgets[note.id] = widget;
}

void NotesWidget::removeNoteWidget(int noteId)
{
    if (m_noteWidgets.contains(noteId)) {
        NotesItemWidget *widget = m_noteWidgets.take(noteId);
        widget->close();
        widget->deleteLater();
    }
}

void NotesWidget::updateNoteCount()
{
    NotesManager *nm = NotesManager::instance();
    int count = nm->noteCount();
    m_countLabel->setText(QString("共 %1 条便签").arg(count));
}

void NotesWidget::onAddButtonClicked()
{
    NoteData newNote;
    newNote.content = "";
    newNote.color = NoteColor::Yellow;
    NotesManager::instance()->addNote(newNote);
}

void NotesWidget::onNoteDeleteRequested(int noteId)
{
    NotesItemWidget *widget = m_noteWidgets.value(noteId, nullptr);
    if (widget) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            widget,
            "确认删除",
            "确定要删除这条便签吗？",
            QMessageBox::Yes | QMessageBox::No
        );
        
        if (reply == QMessageBox::Yes) {
            NotesManager::instance()->removeNote(noteId);
        }
    }
}

void NotesWidget::onNoteDataChanged(int noteId)
{
    if (m_noteWidgets.contains(noteId)) {
        NotesItemWidget *widget = m_noteWidgets[noteId];
        NoteData note = widget->getNoteData();
        NotesManager::instance()->updateNote(note);
    }
}

void NotesWidget::onReminderTriggered(const NoteData &note)
{
    if (m_noteWidgets.contains(note.id)) {
        m_noteWidgets[note.id]->startFlashing();
    }
}

void NotesWidget::stopFlashing()
{
    for (auto it = m_noteWidgets.begin(); it != m_noteWidgets.end(); ++it) {
        it.value()->stopFlashing();
    }
}

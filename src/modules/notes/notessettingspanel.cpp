#include "notessettingspanel.h"
#include "noteswidget.h"
#include "notesmanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMessageBox>

NotesSettingsPanel::NotesSettingsPanel(QWidget *parent)
    : QWidget(parent)
    , m_notesWidget(nullptr)
    , m_addBtn(nullptr)
    , m_showBtn(nullptr)
    , m_hideBtn(nullptr)
    , m_clearAllBtn(nullptr)
    , m_backBtn(nullptr)
    , m_statsLabel(nullptr)
{
    setupUi();
    setupConnections();
}

NotesSettingsPanel::~NotesSettingsPanel()
{
}

void NotesSettingsPanel::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(15);
    
    QHBoxLayout *headerLayout = new QHBoxLayout();
    m_backBtn = new QPushButton("← 返回", this);
    m_backBtn->setStyleSheet("QPushButton { background: none; border: none; color: #3498db; font-size: 14px; } QPushButton:hover { color: #2980b9; }");
    headerLayout->addWidget(m_backBtn);
    
    QLabel *titleLabel = new QLabel("桌面便签", this);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50;");
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    mainLayout->addLayout(headerLayout);
    
    QHBoxLayout *controlLayout = new QHBoxLayout();
    controlLayout->setSpacing(10);
    
    m_addBtn = new QPushButton("+ 新建便签", this);
    m_addBtn->setStyleSheet(
        "QPushButton { background-color: #e67e22; color: white; border: none; padding: 10px 20px; "
        "font-size: 14px; border-radius: 5px; }"
        "QPushButton:hover { background-color: #d35400; }"
    );
    controlLayout->addWidget(m_addBtn);
    
    m_showBtn = new QPushButton("显示所有", this);
    m_showBtn->setStyleSheet(
        "QPushButton { background-color: #27ae60; color: white; border: none; padding: 10px 20px; "
        "font-size: 14px; border-radius: 5px; }"
        "QPushButton:hover { background-color: #2ecc71; }"
    );
    controlLayout->addWidget(m_showBtn);
    
    m_hideBtn = new QPushButton("隐藏所有", this);
    m_hideBtn->setStyleSheet(
        "QPushButton { background-color: #95a5a6; color: white; border: none; padding: 10px 20px; "
        "font-size: 14px; border-radius: 5px; }"
        "QPushButton:hover { background-color: #7f8c8d; }"
    );
    controlLayout->addWidget(m_hideBtn);
    
    controlLayout->addStretch();
    mainLayout->addLayout(controlLayout);
    
    QGroupBox *infoGroup = new QGroupBox("使用说明", this);
    infoGroup->setStyleSheet("QGroupBox { font-weight: bold; border: 1px solid #ddd; border-radius: 5px; margin-top: 10px; padding-top: 10px; }");
    QVBoxLayout *infoLayout = new QVBoxLayout(infoGroup);
    
    QLabel *tip1 = new QLabel("• 每个便签都是独立的窗口", this);
    tip1->setStyleSheet("font-size: 13px; color: #34495e;");
    infoLayout->addWidget(tip1);
    
    QLabel *tip2 = new QLabel("• 可以拖动便签到桌面任意位置", this);
    tip2->setStyleSheet("font-size: 13px; color: #34495e;");
    infoLayout->addWidget(tip2);
    
    QLabel *tip3 = new QLabel("• 双击便签或点击✏按钮进入编辑模式", this);
    tip3->setStyleSheet("font-size: 13px; color: #34495e;");
    infoLayout->addWidget(tip3);
    
    QLabel *tip4 = new QLabel("• 支持设置提醒时间，到时自动通知", this);
    tip4->setStyleSheet("font-size: 13px; color: #34495e;");
    infoLayout->addWidget(tip4);
    
    mainLayout->addWidget(infoGroup);
    
    QGroupBox *statsGroup = new QGroupBox("统计信息", this);
    statsGroup->setStyleSheet("QGroupBox { font-weight: bold; border: 1px solid #ddd; border-radius: 5px; margin-top: 10px; padding-top: 10px; }");
    QVBoxLayout *statsLayout = new QVBoxLayout(statsGroup);
    
    m_statsLabel = new QLabel("共 0 条便签", this);
    m_statsLabel->setStyleSheet("font-size: 14px; color: #2c3e50;");
    statsLayout->addWidget(m_statsLabel);
    
    mainLayout->addWidget(statsGroup);
    
    QGroupBox *actionGroup = new QGroupBox("操作", this);
    actionGroup->setStyleSheet("QGroupBox { font-weight: bold; border: 1px solid #ddd; border-radius: 5px; margin-top: 10px; padding-top: 10px; }");
    QVBoxLayout *actionLayout = new QVBoxLayout(actionGroup);
    actionLayout->setSpacing(10);
    
    m_clearAllBtn = new QPushButton("清空所有便签", this);
    m_clearAllBtn->setStyleSheet(
        "QPushButton { background-color: #e74c3c; color: white; border: none; padding: 8px 15px; "
        "font-size: 13px; border-radius: 5px; }"
        "QPushButton:hover { background-color: #c0392b; }"
    );
    actionLayout->addWidget(m_clearAllBtn);
    
    mainLayout->addWidget(actionGroup);
    mainLayout->addStretch();
}

void NotesSettingsPanel::setupConnections()
{
    connect(m_addBtn, &QPushButton::clicked, this, &NotesSettingsPanel::onAddNoteClicked);
    connect(m_showBtn, &QPushButton::clicked, this, &NotesSettingsPanel::onShowNotesClicked);
    connect(m_hideBtn, &QPushButton::clicked, this, &NotesSettingsPanel::onHideNotesClicked);
    connect(m_clearAllBtn, &QPushButton::clicked, this, &NotesSettingsPanel::onClearAllClicked);
    connect(m_backBtn, &QPushButton::clicked, this, &NotesSettingsPanel::onBackClicked);
    
    NotesManager *nm = NotesManager::instance();
    connect(nm, &NotesManager::noteAdded, this, [this](const NoteData &) { updateStats(); });
    connect(nm, &NotesManager::noteRemoved, this, [this](int) { updateStats(); });
    connect(nm, &NotesManager::notesCleared, this, [this]() { updateStats(); });
}

void NotesSettingsPanel::setNotesWidget(NotesWidget *widget)
{
    m_notesWidget = widget;
    updateStats();
}

void NotesSettingsPanel::updateStats()
{
    NotesManager *nm = NotesManager::instance();
    int count = nm->noteCount();
    m_statsLabel->setText(QString("共 %1 条便签").arg(count));
}

void NotesSettingsPanel::onShowNotesClicked()
{
    if (m_notesWidget) {
        m_notesWidget->showAllNotes();
    }
    emit showNotesRequested();
}

void NotesSettingsPanel::onAddNoteClicked()
{
    NotesManager::instance()->addNote(NoteData());
}

void NotesSettingsPanel::onHideNotesClicked()
{
    if (m_notesWidget) {
        m_notesWidget->hideAllNotes();
    }
    emit hideNotesRequested();
}

void NotesSettingsPanel::onClearAllClicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "确认清空",
        "确定要清空所有便签吗？\n此操作不可撤销。",
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        NotesManager::instance()->clearAllNotes();
        updateStats();
    }
}

void NotesSettingsPanel::onBackClicked()
{
    emit backClicked();
}

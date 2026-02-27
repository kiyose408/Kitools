#include "notesitemwidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QDateTimeEdit>
#include <QCheckBox>
#include <QMouseEvent>
#include <QTimer>
#include <QPainter>
#include <QMessageBox>
#include <QApplication>
#include <QScreen>
#include <QRegularExpression>

NotesItemWidget::NotesItemWidget(const NoteData &note, QWidget *parent)
    : QWidget(parent)
    , m_noteId(note.id)
    , m_title(note.title)
    , m_content(note.content)
    , m_color(note.color)
    , m_hasReminder(note.hasReminder)
    , m_reminderTime(note.reminderTime)
    , m_isFlashing(false)
    , m_flashState(false)
    , m_flashTimer(nullptr)
    , m_isEditing(false)
    , m_isDragging(false)
    , m_viewWidget(nullptr)
    , m_editWidget(nullptr)
    , m_titleLabel(nullptr)
    , m_contentLabel(nullptr)
    , m_reminderLabel(nullptr)
    , m_deleteBtn(nullptr)
    , m_editBtn(nullptr)
    , m_colorIndicator(nullptr)
    , m_titleEdit(nullptr)
    , m_contentEdit(nullptr)
    , m_reminderCheck(nullptr)
    , m_reminderDateTime(nullptr)
    , m_saveBtn(nullptr)
    , m_cancelBtn(nullptr)
{
    setupUi();
    setupConnections();
    updateAppearance();
    
    m_flashTimer = new QTimer(this);
    connect(m_flashTimer, &QTimer::timeout, this, &NotesItemWidget::onFlashTimer);
    
    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground, false);
    setMinimumSize(300, 220);
    resize(340, 300);
}

NotesItemWidget::~NotesItemWidget()
{
}

void NotesItemWidget::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    m_viewWidget = new QWidget(this);
    m_viewWidget->setObjectName("viewWidget");
    QVBoxLayout *viewLayout = new QVBoxLayout(m_viewWidget);
    viewLayout->setContentsMargins(10, 8, 10, 8);
    viewLayout->setSpacing(8);
    
    QWidget *headerWidget = new QWidget(m_viewWidget);
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(8);
    
    m_colorIndicator = new QLabel(headerWidget);
    m_colorIndicator->setFixedSize(12, 12);
    m_colorIndicator->setStyleSheet("border-radius: 6px;");
    headerLayout->addWidget(m_colorIndicator);
    
    m_titleLabel = new QLabel(headerWidget);
    m_titleLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #2c3e50;");
    m_titleLabel->setWordWrap(true);
    headerLayout->addWidget(m_titleLabel, 1);
    
    m_editBtn = new QPushButton("✏", headerWidget);
    m_editBtn->setFixedSize(24, 24);
    m_editBtn->setStyleSheet(
        "QPushButton { background: transparent; border: none; color: #7f8c8d; font-size: 14px; }"
        "QPushButton:hover { color: #3498db; }"
    );
    m_editBtn->setToolTip("编辑便签");
    m_editBtn->hide();
    headerLayout->addWidget(m_editBtn);
    
    m_deleteBtn = new QPushButton("×", headerWidget);
    m_deleteBtn->setFixedSize(24, 24);
    m_deleteBtn->setStyleSheet(
        "QPushButton { background: transparent; border: none; color: #e74c3c; font-size: 16px; font-weight: bold; }"
        "QPushButton:hover { background: #e74c3c; color: white; border-radius: 12px; }"
    );
    m_deleteBtn->setToolTip("删除便签");
    m_deleteBtn->hide();
    headerLayout->addWidget(m_deleteBtn);
    
    viewLayout->addWidget(headerWidget);
    
    m_contentLabel = new QLabel(m_viewWidget);
    m_contentLabel->setStyleSheet("font-size: 13px; color: #34495e;");
    m_contentLabel->setWordWrap(true);
    m_contentLabel->setTextFormat(Qt::RichText);
    m_contentLabel->setAlignment(Qt::AlignTop);
    m_contentLabel->setOpenExternalLinks(true);
    viewLayout->addWidget(m_contentLabel, 1);
    
    m_reminderLabel = new QLabel(m_viewWidget);
    m_reminderLabel->setStyleSheet("font-size: 11px; color: #e67e22;");
    m_reminderLabel->hide();
    viewLayout->addWidget(m_reminderLabel);
    
    mainLayout->addWidget(m_viewWidget);
    
    m_editWidget = new QWidget(this);
    m_editWidget->setObjectName("editWidget");
    QVBoxLayout *editLayout = new QVBoxLayout(m_editWidget);
    editLayout->setContentsMargins(10, 8, 10, 8);
    editLayout->setSpacing(6);
    
    QWidget *editHeaderWidget = new QWidget(m_editWidget);
    QHBoxLayout *editHeaderLayout = new QHBoxLayout(editHeaderWidget);
    editHeaderLayout->setContentsMargins(0, 0, 0, 0);
    
    QLabel *editTitleLabel = new QLabel("编辑便签", editHeaderWidget);
    editTitleLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #2c3e50;");
    editHeaderLayout->addWidget(editTitleLabel);
    editHeaderLayout->addStretch();
    
    m_saveBtn = new QPushButton("保存", editHeaderWidget);
    m_saveBtn->setStyleSheet(
        "QPushButton { background-color: #27ae60; color: white; border: none; padding: 4px 12px; "
        "font-size: 12px; border-radius: 3px; }"
        "QPushButton:hover { background-color: #2ecc71; }"
    );
    editHeaderLayout->addWidget(m_saveBtn);
    
    m_cancelBtn = new QPushButton("取消", editHeaderWidget);
    m_cancelBtn->setStyleSheet(
        "QPushButton { background-color: #95a5a6; color: white; border: none; padding: 4px 12px; "
        "font-size: 12px; border-radius: 3px; }"
        "QPushButton:hover { background-color: #7f8c8d; }"
    );
    editHeaderLayout->addWidget(m_cancelBtn);
    
    editLayout->addWidget(editHeaderWidget);
    
    m_titleEdit = new QLineEdit(m_editWidget);
    m_titleEdit->setPlaceholderText("标题（可选）");
    m_titleEdit->setStyleSheet("QLineEdit { padding: 5px; border: 1px solid #ddd; border-radius: 3px; font-size: 13px; }");
    m_titleEdit->setMaximumHeight(30);
    editLayout->addWidget(m_titleEdit);
    
    QWidget *settingsRow = new QWidget(m_editWidget);
    QHBoxLayout *settingsLayout = new QHBoxLayout(settingsRow);
    settingsLayout->setContentsMargins(0, 0, 0, 0);
    settingsLayout->setSpacing(10);
    
    QWidget *colorWidget = new QWidget(settingsRow);
    QHBoxLayout *colorLayout = new QHBoxLayout(colorWidget);
    colorLayout->setContentsMargins(0, 0, 0, 0);
    colorLayout->setSpacing(4);
    
    QLabel *colorLabel = new QLabel("颜色:", colorWidget);
    colorLabel->setStyleSheet("font-size: 12px;");
    colorLayout->addWidget(colorLabel);
    
    QList<QPair<NoteColor, QString>> colors = {
        {NoteColor::Yellow, "#FFC107"},
        {NoteColor::Green, "#4CAF50"},
        {NoteColor::Blue, "#2196F3"},
        {NoteColor::Pink, "#E91E63"},
        {NoteColor::Purple, "#9C27B0"}
    };
    
    for (const auto &pair : colors) {
        QPushButton *btn = new QPushButton(colorWidget);
        btn->setFixedSize(18, 18);
        btn->setStyleSheet(QString(
            "QPushButton { background-color: %1; border: 2px solid transparent; border-radius: 9px; }"
            "QPushButton:hover { border: 2px solid #333; }"
        ).arg(pair.second));
        btn->setProperty("color", static_cast<int>(pair.first));
        colorLayout->addWidget(btn);
        m_colorButtons.append(btn);
    }
    settingsLayout->addWidget(colorWidget);
    
    QWidget *reminderWidget = new QWidget(settingsRow);
    QHBoxLayout *reminderLayout = new QHBoxLayout(reminderWidget);
    reminderLayout->setContentsMargins(0, 0, 0, 0);
    reminderLayout->setSpacing(4);
    
    m_reminderCheck = new QCheckBox("提醒", reminderWidget);
    m_reminderCheck->setStyleSheet("font-size: 12px;");
    reminderLayout->addWidget(m_reminderCheck);
    
    m_reminderDateTime = new QDateTimeEdit(reminderWidget);
    m_reminderDateTime->setCalendarPopup(true);
    m_reminderDateTime->setDateTime(QDateTime::currentDateTime().addSecs(3600));
    m_reminderDateTime->setDisplayFormat("yyyy-MM-dd hh:mm");
    m_reminderDateTime->setEnabled(false);
    m_reminderDateTime->setStyleSheet("QDateTimeEdit { padding: 2px; border: 1px solid #ddd; border-radius: 3px; font-size: 11px; }");
    reminderLayout->addWidget(m_reminderDateTime);
    settingsLayout->addWidget(reminderWidget);
    
    settingsLayout->addStretch();
    editLayout->addWidget(settingsRow);
    
    m_contentEdit = new QTextEdit(m_editWidget);
    m_contentEdit->setPlaceholderText("输入便签内容，支持 Markdown 格式...");
    m_contentEdit->setStyleSheet("QTextEdit { padding: 5px; border: 1px solid #ddd; border-radius: 3px; font-size: 13px; }");
    m_contentEdit->setMinimumHeight(120);
    editLayout->addWidget(m_contentEdit, 1);
    
    m_editWidget->hide();
    mainLayout->addWidget(m_editWidget);
    
    setCursor(Qt::ArrowCursor);
}

void NotesItemWidget::setupConnections()
{
    connect(m_deleteBtn, &QPushButton::clicked, this, &NotesItemWidget::onDeleteClicked);
    connect(m_editBtn, &QPushButton::clicked, this, &NotesItemWidget::onEditClicked);
    connect(m_saveBtn, &QPushButton::clicked, this, &NotesItemWidget::onSaveClicked);
    connect(m_cancelBtn, &QPushButton::clicked, this, &NotesItemWidget::onCancelClicked);
    connect(m_reminderCheck, &QCheckBox::toggled, this, &NotesItemWidget::onReminderToggled);
    
    for (QPushButton *btn : m_colorButtons) {
        connect(btn, &QPushButton::clicked, this, [this, btn]() {
            m_color = static_cast<NoteColor>(btn->property("color").toInt());
            updateColorButtons();
            updateAppearance();
        });
    }
}

void NotesItemWidget::setNoteData(const NoteData &note)
{
    m_noteId = note.id;
    m_title = note.title;
    m_content = note.content;
    m_color = note.color;
    m_hasReminder = note.hasReminder;
    m_reminderTime = note.reminderTime;
    updateAppearance();
}

NoteData NotesItemWidget::getNoteData() const
{
    NoteData note;
    note.id = m_noteId;
    note.title = m_title;
    note.content = m_content;
    note.color = m_color;
    note.hasReminder = m_hasReminder;
    note.reminderTime = m_reminderTime;
    return note;
}

void NotesItemWidget::setFlashing(bool flashing)
{
    m_isFlashing = flashing;
    m_flashState = false;
    update();
}

void NotesItemWidget::startFlashing()
{
    m_isFlashing = true;
    m_flashState = false;
    m_flashTimer->start(500);
    raise();
    activateWindow();
}

void NotesItemWidget::stopFlashing()
{
    m_isFlashing = false;
    m_flashState = false;
    m_flashTimer->stop();
    updateAppearance();
}

void NotesItemWidget::onFlashTimer()
{
    m_flashState = !m_flashState;
    update();
}

void NotesItemWidget::enterEditMode()
{
    m_isEditing = true;
    m_titleEdit->setText(m_title);
    m_contentEdit->setPlainText(m_content);
    m_reminderCheck->setChecked(m_hasReminder);
    if (m_hasReminder && m_reminderTime.isValid()) {
        m_reminderDateTime->setDateTime(m_reminderTime);
    } else {
        m_reminderDateTime->setDateTime(QDateTime::currentDateTime().addSecs(3600));
    }
    updateColorButtons();
    
    m_viewWidget->hide();
    m_editWidget->show();
    m_contentEdit->setFocus();
}

void NotesItemWidget::exitEditMode()
{
    m_isEditing = false;
    m_editWidget->hide();
    m_viewWidget->show();
}

void NotesItemWidget::onDeleteClicked()
{
    emit deleteRequested(m_noteId);
}

void NotesItemWidget::onEditClicked()
{
    enterEditMode();
}

void NotesItemWidget::onSaveClicked()
{
    m_title = m_titleEdit->text().trimmed();
    m_content = m_contentEdit->toPlainText();
    m_hasReminder = m_reminderCheck->isChecked();
    if (m_hasReminder) {
        m_reminderTime = m_reminderDateTime->dateTime();
    } else {
        m_reminderTime = QDateTime();
    }
    
    updateAppearance();
    exitEditMode();
    emit dataChanged(m_noteId);
}

void NotesItemWidget::onCancelClicked()
{
    exitEditMode();
}

void NotesItemWidget::onReminderToggled(bool checked)
{
    m_reminderDateTime->setEnabled(checked);
}

void NotesItemWidget::updateAppearance()
{
    QString title = m_title.isEmpty() ? getFirstLine(m_content) : m_title;
    m_titleLabel->setText(title);
    
    QString displayContent = m_content;
    if (displayContent.length() > 500) {
        displayContent = displayContent.left(500) + "...";
    }
    m_contentLabel->setText(renderMarkdown(displayContent));
    
    QColor indicatorColor;
    switch (m_color) {
        case NoteColor::Yellow: indicatorColor = QColor(255, 193, 7); break;
        case NoteColor::Green: indicatorColor = QColor(76, 175, 80); break;
        case NoteColor::Blue: indicatorColor = QColor(33, 150, 243); break;
        case NoteColor::Pink: indicatorColor = QColor(233, 30, 99); break;
        case NoteColor::Purple: indicatorColor = QColor(156, 39, 176); break;
    }
    m_colorIndicator->setStyleSheet(QString("background-color: %1; border-radius: 6px;").arg(indicatorColor.name()));
    
    if (m_hasReminder) {
        m_reminderLabel->setText(QString("提醒: %1").arg(m_reminderTime.toString("yyyy-MM-dd hh:mm")));
        m_reminderLabel->show();
    } else {
        m_reminderLabel->hide();
    }
    
    NoteData tempNote;
    tempNote.color = m_color;
    QColor bgColor = tempNote.toQColor();
    
    QString style = QString(
        "QWidget#viewWidget, QWidget#editWidget { background-color: %1; border-radius: 8px; }"
    ).arg(bgColor.name());
    
    if (m_isFlashing) {
        style = QString(
            "QWidget#viewWidget, QWidget#editWidget { background-color: #fff3cd; border: 2px solid #ffc107; border-radius: 8px; }"
        );
    }
    
    setStyleSheet(style);
}

QString NotesItemWidget::renderMarkdown(const QString &markdown)
{
    QStringList lines = markdown.split('\n');
    QStringList result;
    bool inCodeBlock = false;
    QString codeBlockContent;
    QString codeBlockLang;
    
    for (const QString &line : lines) {
        QString processedLine = line.toHtmlEscaped();
        
        if (processedLine.startsWith("```")) {
            if (!inCodeBlock) {
                inCodeBlock = true;
                codeBlockLang = processedLine.mid(3).trimmed();
                codeBlockContent.clear();
            } else {
                inCodeBlock = false;
                result.append(QString("<pre style='background:#f5f5f5;padding:8px;border-radius:4px;overflow-x:auto;font-family:monospace;'><code>%1</code></pre>")
                    .arg(codeBlockContent));
            }
            continue;
        }
        
        if (inCodeBlock) {
            if (!codeBlockContent.isEmpty()) {
                codeBlockContent += "\n";
            }
            codeBlockContent += processedLine;
            continue;
        }
        
        if (processedLine.startsWith("### ")) {
            QString title = processedLine.mid(4);
            result.append(QString("<h3 style='font-size:15px;margin:8px 0 4px 0;font-weight:bold;'>%1</h3>").arg(title));
            continue;
        }
        
        if (processedLine.startsWith("## ")) {
            QString title = processedLine.mid(3);
            result.append(QString("<h2 style='font-size:17px;margin:10px 0 5px 0;font-weight:bold;'>%1</h2>").arg(title));
            continue;
        }
        
        if (processedLine.startsWith("# ")) {
            QString title = processedLine.mid(2);
            result.append(QString("<h1 style='font-size:19px;margin:12px 0 6px 0;font-weight:bold;'>%1</h1>").arg(title));
            continue;
        }
        
        if (processedLine.startsWith("- ") || processedLine.startsWith("* ")) {
            QString item = processedLine.mid(2);
            item = applyInlineFormatting(item);
            result.append(QString("• %1").arg(item));
            continue;
        }
        
        QRegularExpression olRegex("^(\\d+)\\. (.*)$");
        QRegularExpressionMatch olMatch = olRegex.match(processedLine);
        if (olMatch.hasMatch()) {
            QString num = olMatch.captured(1);
            QString item = olMatch.captured(2);
            item = applyInlineFormatting(item);
            result.append(QString("%1. %2").arg(num).arg(item));
            continue;
        }
        
        if (processedLine.trimmed().isEmpty()) {
            result.append("<br>");
        } else {
            processedLine = applyInlineFormatting(processedLine);
            result.append(processedLine);
        }
    }
    
    if (inCodeBlock) {
        result.append(QString("<pre style='background:#f5f5f5;padding:8px;border-radius:4px;overflow-x:auto;font-family:monospace;'><code>%1</code></pre>")
            .arg(codeBlockContent));
    }
    
    return result.join("<br>");
}

QString NotesItemWidget::applyInlineFormatting(const QString &text)
{
    QString result = text;
    
    QRegularExpression boldRegex("\\*\\*([^*]+?)\\*\\*");
    result.replace(boldRegex, "<b>\\1</b>");
    
    QRegularExpression italicRegex("(?<!\\*)\\*([^*]+?)\\*(?!\\*)");
    result.replace(italicRegex, "<i>\\1</i>");
    
    QRegularExpression boldItalicRegex("\\*\\*\\*([^*]+?)\\*\\*\\*");
    result.replace(boldItalicRegex, "<b><i>\\1</i></b>");
    
    QRegularExpression strikeRegex("~~([^~]+?)~~");
    result.replace(strikeRegex, "<s>\\1</s>");
    
    QRegularExpression inlineCodeRegex("`([^`]+?)`");
    result.replace(inlineCodeRegex, "<code style='background:#f0f0f0;padding:2px 4px;border-radius:3px;font-family:monospace;'>\\1</code>");
    
    QRegularExpression linkRegex("\\[([^\\]]+?)\\]\\(([^)]+?)\\)");
    result.replace(linkRegex, "<a href='\\2' style='color:#3498db;text-decoration:none;'>\\1</a>");
    
    QRegularExpression imgRegex("!\\[([^\\]]*?)\\]\\(([^)]+?)\\)");
    result.replace(imgRegex, "<img src='\\2' alt='\\1' style='max-width:100%;'>");
    
    return result;
}

void NotesItemWidget::updateColorButtons()
{
    for (QPushButton *btn : m_colorButtons) {
        NoteColor btnColor = static_cast<NoteColor>(btn->property("color").toInt());
        QString colorHex;
        switch (btnColor) {
            case NoteColor::Yellow: colorHex = "#FFC107"; break;
            case NoteColor::Green: colorHex = "#4CAF50"; break;
            case NoteColor::Blue: colorHex = "#2196F3"; break;
            case NoteColor::Pink: colorHex = "#E91E63"; break;
            case NoteColor::Purple: colorHex = "#9C27B0"; break;
        }
        
        if (btnColor == m_color) {
            btn->setStyleSheet(QString(
                "QPushButton { background-color: %1; border: 2px solid #333; border-radius: 10px; }"
            ).arg(colorHex));
        } else {
            btn->setStyleSheet(QString(
                "QPushButton { background-color: %1; border: 2px solid transparent; border-radius: 10px; }"
                "QPushButton:hover { border: 2px solid #333; }"
            ).arg(colorHex));
        }
    }
}

QString NotesItemWidget::getFirstLine(const QString &content) const
{
    QStringList lines = content.split('\n');
    for (const QString &line : lines) {
        QString trimmed = line.trimmed();
        if (!trimmed.isEmpty()) {
            if (trimmed.startsWith("#")) {
                trimmed = trimmed.mid(1).trimmed();
            }
            if (trimmed.length() > 30) {
                return trimmed.left(30) + "...";
            }
            return trimmed;
        }
    }
    return "无标题";
}

void NotesItemWidget::enterEvent(QEnterEvent *event)
{
    Q_UNUSED(event);
    m_deleteBtn->show();
    m_editBtn->show();
}

void NotesItemWidget::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    if (!m_isEditing) {
        m_deleteBtn->hide();
        m_editBtn->hide();
    }
}

void NotesItemWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && !m_isEditing) {
        m_isDragging = true;
        m_dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
    }
    QWidget::mousePressEvent(event);
}

void NotesItemWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isDragging && (event->buttons() & Qt::LeftButton)) {
        move(event->globalPosition().toPoint() - m_dragPosition);
    }
    QWidget::mouseMoveEvent(event);
}

void NotesItemWidget::mouseReleaseEvent(QMouseEvent *event)
{
    m_isDragging = false;
    QWidget::mouseReleaseEvent(event);
}

void NotesItemWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    QPen pen(Qt::NoPen);
    painter.setPen(pen);
    
    NoteData tempNote;
    tempNote.color = m_color;
    QColor normalBgColor = tempNote.toQColor();
    
    if (m_isFlashing && m_flashState) {
        QColor invertedColor = QColor(255 - normalBgColor.red(), 
                                       255 - normalBgColor.green(), 
                                       255 - normalBgColor.blue());
        painter.setBrush(QBrush(invertedColor));
        
        QString invertedStyle = QString(
            "QLabel { color: %1; }"
            "QPushButton { color: %1; }"
        ).arg(normalBgColor.name());
        
        m_titleLabel->setStyleSheet(QString("font-size: 14px; font-weight: bold; color: %1;").arg(normalBgColor.name()));
        m_contentLabel->setStyleSheet(QString("font-size: 13px; color: %1;").arg(normalBgColor.name()));
        m_reminderLabel->setStyleSheet(QString("font-size: 11px; color: %1;").arg(normalBgColor.name()));
    } else {
        painter.setBrush(QBrush(normalBgColor));
        m_titleLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #2c3e50;");
        m_contentLabel->setStyleSheet("font-size: 13px; color: #34495e;");
        m_reminderLabel->setStyleSheet("font-size: 11px; color: #e67e22;");
    }
    
    painter.drawRoundedRect(rect(), 8, 8);
}

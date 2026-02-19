#include "taskitemwidget.h"

TaskItemWidget::TaskItemWidget(const TaskData &task, QWidget *parent)
    : QWidget(parent)
    , m_taskId(task.id)
    , m_description(task.description)
    , m_isCompleted(task.isCompleted)
    , m_score(task.completionScore)
    , m_editMode(false)
    , m_lastEmittedScore(task.completionScore)
    , m_checkBox(nullptr)
    , m_descriptionLabel(nullptr)
    , m_editLineEdit(nullptr)
    , m_scoreSlider(nullptr)
    , m_scoreLabel(nullptr)
    , m_editBtn(nullptr)
    , m_deleteBtn(nullptr)
    , m_scoreWidget(nullptr)
{
    setupUi();
    updateAppearance();
}

TaskItemWidget::~TaskItemWidget()
{
}

void TaskItemWidget::setupUi()
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);
    layout->setSpacing(8);
    
    m_checkBox = new QCheckBox(this);
    m_checkBox->setChecked(m_isCompleted);
    connect(m_checkBox, &QCheckBox::toggled, this, &TaskItemWidget::onCheckBoxToggled);
    layout->addWidget(m_checkBox);
    
    m_descriptionLabel = new QLabel(m_description, this);
    m_descriptionLabel->setWordWrap(true);
    m_descriptionLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    layout->addWidget(m_descriptionLabel, 1);
    
    m_editLineEdit = new QLineEdit(m_description, this);
    m_editLineEdit->hide();
    connect(m_editLineEdit, &QLineEdit::returnPressed, this, &TaskItemWidget::onEditFinished);
    layout->addWidget(m_editLineEdit, 1);
    
    m_scoreWidget = new QWidget(this);
    QHBoxLayout *scoreLayout = new QHBoxLayout(m_scoreWidget);
    scoreLayout->setContentsMargins(0, 0, 0, 0);
    scoreLayout->setSpacing(5);
    
    m_scoreSlider = new QSlider(Qt::Horizontal, m_scoreWidget);
    m_scoreSlider->setRange(0, 10);
    m_scoreSlider->setValue(m_score / 10);
    m_scoreSlider->setFixedWidth(80);
    m_scoreSlider->setEnabled(m_isCompleted);
    connect(m_scoreSlider, &QSlider::valueChanged, this, &TaskItemWidget::onSliderValueChanged);
    connect(m_scoreSlider, &QSlider::sliderReleased, this, &TaskItemWidget::onSliderReleased);
    scoreLayout->addWidget(m_scoreSlider);
    
    m_scoreLabel = new QLabel(QString("%1").arg(m_score / 10), m_scoreWidget);
    m_scoreLabel->setFixedWidth(20);
    scoreLayout->addWidget(m_scoreLabel);
    
    m_scoreWidget->setVisible(m_isCompleted);
    layout->addWidget(m_scoreWidget);
    
    m_editBtn = new QPushButton("✏", this);
    m_editBtn->setFixedSize(28, 28);
    m_editBtn->setStyleSheet(
        "QPushButton { background-color: transparent; border: none; color: #7f8c8d; font-size: 14px; }"
        "QPushButton:hover { color: #3498db; }"
    );
    connect(m_editBtn, &QPushButton::clicked, this, &TaskItemWidget::onEditClicked);
    layout->addWidget(m_editBtn);
    
    m_deleteBtn = new QPushButton("×", this);
    m_deleteBtn->setFixedSize(28, 28);
    m_deleteBtn->setStyleSheet(
        "QPushButton { background-color: transparent; border: none; color: #e74c3c; font-size: 18px; font-weight: bold; }"
        "QPushButton:hover { background-color: #e74c3c; color: white; border-radius: 14px; }"
    );
    connect(m_deleteBtn, &QPushButton::clicked, this, &TaskItemWidget::onDeleteClicked);
    layout->addWidget(m_deleteBtn);
    
    setStyleSheet(
        "TaskItemWidget { background-color: #ffffff; border-radius: 5px; margin: 2px; }"
        "TaskItemWidget:hover { background-color: #f8f9fa; }"
    );
}

void TaskItemWidget::setTaskData(const TaskData &task)
{
    m_taskId = task.id;
    m_description = task.description;
    m_isCompleted = task.isCompleted;
    m_score = task.completionScore;
    m_lastEmittedScore = task.completionScore;
    
    m_checkBox->blockSignals(true);
    m_checkBox->setChecked(m_isCompleted);
    m_checkBox->blockSignals(false);
    
    m_descriptionLabel->setText(m_description);
    m_editLineEdit->setText(m_description);
    
    m_scoreSlider->blockSignals(true);
    m_scoreSlider->setValue(m_score / 10);
    m_scoreSlider->blockSignals(false);
    m_scoreSlider->setEnabled(m_isCompleted);
    
    m_scoreLabel->setText(QString("%1").arg(m_score / 10));
    
    updateAppearance();
}

void TaskItemWidget::setEditMode(bool editMode)
{
    m_editMode = editMode;
    if (editMode) {
        m_descriptionLabel->hide();
        m_editLineEdit->show();
        m_editLineEdit->setFocus();
        m_editLineEdit->selectAll();
    } else {
        m_editLineEdit->hide();
        m_descriptionLabel->show();
    }
}

void TaskItemWidget::updateAppearance()
{
    if (m_isCompleted) {
        m_descriptionLabel->setStyleSheet("color: #95a5a6; text-decoration: line-through;");
        m_scoreWidget->show();
    } else {
        m_descriptionLabel->setStyleSheet("color: #2c3e50;");
        m_scoreWidget->hide();
    }
}

void TaskItemWidget::onCheckBoxToggled(bool checked)
{
    m_isCompleted = checked;
    m_scoreSlider->setEnabled(checked);
    if (!checked) {
        m_score = 0;
        m_lastEmittedScore = 0;
        m_scoreSlider->setValue(0);
        m_scoreLabel->setText("0");
    }
    updateAppearance();
    emit completedChanged(m_taskId, checked);
}

void TaskItemWidget::onSliderValueChanged(int value)
{
    m_score = value * 10;
    m_scoreLabel->setText(QString("%1").arg(value));
}

void TaskItemWidget::onSliderReleased()
{
    int newScore = m_scoreSlider->value() * 10;
    if (newScore != m_lastEmittedScore) {
        m_score = newScore;
        m_lastEmittedScore = newScore;
        emit scoreChanged(m_taskId, newScore);
    }
}

void TaskItemWidget::onDeleteClicked()
{
    emit deleteRequested(m_taskId);
}

void TaskItemWidget::onEditClicked()
{
    if (m_editMode) {
        onEditFinished();
    } else {
        setEditMode(true);
    }
}

void TaskItemWidget::onEditFinished()
{
    QString newText = m_editLineEdit->text().trimmed();
    if (!newText.isEmpty() && newText != m_description) {
        m_description = newText;
        m_descriptionLabel->setText(newText);
        emit editRequested(m_taskId, newText);
    }
    setEditMode(false);
}

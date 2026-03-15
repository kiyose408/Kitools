#include "clipboarditemwidget.h"
#include <QMouseEvent>
#include <QFont>
#include <QPalette>
#include <QDebug>

ClipboardItemWidget::ClipboardItemWidget(const ClipboardItem& item, QWidget* parent)
    : QWidget(parent)
    , m_item(item)
{
    setupUI();
    setMinimumHeight(60);
    setMaximumHeight(80);
}

void ClipboardItemWidget::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 5, 10, 5);
    mainLayout->setSpacing(2);

    QHBoxLayout* topLayout = new QHBoxLayout();

    m_typeLabel = new QLabel(this);
    QString typeText;
    QString typeColor;
    switch (m_item.type) {
        case ClipboardType::Text:
            typeText = "文本";
            typeColor = "#3498db";
            break;
        case ClipboardType::Image:
            typeText = "图片";
            typeColor = "#9b59b6";
            break;
        case ClipboardType::File:
            typeText = "文件";
            typeColor = "#27ae60";
            break;
        case ClipboardType::Html:
            typeText = "HTML";
            typeColor = "#e67e22";
            break;
        default:
            typeText = "未知";
            typeColor = "#95a5a6";
    }
    m_typeLabel->setText(typeText);
    m_typeLabel->setStyleSheet(QString("QLabel { background-color: %1; color: white; padding: 2px 6px; border-radius: 3px; font-size: 10px; }").arg(typeColor));
    m_typeLabel->setFixedWidth(50);

    m_timeLabel = new QLabel(this);
    m_timeLabel->setText(m_item.timestamp.toString("yyyy-MM-dd HH:mm"));
    m_timeLabel->setStyleSheet("QLabel { color: #7f8c8d; font-size: 10px; }");

    m_favoriteLabel = new QLabel(this);
    m_favoriteLabel->setText(m_item.isFavorite ? "★" : "☆");
    m_favoriteLabel->setStyleSheet(QString("QLabel { color: %1; font-size: 14px; }")
        .arg(m_item.isFavorite ? "#f39c12" : "#bdc3c7"));

    topLayout->addWidget(m_typeLabel);
    topLayout->addStretch();
    topLayout->addWidget(m_favoriteLabel);
    topLayout->addWidget(m_timeLabel);

    m_previewLabel = new QLabel(this);
    m_previewLabel->setText(m_item.preview);
    m_previewLabel->setWordWrap(true);
    m_previewLabel->setStyleSheet("QLabel { color: #2c3e50; font-size: 12px; }");
    m_previewLabel->setMaximumHeight(40);

    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(m_previewLabel);

    setStyleSheet("ClipboardItemWidget { background-color: white; border-bottom: 1px solid #ecf0f1; } "
                  "ClipboardItemWidget:hover { background-color: #f8f9fa; }");

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &ClipboardItemWidget::customContextMenuRequested, this, &ClipboardItemWidget::showContextMenu);
}

void ClipboardItemWidget::updateItem(const ClipboardItem& item) {
    m_item = item;
    updateUI();
}

void ClipboardItemWidget::updateUI() {
    QString typeText;
    QString typeColor;
    switch (m_item.type) {
        case ClipboardType::Text:
            typeText = "文本";
            typeColor = "#3498db";
            break;
        case ClipboardType::Image:
            typeText = "图片";
            typeColor = "#9b59b6";
            break;
        case ClipboardType::File:
            typeText = "文件";
            typeColor = "#27ae60";
            break;
        case ClipboardType::Html:
            typeText = "HTML";
            typeColor = "#e67e22";
            break;
        default:
            typeText = "未知";
            typeColor = "#95a5a6";
    }
    m_typeLabel->setText(typeText);
    m_typeLabel->setStyleSheet(QString("QLabel { background-color: %1; color: white; padding: 2px 6px; border-radius: 3px; font-size: 10px; }").arg(typeColor));
    m_previewLabel->setText(m_item.preview);
    m_timeLabel->setText(m_item.timestamp.toString("yyyy-MM-dd HH:mm"));
    m_favoriteLabel->setText(m_item.isFavorite ? "★" : "☆");
    m_favoriteLabel->setStyleSheet(QString("QLabel { color: %1; font-size: 14px; }")
        .arg(m_item.isFavorite ? "#f39c12" : "#bdc3c7"));
}

void ClipboardItemWidget::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        qDebug() << "双击复制项目:" << m_item.id;
        emit copyClicked(m_item.id);
    }
    QWidget::mouseDoubleClickEvent(event);
}

void ClipboardItemWidget::onCopyClicked() {
    emit copyClicked(m_item.id);
}

void ClipboardItemWidget::onFavoriteClicked() {
    emit favoriteClicked(m_item.id);
}

void ClipboardItemWidget::onDeleteClicked() {
    emit deleteClicked(m_item.id);
}

void ClipboardItemWidget::showContextMenu(const QPoint& pos) {
    QMenu menu(this);

    QAction* copyAction = new QAction("复制", this);
    connect(copyAction, &QAction::triggered, this, &ClipboardItemWidget::onCopyClicked);
    menu.addAction(copyAction);

    QAction* viewAction = new QAction("查看详情", this);
    connect(viewAction, &QAction::triggered, this, [this]() { emit viewDetails(m_item.id); });
    menu.addAction(viewAction);

    QAction* favoriteAction = new QAction(m_item.isFavorite ? "取消收藏" : "添加收藏", this);
    connect(favoriteAction, &QAction::triggered, this, &ClipboardItemWidget::onFavoriteClicked);
    menu.addAction(favoriteAction);

    menu.addSeparator();

    QAction* deleteAction = new QAction("删除", this);
    connect(deleteAction, &QAction::triggered, this, &ClipboardItemWidget::onDeleteClicked);
    menu.addAction(deleteAction);

    menu.exec(mapToGlobal(pos));
}

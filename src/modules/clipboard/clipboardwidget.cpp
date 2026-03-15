#include "clipboardwidget.h"
#include "clipboarddatatypes.h"
#include <QScrollArea>
#include <QScrollBar>
#include <QLabel>
#include <QMessageBox>
#include <QDebug>
#include <QDialog>
#include <QTextEdit>

ClipboardWidget::ClipboardWidget(QWidget* parent)
    : QWidget(parent)
    , m_manager(nullptr)
    , m_searchEdit(nullptr)
    , m_filterCombo(nullptr)
    , m_clearBtn(nullptr)
    , m_refreshBtn(nullptr)
    , m_closeBtn(nullptr)
    , m_listLayout(nullptr)
    , m_listContainer(nullptr)
    , m_currentFilter(0)
{
    qDebug() << "剪贴板窗口构造函数开始";
    
    m_manager = ClipboardManager::instance(this);
    
    setupUI();
    loadItems();

    m_manager->startMonitoring();

    connect(m_manager, &ClipboardManager::itemAdded, this, &ClipboardWidget::onItemAdded);
    connect(m_manager, &ClipboardManager::itemDeleted, this, &ClipboardWidget::onItemDeleted);
    connect(m_manager, &ClipboardManager::favoritesChanged, this, &ClipboardWidget::refresh);
    connect(m_manager, &ClipboardManager::countChanged, this, &ClipboardWidget::refresh);
    
    qDebug() << "剪贴板窗口构造函数完成";
}

ClipboardWidget::~ClipboardWidget() {
    if (m_manager) {
        m_manager->stopMonitoring();
    }
}

void ClipboardWidget::setupUI() {
    setWindowTitle("剪贴板历史");
    setMinimumSize(400, 500);
    setStyleSheet("QWidget { background-color: #f5f6fa; }");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);

    QLabel* titleLabel = new QLabel("剪贴板历史", this);
    titleLabel->setStyleSheet("QLabel { font-size: 18px; font-weight: bold; color: #2c3e50; }");
    mainLayout->addWidget(titleLabel);

    QHBoxLayout* searchLayout = new QHBoxLayout();
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("搜索...");
    m_searchEdit->setStyleSheet("QLineEdit { padding: 8px; border: 1px solid #dcdde1; border-radius: 5px; background-color: white; }");
    connect(m_searchEdit, &QLineEdit::textChanged, this, &ClipboardWidget::onSearchChanged);
    searchLayout->addWidget(m_searchEdit);
    mainLayout->addLayout(searchLayout);

    QHBoxLayout* filterLayout = new QHBoxLayout();
    m_filterCombo = new QComboBox(this);
    m_filterCombo->addItem("全部", 0);
    m_filterCombo->addItem("文本", 1);
    m_filterCombo->addItem("图片", 2);
    m_filterCombo->addItem("文件", 3);
    m_filterCombo->addItem("收藏", 4);
    m_filterCombo->setStyleSheet("QComboBox { padding: 5px; border: 1px solid #dcdde1; border-radius: 5px; background-color: white; }");
    connect(m_filterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ClipboardWidget::onFilterChanged);
    filterLayout->addWidget(m_filterCombo);

    m_refreshBtn = new QPushButton("刷新", this);
    m_refreshBtn->setStyleSheet("QPushButton { padding: 5px 10px; background-color: #3498db; color: white; border: none; border-radius: 5px; }");
    connect(m_refreshBtn, &QPushButton::clicked, this, &ClipboardWidget::onRefresh);
    filterLayout->addWidget(m_refreshBtn);

    m_clearBtn = new QPushButton("清空", this);
    m_clearBtn->setStyleSheet("QPushButton { padding: 5px 10px; background-color: #e74c3c; color: white; border: none; border-radius: 5px; }");
    connect(m_clearBtn, &QPushButton::clicked, this, &ClipboardWidget::onClearAll);
    filterLayout->addWidget(m_clearBtn);

    mainLayout->addLayout(filterLayout);

    m_listContainer = new QWidget(this);
    m_listLayout = new QVBoxLayout(m_listContainer);
    m_listLayout->setContentsMargins(0, 0, 0, 0);
    m_listLayout->setSpacing(0);
    m_listLayout->addStretch();

    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidget(m_listContainer);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("QScrollArea { border: none; background-color: transparent; }");
    mainLayout->addWidget(scrollArea);

    QHBoxLayout* bottomLayout = new QHBoxLayout();
    m_closeBtn = new QPushButton("关闭", this);
    m_closeBtn->setStyleSheet("QPushButton { padding: 8px 20px; background-color: #95a5a6; color: white; border: none; border-radius: 5px; }");
    connect(m_closeBtn, &QPushButton::clicked, this, &ClipboardWidget::closeRequested);
    bottomLayout->addStretch();
    bottomLayout->addWidget(m_closeBtn);
    mainLayout->addLayout(bottomLayout);
}

void ClipboardWidget::loadItems() {
    if (!m_listLayout || !m_manager) return;
    
    QLayoutItem* child;
    while ((child = m_listLayout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            child->widget()->deleteLater();
        }
        delete child;
    }

    QList<ClipboardItem> items;

    if (!m_currentSearch.isEmpty()) {
        items = m_manager->search(m_currentSearch);
    } else if (m_currentFilter == 0) {
        items = m_manager->getAllItems();
    } else if (m_currentFilter == 4) {
        items = m_manager->getFavorites();
    } else {
        items = m_manager->getItemsByType(static_cast<ClipboardType>(m_currentFilter - 1));
    }

    for (const ClipboardItem& item : items) {
        ClipboardItemWidget* widget = new ClipboardItemWidget(item, this);
        connect(widget, &ClipboardItemWidget::copyClicked, this, &ClipboardWidget::onItemCopy);
        connect(widget, &ClipboardItemWidget::favoriteClicked, this, &ClipboardWidget::onItemFavorite);
        connect(widget, &ClipboardItemWidget::deleteClicked, this, &ClipboardWidget::onItemDelete);
        connect(widget, &ClipboardItemWidget::viewDetails, this, &ClipboardWidget::onItemViewDetails);
        m_listLayout->insertWidget(m_listLayout->count() - 1, widget);
    }
}

void ClipboardWidget::refresh() {
    loadItems();
}

void ClipboardWidget::onSearchChanged(const QString& text) {
    m_currentSearch = text;
    loadItems();
}

void ClipboardWidget::onFilterChanged(int index) {
    m_currentFilter = m_filterCombo->itemData(index).toInt();
    loadItems();
}

void ClipboardWidget::onItemCopy(int id) {
    if (m_manager) {
        m_manager->copyToClipboard(id);
    }
}

void ClipboardWidget::onItemFavorite(int id) {
    if (!m_manager) return;
    
    QList<ClipboardItem> items = m_manager->getAllItems();
    ClipboardItem item;
    for (const ClipboardItem& it : items) {
        if (it.id == id) {
            item = it;
            break;
        }
    }
    if (item.id != 0) {
        if (item.isFavorite) {
            m_manager->removeFromFavorites(id);
        } else {
            m_manager->addToFavorites(id);
        }
    }
}

void ClipboardWidget::onItemDelete(int id) {
    if (m_manager) {
        m_manager->deleteItem(id);
    }
}

void ClipboardWidget::onItemViewDetails(int id) {
    if (!m_manager) return;
    
    ClipboardItem item = m_manager->getItemById(id);
    if (item.id == 0) return;
    
    QDialog* dialog = new QDialog(this);
    dialog->setWindowTitle("内容详情");
    dialog->setMinimumSize(500, 400);
    
    QVBoxLayout* layout = new QVBoxLayout(dialog);
    
    QLabel* infoLabel = new QLabel(QString("类型: %1 | 时间: %2")
        .arg(item.typeToString())
        .arg(item.timestamp.toString("yyyy-MM-dd HH:mm:ss")), dialog);
    infoLabel->setStyleSheet("QLabel { color: #7f8c8d; font-size: 12px; }");
    layout->addWidget(infoLabel);
    
    QTextEdit* textEdit = new QTextEdit(dialog);
    textEdit->setPlainText(item.content);
    textEdit->setReadOnly(true);
    textEdit->setStyleSheet("QTextEdit { background-color: white; border: 1px solid #dcdde1; border-radius: 5px; }");
    layout->addWidget(textEdit);
    
    QHBoxLayout* btnLayout = new QHBoxLayout();
    QPushButton* copyBtn = new QPushButton("复制全部", dialog);
    copyBtn->setStyleSheet("QPushButton { padding: 8px 20px; background-color: #3498db; color: white; border: none; border-radius: 5px; }");
    connect(copyBtn, &QPushButton::clicked, this, [this, id, dialog]() {
        m_manager->copyToClipboard(id);
        dialog->close();
    });
    
    QPushButton* closeBtn = new QPushButton("关闭", dialog);
    closeBtn->setStyleSheet("QPushButton { padding: 8px 20px; background-color: #95a5a6; color: white; border: none; border-radius: 5px; }");
    connect(closeBtn, &QPushButton::clicked, dialog, &QDialog::close);
    
    btnLayout->addStretch();
    btnLayout->addWidget(copyBtn);
    btnLayout->addWidget(closeBtn);
    layout->addLayout(btnLayout);
    
    dialog->exec();
    dialog->deleteLater();
}

void ClipboardWidget::onItemAdded(const ClipboardItem& item) {
    Q_UNUSED(item);
    loadItems();
}

void ClipboardWidget::onItemDeleted(int id) {
    Q_UNUSED(id);
    loadItems();
}

void ClipboardWidget::onClearAll() {
    QMessageBox::StandardButton reply = QMessageBox::question(this, "确认清空", 
        "确定要清空所有剪贴板历史吗？", 
        QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        if (m_manager) {
            m_manager->clearAll();
        }
        loadItems();
    }
}

void ClipboardWidget::onRefresh() {
    loadItems();
}

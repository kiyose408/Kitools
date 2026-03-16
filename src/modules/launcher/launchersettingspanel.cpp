#include "launchersettingspanel.h"
#include <QDebug>

LauncherSettingsPanel::LauncherSettingsPanel(QWidget* parent)
    : QWidget(parent)
    , m_indexer(nullptr)
    , m_searchEdit(nullptr)
    , m_itemsList(nullptr)
    , m_statsLabel(nullptr)
    , m_refreshBtn(nullptr)
{
    qDebug() << "LauncherSettingsPanel 初始化开始";
    
    m_indexer = LauncherIndexer::instance(this);
    
    setupUI();
    
    connect(m_indexer, &LauncherIndexer::indexingFinished, this, &LauncherSettingsPanel::onIndexingFinished);
    
    qDebug() << "LauncherSettingsPanel 初始化完成";
}

LauncherSettingsPanel::~LauncherSettingsPanel() {
}

void LauncherSettingsPanel::setupUI() {
    setWindowTitle("快捷启动器设置");
    setMinimumSize(400, 500);
    setStyleSheet("QWidget { background-color: #f5f6fa; }");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    QLabel* titleLabel = new QLabel("快捷启动器", this);
    titleLabel->setStyleSheet("QLabel { font-size: 18px; font-weight: bold; color: #2c3e50; }");
    mainLayout->addWidget(titleLabel);

    QHBoxLayout* searchLayout = new QHBoxLayout();
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("搜索应用...");
    m_searchEdit->setStyleSheet("QLineEdit { padding: 8px; border: 1px solid #dcdde1; border-radius: 5px; background-color: white; }");
    connect(m_searchEdit, &QLineEdit::textChanged, this, &LauncherSettingsPanel::onSearchChanged);
    searchLayout->addWidget(m_searchEdit);

    m_refreshBtn = new QPushButton("刷新索引", this);
    m_refreshBtn->setStyleSheet("QPushButton { padding: 8px 15px; background-color: #3498db; color: white; border: none; border-radius: 5px; }");
    connect(m_refreshBtn, &QPushButton::clicked, this, &LauncherSettingsPanel::onRefreshClicked);
    searchLayout->addWidget(m_refreshBtn);
    mainLayout->addLayout(searchLayout);

    QGroupBox* listGroup = new QGroupBox("应用列表", this);
    listGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #2c3e50; }");

    QVBoxLayout* listLayout = new QVBoxLayout(listGroup);
    m_itemsList = new QListWidget(this);
    m_itemsList->setStyleSheet("QListWidget { background-color: white; border: 1px solid #dcdde1; border-radius: 5px; }");
    m_itemsList->setSpacing(2);
    connect(m_itemsList, &QListWidget::itemDoubleClicked, this, &LauncherSettingsPanel::onItemDoubleClicked);
    listLayout->addWidget(m_itemsList);

    mainLayout->addWidget(listGroup);

    m_statsLabel = new QLabel("已索引: 0 个应用", this);
    m_statsLabel->setStyleSheet("QLabel { color: #7f8c8d; font-size: 12px; }");
    mainLayout->addWidget(m_statsLabel);

    mainLayout->addStretch();

    QPushButton* backBtn = new QPushButton("返回", this);
    backBtn->setStyleSheet("QPushButton { padding: 10px 20px; background-color: #95a5a6; color: white; border: none; border-radius: 5px; }");
    connect(backBtn, &QPushButton::clicked, this, &LauncherSettingsPanel::backRequested);
    mainLayout->addWidget(backBtn);

    loadItems();
}

void LauncherSettingsPanel::loadItems() {
    if (!m_indexer || !m_itemsList) return;
    
    m_itemsList->clear();
    
    QString searchText = m_searchEdit ? m_searchEdit->text() : "";
    QList<LauncherItem> items = m_indexer->search(searchText, 50);
    
    for (const LauncherItem& item : items) {
        QString displayText = QString("%1 %2").arg(item.isPinned ? "★" : "☆").arg(item.name);
        QListWidgetItem* listItem = new QListWidgetItem(displayText, m_itemsList);
        listItem->setData(Qt::UserRole, item.id);
        listItem->setToolTip(item.path);
    }
    
    updateStats();
}

void LauncherSettingsPanel::updateStats() {
    if (m_statsLabel && m_indexer) {
        int total = m_indexer->getTotalCount();
        int pinned = m_indexer->getPinnedItems().size();
        m_statsLabel->setText(QString("已索引: %1 个应用 | 已固定: %2 个").arg(total).arg(pinned));
    }
}

void LauncherSettingsPanel::onRefreshClicked() {
    qDebug() << "刷新索引按钮点击";
    m_refreshBtn->setEnabled(false);
    m_refreshBtn->setText("索引中...");
    emit refreshRequested();
}

void LauncherSettingsPanel::onSearchChanged(const QString& text) {
    Q_UNUSED(text);
    loadItems();
}

void LauncherSettingsPanel::onItemDoubleClicked(QListWidgetItem* item) {
    if (!item || !m_indexer) return;
    
    int id = item->data(Qt::UserRole).toInt();
    m_indexer->launchItem(id);
    qDebug() << "启动应用ID:" << id;
}

void LauncherSettingsPanel::onIndexingFinished(int totalCount) {
    qDebug() << "索引完成，共" << totalCount << "个应用";
    m_refreshBtn->setEnabled(true);
    m_refreshBtn->setText("刷新索引");
    loadItems();
}

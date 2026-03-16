#include "launchersettingspanel.h"
#include <QComboBox>
#include <QTimer>
#include <QDebug>

LauncherSettingsPanel::LauncherSettingsPanel(QWidget* parent)
    : QWidget(parent)
    , m_indexer(nullptr)
    , m_searchEdit(nullptr)
    , m_categoryCombo(nullptr)
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
    setMinimumSize(500, 600);
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

    QHBoxLayout* filterLayout = new QHBoxLayout();
    QLabel* filterLabel = new QLabel("分类筛选:", this);
    filterLabel->setStyleSheet("QLabel { color: #2c3e50; font-size: 12px; }");
    filterLayout->addWidget(filterLabel);

    m_categoryCombo = new QComboBox(this);
    m_categoryCombo->addItem("全部应用", "all");
    m_categoryCombo->addItem("常用应用", "frequent");
    m_categoryCombo->addItem("最近使用", "recent");
    m_categoryCombo->addItem("已固定", "pinned");
    m_categoryCombo->addItem("系统工具", "system");
    m_categoryCombo->addItem("开发工具", "development");
    m_categoryCombo->addItem("办公软件", "office");
    m_categoryCombo->addItem("网络应用", "network");
    m_categoryCombo->addItem("多媒体", "media");
    m_categoryCombo->addItem("其他", "other");
    m_categoryCombo->setStyleSheet("QComboBox { padding: 5px 10px; border: 1px solid #dcdde1; border-radius: 5px; background-color: white; min-width: 120px; }");
    connect(m_categoryCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &LauncherSettingsPanel::onCategoryChanged);
    filterLayout->addWidget(m_categoryCombo);
    filterLayout->addStretch();
    mainLayout->addLayout(filterLayout);

    QGroupBox* listGroup = new QGroupBox("应用列表", this);
    listGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #2c3e50; }");

    QVBoxLayout* listLayout = new QVBoxLayout(listGroup);
    m_itemsList = new QListWidget(this);
    m_itemsList->setStyleSheet("QListWidget { background-color: white; border: 1px solid #dcdde1; border-radius: 5px; }");
    m_itemsList->setSpacing(2);
    m_itemsList->setAlternatingRowColors(true);
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

QString LauncherSettingsPanel::getCurrentCategory() const {
    if (!m_categoryCombo) return "all";
    return m_categoryCombo->currentData().toString();
}

void LauncherSettingsPanel::loadItems() {
    if (!m_indexer || !m_itemsList) return;
    
    m_itemsList->clear();
    
    QString searchText = m_searchEdit ? m_searchEdit->text() : "";
    QString category = getCurrentCategory();
    
    QList<LauncherItem> allItems = m_indexer->search(searchText, 0);
    QList<LauncherItem> filteredItems;
    
    for (const LauncherItem& item : allItems) {
        bool match = false;
        
        if (category == "all") {
            match = true;
        } else if (category == "frequent") {
            match = item.launchCount > 5;
        } else if (category == "recent") {
            QDateTime weekAgo = QDateTime::currentDateTime().addDays(-7);
            match = item.lastLaunched.isValid() && item.lastLaunched > weekAgo;
        } else if (category == "pinned") {
            match = item.isPinned;
        } else if (category == "system") {
            QString pathLower = item.path.toLower();
            match = pathLower.contains("windows") || 
                    pathLower.contains("system32") ||
                    pathLower.contains("syswow64") ||
                    item.name.toLower().contains("windows") ||
                    item.name.toLower().contains("cmd") ||
                    item.name.toLower().contains("powershell") ||
                    item.name.toLower().contains("notepad") ||
                    item.name.toLower().contains("计算器") ||
                    item.name.toLower().contains("记事本");
        } else if (category == "development") {
            QString nameLower = item.name.toLower();
            match = nameLower.contains("visual studio") ||
                    nameLower.contains("vs code") ||
                    nameLower.contains("code") ||
                    nameLower.contains("git") ||
                    nameLower.contains("node") ||
                    nameLower.contains("python") ||
                    nameLower.contains("java") ||
                    nameLower.contains("idea") ||
                    nameLower.contains("eclipse") ||
                    nameLower.contains("qt") ||
                    nameLower.contains("mingw") ||
                    nameLower.contains("cmake") ||
                    nameLower.contains("debug");
        } else if (category == "office") {
            QString nameLower = item.name.toLower();
            match = nameLower.contains("word") ||
                    nameLower.contains("excel") ||
                    nameLower.contains("powerpoint") ||
                    nameLower.contains("office") ||
                    nameLower.contains("wps") ||
                    nameLower.contains("excel") ||
                    nameLower.contains("文档") ||
                    nameLower.contains("表格") ||
                    nameLower.contains("ppt") ||
                    nameLower.contains("outlook") ||
                    nameLower.contains("onenote");
        } else if (category == "network") {
            QString nameLower = item.name.toLower();
            match = nameLower.contains("chrome") ||
                    nameLower.contains("firefox") ||
                    nameLower.contains("edge") ||
                    nameLower.contains("browser") ||
                    nameLower.contains("浏览器") ||
                    nameLower.contains("wechat") ||
                    nameLower.contains("微信") ||
                    nameLower.contains("qq") ||
                    nameLower.contains("tim") ||
                    nameLower.contains("dingtalk") ||
                    nameLower.contains("钉钉") ||
                    nameLower.contains("telegram") ||
                    nameLower.contains("discord") ||
                    nameLower.contains("slack") ||
                    nameLower.contains("teams");
        } else if (category == "media") {
            QString nameLower = item.name.toLower();
            match = nameLower.contains("vlc") ||
                    nameLower.contains("player") ||
                    nameLower.contains("播放器") ||
                    nameLower.contains("music") ||
                    nameLower.contains("音乐") ||
                    nameLower.contains("video") ||
                    nameLower.contains("视频") ||
                    nameLower.contains("spotify") ||
                    nameLower.contains("netease") ||
                    nameLower.contains("photoshop") ||
                    nameLower.contains("lightroom") ||
                    nameLower.contains("premiere");
        } else if (category == "other") {
            QString nameLower = item.name.toLower();
            QString pathLower = item.path.toLower();
            bool isSystem = pathLower.contains("windows") || pathLower.contains("system32");
            bool isDev = nameLower.contains("visual studio") || nameLower.contains("code") ||
                        nameLower.contains("git") || nameLower.contains("python");
            bool isOffice = nameLower.contains("word") || nameLower.contains("excel") ||
                          nameLower.contains("office") || nameLower.contains("wps");
            bool isNetwork = nameLower.contains("chrome") || nameLower.contains("firefox") ||
                            nameLower.contains("edge") || nameLower.contains("wechat") ||
                            nameLower.contains("qq");
            bool isMedia = nameLower.contains("player") || nameLower.contains("music") ||
                          nameLower.contains("video") || nameLower.contains("vlc");
            match = !isSystem && !isDev && !isOffice && !isNetwork && !isMedia;
        }
        
        if (match) {
            filteredItems.append(item);
        }
    }
    
    int maxDisplay = 100;
    if (filteredItems.size() > maxDisplay) {
        filteredItems = filteredItems.mid(0, maxDisplay);
    }
    
    for (const LauncherItem& item : filteredItems) {
        QString displayText = QString("%1 %2 [%3]")
            .arg(item.isPinned ? "★" : "☆")
            .arg(item.name)
            .arg(item.launchCount > 0 ? QString::number(item.launchCount) : "新");
        QListWidgetItem* listItem = new QListWidgetItem(displayText, m_itemsList);
        listItem->setData(Qt::UserRole, item.id);
        listItem->setToolTip(QString("%1\n路径: %2\n启动次数: %3")
            .arg(item.name)
            .arg(item.path)
            .arg(item.launchCount));
    }
    
    updateStats();
}

void LauncherSettingsPanel::updateStats() {
    if (m_statsLabel && m_indexer) {
        int total = m_indexer->getTotalCount();
        int pinned = m_indexer->getPinnedItems().size();
        int displayed = m_itemsList ? m_itemsList->count() : 0;
        m_statsLabel->setText(QString("已索引: %1 个应用 | 已固定: %2 个 | 当前显示: %3 个")
            .arg(total).arg(pinned).arg(displayed));
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

void LauncherSettingsPanel::onCategoryChanged(int index) {
    Q_UNUSED(index);
    loadItems();
}

void LauncherSettingsPanel::onItemDoubleClicked(QListWidgetItem* item) {
    if (!item || !m_indexer) return;
    
    int id = item->data(Qt::UserRole).toInt();
    m_indexer->launchItem(id);
    qDebug() << "启动应用ID:" << id;
    
    QTimer::singleShot(500, this, [this]() {
        loadItems();
    });
}

void LauncherSettingsPanel::onIndexingFinished(int totalCount) {
    qDebug() << "索引完成，共" << totalCount << "个应用";
    m_refreshBtn->setEnabled(true);
    m_refreshBtn->setText("刷新索引");
    loadItems();
}

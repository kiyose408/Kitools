#include "launcherindexer.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QProcess>
#include <QJsonArray>
#include <QJsonDocument>
#include <QCoreApplication>
#include <algorithm>

LauncherIndexer* LauncherIndexer::m_instance = nullptr;

LauncherIndexer::LauncherIndexer(QObject* parent)
    : QObject(parent)
    , m_watcher(nullptr)
{
    qDebug() << "LauncherIndexer 初始化开始";
    
    QDir appDataDir(QCoreApplication::applicationDirPath());
    appDataDir.mkpath("data");
    m_dataFilePath = appDataDir.filePath("data/launcher.json");

    loadFromFile();

    m_watcher = new QFileSystemWatcher(this);
    
    QString homeDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    
    m_scanPaths << QStringList{
        QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation),
        QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
        homeDir + "/AppData/Roaming/Microsoft/Windows/Start Menu",
        homeDir + "/AppData/Roaming/Microsoft/Windows/Start Menu/Programs",
        "C:/ProgramData/Microsoft/Windows/Start Menu",
        "C:/ProgramData/Microsoft/Windows/Start Menu/Programs",
        "C:/Program Files",
        "C:/Program Files (x86)",
        "C:/Program Files (x86)/WindowsApps",
        "D:/Program Files",
        "D:/Program Files (x86)",
        "E:/Program Files",
        "E:/Program Files (x86)",
        homeDir + "/AppData/Local/Microsoft/WindowsApps",
        homeDir + "/AppData/Local/Programs",
        "C:/Windows/System32",
        "C:/Windows/SysWOW64"
    };

    for (const QString& path : m_scanPaths) {
        if (QDir(path).exists()) {
            m_watcher->addPath(path);
        }
    }
    connect(m_watcher, &QFileSystemWatcher::directoryChanged, this, &LauncherIndexer::onDirectoryChanged);

    qDebug() << "LauncherIndexer 初始化完成，扫描路径:" << m_scanPaths.size() << "个";
}

LauncherIndexer::~LauncherIndexer() {
    saveToFile();
}

LauncherIndexer* LauncherIndexer::instance(QObject* parent) {
    if (!m_instance) {
        m_instance = new LauncherIndexer(parent);
    }
    return m_instance;
}

void LauncherIndexer::startIndexing() {
    qDebug() << "开始索引应用";
    emit indexingStarted();
    
    m_items.clear();
    
    scanStartMenu();
    scanDesktop();
    scanCommonPaths();
    
    saveToFile();
    
    qDebug() << "索引完成，共" << m_items.size() << "个应用";
    emit indexingFinished(m_items.size());
}

void LauncherIndexer::refreshIndex() {
    startIndexing();
}

QList<LauncherItem> LauncherIndexer::getAllItems() const {
    return m_items;
}

QList<LauncherItem> LauncherIndexer::search(const QString& query, int maxResults) const {
    QList<LauncherItem> results;
    if (query.isEmpty()) {
        results = m_items;
    } else {
        QString lowerQuery = query.toLower();
        for (const LauncherItem& item : m_items) {
            if (item.name.toLower().contains(lowerQuery)) {
                results.append(item);
                continue;
            }
            for (const QString& kw : item.keywords) {
                if (kw.toLower().contains(lowerQuery)) {
                    results.append(item);
                    break;
                }
            }
        }
    }

    std::sort(results.begin(), results.end(), [](const LauncherItem& a, const LauncherItem& b) {
        if (a.isPinned != b.isPinned) {
            return a.isPinned;
        }
        if (a.launchCount != b.launchCount) {
            return a.launchCount > b.launchCount;
        }
        return a.lastLaunched > b.lastLaunched;
    });

    if (maxResults > 0 && results.size() > maxResults) {
        results = results.mid(0, maxResults);
    }

    return results;
}

QList<LauncherItem> LauncherIndexer::getPinnedItems() const {
    QList<LauncherItem> results;
    for (const LauncherItem& item : m_items) {
        if (item.isPinned) {
            results.append(item);
        }
    }
    return results;
}

LauncherItem LauncherIndexer::getItemById(int id) const {
    for (const LauncherItem& item : m_items) {
        if (item.id == id) {
            return item;
        }
    }
    return LauncherItem();
}

void LauncherIndexer::launchItem(int id) {
    for (const LauncherItem& item : m_items) {
        if (item.id == id) {
            QProcess::startDetached(item.path, QStringList() << item.arguments.split(" "));
            incrementLaunchCount(id);
            emit itemLaunched(item);
            qDebug() << "启动应用:" << item.name;
            break;
        }
    }
}

void LauncherIndexer::incrementLaunchCount(int id) {
    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items[i].id == id) {
            m_items[i].launchCount++;
            m_items[i].lastLaunched = QDateTime::currentDateTime();
            saveToFile();
            break;
        }
    }
}

void LauncherIndexer::addCustomItem(const LauncherItem& item) {
    LauncherItem newItem = item;
    newItem.id = generateId();
    newItem.addedTime = QDateTime::currentDateTime();
    m_items.append(newItem);
    saveToFile();
    emit itemsChanged();
    qDebug() << "添加自定义项目:" << newItem.name;
}

void LauncherIndexer::removeItem(int id) {
    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items[i].id == id) {
            m_items.removeAt(i);
            saveToFile();
            emit itemsChanged();
            qDebug() << "移除项目:" << id;
            break;
        }
    }
}

void LauncherIndexer::pinItem(int id, bool pinned) {
    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items[i].id == id) {
            m_items[i].isPinned = pinned;
            saveToFile();
            emit itemsChanged();
            qDebug() << "固定项目:" << id << "状态:" << pinned;
            break;
        }
    }
}

int LauncherIndexer::getTotalCount() const {
    return m_items.size();
}

void LauncherIndexer::onDirectoryChanged(const QString& path) {
    qDebug() << "目录变化:" << path;
    startIndexing();
}

void LauncherIndexer::scanStartMenu() {
    QString homeDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    
    QStringList startMenuPaths;
    startMenuPaths << QStringList{
        homeDir + "/AppData/Roaming/Microsoft/Windows/Start Menu",
        homeDir + "/AppData/Roaming/Microsoft/Windows/Start Menu/Programs",
        "C:/ProgramData/Microsoft/Windows/Start Menu",
        "C:/ProgramData/Microsoft/Windows/Start Menu/Programs"
    };
    
    for (const QString& path : startMenuPaths) {
        scanDirectory(path, LauncherItemType::Application);
    }
}

void LauncherIndexer::scanDesktop() {
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    scanDirectory(desktopPath, LauncherItemType::Application);
    
    QString publicDesktop = "C:/Users/Public/Desktop";
    scanDirectory(publicDesktop, LauncherItemType::Application);
}

void LauncherIndexer::scanCommonPaths() {
    QString homeDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    
    QStringList commonPaths;
    commonPaths << QStringList{
        "C:/Program Files",
        "C:/Program Files (x86)",
        "C:/Program Files (x86)/WindowsApps",
        "D:/Program Files",
        "D:/Program Files (x86)",
        "E:/Program Files",
        "E:/Program Files (x86)",
        homeDir + "/AppData/Local/Microsoft/WindowsApps",
        homeDir + "/AppData/Local/Programs",
        "C:/Windows/System32",
        "C:/Windows/SysWOW64"
    };
    
    for (const QString& path : commonPaths) {
        scanDirectory(path, LauncherItemType::Application);
    }
}

void LauncherIndexer::scanDirectory(const QString& directory, LauncherItemType type, int depth) {
    if (depth > 5) {
        return;
    }
    
    QDir dir(directory);
    if (!dir.exists()) {
        return;
    }

    dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    QFileInfoList entries = dir.entryInfoList();

    int total = entries.size();
    int current = 0;

    for (const QFileInfo& entry : entries) {
        current++;
        
        if (current % 10 == 0) {
            emit indexingProgress(current, total);
        }

        if (entry.isDir()) {
            QString dirName = entry.fileName().toLower();
            if (dirName.startsWith("$") || dirName.startsWith(".") || 
                dirName == "windows" || dirName == "programdata" ||
                dirName.contains("cache") || dirName.contains("temp") ||
                dirName.contains("log") || dirName.contains("backup")) {
                continue;
            }
            scanDirectory(entry.absoluteFilePath(), type, depth + 1);
            continue;
        }

        QString suffix = entry.suffix().toLower();
        if (suffix == "lnk") {
            parseLnkFile(entry.absoluteFilePath());
        } else if (suffix == "exe") {
            parseExeFile(entry.absoluteFilePath());
        } else if (suffix == "url") {
            parseUrlFile(entry.absoluteFilePath());
        } else if (suffix == "msi") {
            parseMsiFile(entry.absoluteFilePath());
        } else if (suffix == "bat" || suffix == "cmd") {
            parseScriptFile(entry.absoluteFilePath());
        }
    }
}

void LauncherIndexer::parseLnkFile(const QString& lnkPath) {
    QFileInfo info(lnkPath);
    QString baseName = info.completeBaseName();
    
    if (baseName.isEmpty()) {
        return;
    }

    if (isDuplicate(lnkPath)) {
        return;
    }

    LauncherItem item;
    item.id = generateId();
    item.name = baseName;
    item.path = lnkPath;
    item.iconPath = lnkPath;
    item.addedTime = QDateTime::currentDateTime();
    
    QStringList nameParts = baseName.split(" ");
    for (const QString& part : nameParts) {
        if (part.length() > 1) {
            item.keywords.append(part.toLower());
        }
    }
    
    m_items.append(item);
}

void LauncherIndexer::parseExeFile(const QString& exePath) {
    QFileInfo info(exePath);
    QString baseName = info.completeBaseName();
    
    if (baseName.isEmpty()) {
        return;
    }

    if (isDuplicate(exePath)) {
        return;
    }

    LauncherItem item;
    item.id = generateId();
    item.name = baseName;
    item.path = exePath;
    item.iconPath = exePath;
    item.addedTime = QDateTime::currentDateTime();
    
    QStringList nameParts = baseName.split(" ");
    for (const QString& part : nameParts) {
        if (part.length() > 1) {
            item.keywords.append(part.toLower());
        }
    }
    
    m_items.append(item);
}

void LauncherIndexer::parseUrlFile(const QString& urlPath) {
    QFileInfo info(urlPath);
    QString baseName = info.completeBaseName();
    
    if (baseName.isEmpty()) {
        return;
    }

    if (isDuplicate(urlPath)) {
        return;
    }

    LauncherItem item;
    item.id = generateId();
    item.name = baseName;
    item.path = urlPath;
    item.type = LauncherItemType::Url;
    item.iconPath = urlPath;
    item.addedTime = QDateTime::currentDateTime();
    
    m_items.append(item);
}

void LauncherIndexer::parseMsiFile(const QString& msiPath) {
    QFileInfo info(msiPath);
    QString baseName = info.completeBaseName();
    
    if (baseName.isEmpty()) {
        return;
    }

    if (isDuplicate(msiPath)) {
        return;
    }

    LauncherItem item;
    item.id = generateId();
    item.name = baseName;
    item.path = msiPath;
    item.iconPath = msiPath;
    item.addedTime = QDateTime::currentDateTime();
    
    QStringList nameParts = baseName.split(" ");
    for (const QString& part : nameParts) {
        if (part.length() > 1) {
            item.keywords.append(part.toLower());
        }
    }
    
    m_items.append(item);
}

void LauncherIndexer::parseScriptFile(const QString& scriptPath) {
    QFileInfo info(scriptPath);
    QString baseName = info.completeBaseName();
    
    if (baseName.isEmpty()) {
        return;
    }

    if (isDuplicate(scriptPath)) {
        return;
    }

    LauncherItem item;
    item.id = generateId();
    item.name = baseName;
    item.path = scriptPath;
    item.iconPath = scriptPath;
    item.addedTime = QDateTime::currentDateTime();
    
    QStringList nameParts = baseName.split(" ");
    for (const QString& part : nameParts) {
        if (part.length() > 1) {
            item.keywords.append(part.toLower());
        }
    }
    
    m_items.append(item);
}

void LauncherIndexer::loadFromFile() {
    QFile file(m_dataFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "启动器数据文件不存在";
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray()) {
        return;
    }

    QJsonArray array = doc.array();
    for (const QJsonValue& value : array) {
        if (value.isObject()) {
            m_items.append(LauncherItem::fromJson(value.toObject()));
        }
    }
    qDebug() << "已加载" << m_items.size() << "个启动器项目";
}

void LauncherIndexer::saveToFile() {
    QJsonArray array;
    for (const LauncherItem& item : m_items) {
        array.append(item.toJson());
    }

    QFile file(m_dataFilePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "无法保存启动器数据文件";
        return;
    }

    QJsonDocument doc(array);
    file.write(doc.toJson());
    file.close();
}

int LauncherIndexer::generateId() {
    static int id = 0;
    return ++id;
}

bool LauncherIndexer::isDuplicate(const QString& path) const {
    for (const LauncherItem& item : m_items) {
        if (item.path == path) {
            return true;
        }
    }
    return false;
}

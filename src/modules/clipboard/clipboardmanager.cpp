#include "clipboardmanager.h"
#include <QDebug>

ClipboardManager* ClipboardManager::m_instance = nullptr;

ClipboardManager::ClipboardManager(QObject* parent)
    : QObject(parent)
    , m_isMonitoring(false)
    , m_maxItems(1000)
{
    qDebug() << "ClipboardManager 构造函数开始";
    
    QDir appDataDir(QCoreApplication::applicationDirPath());
    appDataDir.mkpath("data");
    m_dataFilePath = appDataDir.filePath("data/clipboard.json");

    loadFromFile();

    qDebug() << "ClipboardManager 构造函数完成，数据文件:" << m_dataFilePath;
}

ClipboardManager::~ClipboardManager() {
    stopMonitoring();
    saveToFile();
}

ClipboardManager* ClipboardManager::instance(QObject* parent) {
    if (!m_instance) {
        m_instance = new ClipboardManager(parent);
    }
    return m_instance;
}

void ClipboardManager::startMonitoring() {
    if (!m_isMonitoring) {
        m_isMonitoring = true;
        QClipboard* clipboard = QApplication::clipboard();
        connect(clipboard, &QClipboard::dataChanged, this, &ClipboardManager::onClipboardChanged);
        qDebug() << "剪贴板监控已启动";
    }
}

void ClipboardManager::stopMonitoring() {
    if (m_isMonitoring) {
        m_isMonitoring = false;
        QClipboard* clipboard = QApplication::clipboard();
        disconnect(clipboard, &QClipboard::dataChanged, this, &ClipboardManager::onClipboardChanged);
        qDebug() << "剪贴板监控已停止";
    }
}

void ClipboardManager::onClipboardChanged() {
    qDebug() << "剪贴板内容变化检测到";
    processClipboardContent();
}

void ClipboardManager::processClipboardContent() {
    QClipboard* clipboard = QApplication::clipboard();
    const QMimeData* mimeData = clipboard->mimeData();

    if (!mimeData) {
        qDebug() << "剪贴板数据为空";
        return;
    }

    ClipboardItem item;
    item.id = generateId();
    item.timestamp = QDateTime::currentDateTime();
    item.isFavorite = false;

    if (mimeData->hasUrls()) {
        item.type = ClipboardType::File;
        QList<QUrl> urls = mimeData->urls();
        QStringList filePaths;
        for (int i = 0; i < urls.size(); ++i) {
            const QUrl& url = urls.at(i);
            if (url.isLocalFile()) {
                filePaths.append(url.toLocalFile());
            }
        }
        item.content = filePaths.join("\n");
        qDebug() << "检测到文件:" << item.content.left(50);
    } else if (mimeData->hasImage()) {
        item.type = ClipboardType::Image;
        QImage image = qvariant_cast<QImage>(mimeData->imageData());
        if (!image.isNull()) {
            item.content = "图片数据";
            item.preview = QString("图片 %1x%2").arg(image.width()).arg(image.height());
            qDebug() << "检测到图片:" << item.preview;
        }
    } else if (mimeData->hasText()) {
        QString text = mimeData->text();
        if (mimeData->hasHtml() && !mimeData->html().isEmpty()) {
            item.type = ClipboardType::Html;
            item.content = text;
            qDebug() << "检测到网页内容(已提取纯文本):" << text.left(50);
        } else {
            item.type = ClipboardType::Text;
            item.content = text;
            qDebug() << "检测到文本内容:" << text.left(50);
        }
    }

    if (!item.content.isEmpty()) {
        if (isDuplicate(item.content)) {
            qDebug() << "内容重复，跳过。当前列表大小:" << m_items.size();
            return;
        }
        item.preview = generatePreview(item.content, 200);
        m_items.prepend(item);
        emit itemAdded(item);
        emit countChanged();
        qDebug() << "新项目已添加，总数:" << m_items.size();

        while (m_items.size() > m_maxItems) {
            ClipboardItem removed = m_items.takeLast();
            if (!removed.isFavorite) {
                emit itemDeleted(removed.id);
            }
        }

        saveToFile();
    } else {
        qDebug() << "内容为空或重复，跳过";
    }
}

QList<ClipboardItem> ClipboardManager::getAllItems() const {
    return m_items;
}

QList<ClipboardItem> ClipboardManager::getItemsByType(ClipboardType type) const {
    QList<ClipboardItem> result;
    for (int i = 0; i < m_items.size(); ++i) {
        const ClipboardItem& item = m_items.at(i);
        if (item.type == type) {
            result.append(item);
        }
    }
    return result;
}

QList<ClipboardItem> ClipboardManager::getFavorites() const {
    QList<ClipboardItem> result;
    for (int i = 0; i < m_items.size(); ++i) {
        const ClipboardItem& item = m_items.at(i);
        if (item.isFavorite) {
            result.append(item);
        }
    }
    return result;
}

QList<ClipboardItem> ClipboardManager::search(const QString& keyword) const {
    QList<ClipboardItem> result;
    if (keyword.isEmpty()) return m_items;

    for (int i = 0; i < m_items.size(); ++i) {
        const ClipboardItem& item = m_items.at(i);
        if (item.content.contains(keyword, Qt::CaseInsensitive) ||
            item.preview.contains(keyword, Qt::CaseInsensitive)) {
            result.append(item);
        }
    }
    return result;
}

void ClipboardManager::addToFavorites(int id) {
    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items[i].id == id) {
            m_items[i].isFavorite = true;
            emit itemUpdated(m_items[i]);
            emit favoritesChanged();
            saveToFile();
            break;
        }
    }
}

void ClipboardManager::removeFromFavorites(int id) {
    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items[i].id == id) {
            m_items[i].isFavorite = false;
            emit itemUpdated(m_items[i]);
            emit favoritesChanged();
            saveToFile();
            break;
        }
    }
}

void ClipboardManager::deleteItem(int id) {
    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items[i].id == id) {
            m_items.removeAt(i);
            emit itemDeleted(id);
            emit countChanged();
            saveToFile();
            break;
        }
    }
}

void ClipboardManager::clearAll() {
    qDebug() << "clearAll 开始，当前条数:" << m_items.size();
    m_items.clear();
    qDebug() << "m_items 已清空，当前条数:" << m_items.size();
    emit countChanged();
    saveToFile();
    qDebug() << "已保存到文件";
}

void ClipboardManager::copyToClipboard(int id) {
    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items[i].id == id) {
            QClipboard* clipboard = QApplication::clipboard();
            clipboard->setText(m_items[i].content);
            qDebug() << "已复制到剪贴板:" << m_items[i].content.left(50);
            break;
        }
    }
}

ClipboardItem ClipboardManager::getItemById(int id) const {
    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items.at(i).id == id) {
            return m_items.at(i);
        }
    }
    return ClipboardItem();
}

int ClipboardManager::getTotalCount() const {
    return m_items.size();
}

int ClipboardManager::getCountByType(ClipboardType type) const {
    int count = 0;
    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items.at(i).type == type) {
            count++;
        }
    }
    return count;
}

void ClipboardManager::loadFromFile() {
    QFile file(m_dataFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "数据文件不存在，将创建新文件";
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray()) {
        qDebug() << "数据文件格式错误";
        return;
    }

    QJsonArray array = doc.array();
    for (const QJsonValue& value : array) {
        if (value.isObject()) {
            ClipboardItem item = ClipboardItem::fromJson(value.toObject());
            m_items.append(item);
        }
    }
    qDebug() << "已加载" << m_items.size() << "条剪贴板记录";
}

void ClipboardManager::saveToFile() {
    QJsonArray array;
    for (int i = 0; i < m_items.size(); ++i) {
        array.append(m_items.at(i).toJson());
    }

    QFile file(m_dataFilePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "无法保存数据文件:" << m_dataFilePath;
        return;
    }

    QJsonDocument doc(array);
    file.write(doc.toJson());
    file.close();
}

int ClipboardManager::generateId() {
    static int id = 0;
    return ++id;
}

QString ClipboardManager::generatePreview(const QString& content, int maxLength) {
    QString preview = content;
    preview.replace("\n", " ");
    preview.replace("\r", "");
    preview.replace("\t", " ");
    if (preview.length() > maxLength) {
        return preview.left(maxLength) + "...";
    }
    return preview;
}

bool ClipboardManager::isDuplicate(const QString& content) const {
    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items.at(i).content == content) {
            return true;
        }
    }
    return false;
}

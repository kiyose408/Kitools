#include "activitymanager.h"
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QCoreApplication>
#include <QDir>

ActivityManager* ActivityManager::m_instance = nullptr;

ActivityManager::ActivityManager(QObject* parent)
    : QObject(parent)
    , m_maxRecords(100000)
{
    qDebug() << "========== ActivityManager 初始化 ==========";
    
    QDir appDataDir(QCoreApplication::applicationDirPath());
    appDataDir.mkpath("data");
    m_dataFilePath = appDataDir.filePath("data/activity.json");
    
    qDebug() << "数据文件路径:" << m_dataFilePath;
    
    initDefaultCategoryRules();
    qDebug() << "已初始化" << m_categoryRules.size() << "条默认分类规则";
    
    loadFromFile();
    qDebug() << "已加载" << m_records.size() << "条活动记录";
    
    qDebug() << "ActivityManager 初始化完成";
}

ActivityManager::~ActivityManager() {
    saveToFile();
}

ActivityManager* ActivityManager::instance(QObject* parent) {
    if (!m_instance) {
        m_instance = new ActivityManager(parent);
    }
    return m_instance;
}

void ActivityManager::addActivityRecord(const ActivityRecord& record) {
    qDebug() << "========== 添加活动记录 ==========";
    qDebug() << "进程:" << record.processName;
    qDebug() << "标题:" << record.windowTitle;
    qDebug() << "持续时间:" << record.durationSeconds << "秒";
    
    ActivityRecord newRecord = record;
    newRecord.id = generateId();
    
    newRecord.category = categorizeActivity(newRecord);
    qDebug() << "分类结果:" << categoryToString(newRecord.category);
    
    m_records.append(newRecord);
    qDebug() << "记录已添加，总记录数:" << m_records.size();
    
    emit recordAdded(newRecord);
    qDebug() << "recordAdded 信号已发送";
    
    if (m_records.size() % 100 == 0) {
        qDebug() << "触发自动保存...";
        compressOldData();
        saveToFile();
    }
}

QList<ActivityRecord> ActivityManager::getRecordsByDateRange(const QDate& start, const QDate& end) const {
    QList<ActivityRecord> result;
    
    for (const ActivityRecord& record : m_records) {
        QDate recordDate = record.startTime.date();
        if (recordDate >= start && recordDate <= end) {
            result.append(record);
        }
    }
    
    return result;
}

QList<ActivityRecord> ActivityManager::getAllRecords() const {
    return m_records;
}

DailySummary ActivityManager::getDailySummary(const QDate& date) const {
    DailySummary summary;
    summary.date = date;
    
    for (const ActivityRecord& record : m_records) {
        if (record.startTime.date() == date) {
            summary.totalSeconds += record.durationSeconds;
            summary.categorySeconds[record.category] += record.durationSeconds;
            summary.appSeconds[record.processName] += record.durationSeconds;
        }
    }
    
    return summary;
}

QMap<QDate, DailySummary> ActivityManager::getMonthlySummary(int year, int month) const {
    QMap<QDate, DailySummary> summaries;
    
    QDate start(year, month, 1);
    QDate end = start.addMonths(1).addDays(-1);
    
    for (QDate date = start; date <= end; date = date.addDays(1)) {
        DailySummary summary = getDailySummary(date);
        if (summary.totalSeconds > 0) {
            summaries[date] = summary;
        }
    }
    
    return summaries;
}

void ActivityManager::setCategoryRule(const AppCategoryRule& rule) {
    m_categoryRules[rule.name] = rule;
    saveCategoryRules();
}

void ActivityManager::removeCategoryRule(const QString& name) {
    if (m_categoryRules.contains(name)) {
        m_categoryRules.remove(name);
        saveCategoryRules();
        qDebug() << "已删除分类规则:" << name;
    }
}

QList<AppCategoryRule> ActivityManager::getCategoryRules() const {
    return m_categoryRules.values();
}

ActivityCategory ActivityManager::categorizeActivity(const ActivityRecord& record) const {
    QString processLower = record.processName.toLower();
    QString titleLower = record.windowTitle.toLower();
    
    for (const AppCategoryRule& rule : m_categoryRules) {
        for (const QString& pattern : rule.processPatterns) {
            if (processLower.contains(pattern.toLower())) {
                return rule.category;
            }
        }
        for (const QString& pattern : rule.titlePatterns) {
            if (titleLower.contains(pattern.toLower())) {
                return rule.category;
            }
        }
    }
    
    return ActivityCategory::Other;
}

void ActivityManager::clearRecords(const QDate& date) {
    QList<ActivityRecord> newRecords;
    
    for (const ActivityRecord& record : m_records) {
        if (record.startTime.date() != date) {
            newRecords.append(record);
        }
    }
    
    m_records = newRecords;
    saveToFile();
    emit recordsCleared(date);
}

void ActivityManager::clearAllRecords() {
    m_records.clear();
    saveToFile();
    emit allRecordsCleared();
}

int ActivityManager::getTotalRecordsCount() const {
    return m_records.size();
}

qint64 ActivityManager::getTotalDurationSeconds(const QDate& date) const {
    qint64 total = 0;
    
    for (const ActivityRecord& record : m_records) {
        if (record.startTime.date() == date) {
            total += record.durationSeconds;
        }
    }
    
    return total;
}

void ActivityManager::loadFromFile() {
    QFile file(m_dataFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "活动数据文件不存在";
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
            m_records.append(ActivityRecord::fromJson(value.toObject()));
        }
    }
    
    qDebug() << "已加载" << m_records.size() << "条活动记录";
}

void ActivityManager::saveToFile() {
    QJsonArray array;
    for (const ActivityRecord& record : m_records) {
        array.append(record.toJson());
    }
    
    QFile file(m_dataFilePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "无法保存活动数据文件";
        return;
    }
    
    QJsonDocument doc(array);
    file.write(doc.toJson());
    file.close();
}

int ActivityManager::generateId() {
    static int id = 0;
    return ++id;
}

void ActivityManager::initDefaultCategoryRules() {
    AppCategoryRule devRule;
    devRule.name = "开发工具";
    devRule.category = ActivityCategory::Development;
    devRule.processPatterns = QStringList{
        "code", "qtcreator", "clion", "idea", "pycharm", "vscode", 
        "devenv", "android studio", "eclipse", "netbeans",
        "git", "svn", "mercurial", "cmake", "make", "mingw"
    };
    m_categoryRules[devRule.name] = devRule;
    
    AppCategoryRule browserRule;
    browserRule.name = "浏览器";
    browserRule.category = ActivityCategory::Browser;
    browserRule.processPatterns = QStringList{
        "chrome", "firefox", "edge", "msedge", "safari", 
        "opera", "browser", "360se", "qqbrowser", "sogouexplorer"
    };
    m_categoryRules[browserRule.name] = browserRule;
    
    AppCategoryRule officeRule;
    officeRule.name = "办公软件";
    officeRule.category = ActivityCategory::Office;
    officeRule.processPatterns = QStringList{
        "winword", "excel", "powerpnt", "outlook", "onenote",
        "wps", "et", "wpp", "notepad", "wordpad",
        "acrobat", "foxitreader", "sumatrapdf"
    };
    m_categoryRules[officeRule.name] = officeRule;
    
    AppCategoryRule commRule;
    commRule.name = "社交通讯";
    commRule.category = ActivityCategory::Communication;
    commRule.processPatterns = QStringList{
        "wechat", "qq", "tim", "dingtalk", "telegram",
        "discord", "slack", "teams", "skype", "zoom",
        "微信", "钉钉", "飞书"
    };
    m_categoryRules[commRule.name] = commRule;
    
    AppCategoryRule mediaRule;
    mediaRule.name = "多媒体";
    mediaRule.category = ActivityCategory::Media;
    mediaRule.processPatterns = QStringList{
        "vlc", "potplayer", "kmplayer", "foobar2000", "aimp",
        "netease", "cloudmusic", "qqmusic", "spotify",
        "photoshop", "lightroom", "premiere", "aftereffects"
    };
    m_categoryRules[mediaRule.name] = mediaRule;
    
    AppCategoryRule gameRule;
    gameRule.name = "游戏";
    gameRule.category = ActivityCategory::Game;
    gameRule.processPatterns = QStringList{
        "steam", "epicgames", "origin", "uplay", "battlenet",
        "leagueclient", "dota2", "csgo", "minecraft",
        "游戏", "game"
    };
    m_categoryRules[gameRule.name] = gameRule;
    
    AppCategoryRule systemRule;
    systemRule.name = "系统工具";
    systemRule.category = ActivityCategory::System;
    systemRule.processPatterns = QStringList{
        "explorer", "cmd", "powershell", "taskmgr", "regedit",
        "control", "mmc", "services", "eventvwr",
        "计算器", "记事本", "资源管理器"
    };
    m_categoryRules[systemRule.name] = systemRule;
}

void ActivityManager::compressOldData() {
    if (m_records.size() <= m_maxRecords) {
        return;
    }
    
    QDate threshold = QDate::currentDate().addMonths(-3);
    
    QMap<QDate, DailySummary> compressed;
    QList<ActivityRecord> newRecords;
    
    for (const ActivityRecord& record : m_records) {
        if (record.startTime.date() < threshold) {
            QDate date = record.startTime.date();
            DailySummary& summary = compressed[date];
            summary.date = date;
            summary.totalSeconds += record.durationSeconds;
            summary.categorySeconds[record.category] += record.durationSeconds;
            summary.appSeconds[record.processName] += record.durationSeconds;
        } else {
            newRecords.append(record);
        }
    }
    
    m_records = newRecords;
    qDebug() << "压缩旧数据，删除" << (m_records.size() - newRecords.size()) << "条记录";
}

void ActivityManager::loadCategoryRules() {
}

void ActivityManager::saveCategoryRules() {
}

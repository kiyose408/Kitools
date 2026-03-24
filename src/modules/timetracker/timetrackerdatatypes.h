#ifndef TIMETRACKERDATATYPES_H
#define TIMETRACKERDATATYPES_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>
#include <QMap>

enum class ActivityCategory {
    Development = 0,
    Browser = 1,
    Office = 2,
    Communication = 3,
    Media = 4,
    Game = 5,
    System = 6,
    Other = 7
};

enum class BrowserSubCategory {
    Work = 0,
    Learning = 1,
    Social = 2,
    Video = 3,
    Shopping = 4,
    News = 5,
    Other = 6
};
struct ActivityRecord {
    int id = 0;
    QString processName;
    QString windowTitle;
    QString filePath;
    ActivityCategory category = ActivityCategory::Other;
    BrowserSubCategory browserSubCategory = BrowserSubCategory::Other;
    QString browserDomain;
    QString browserPageTitle;
    QDateTime startTime;
    QDateTime endTime;
    int durationSeconds = 0;
    
    ActivityRecord() = default;
    
    QJsonObject toJson() const {
        QJsonObject obj;
        obj["id"] = id;
        obj["processName"] = processName;
        obj["windowTitle"] = windowTitle;
        obj["filePath"] = filePath;
        obj["category"] = static_cast<int>(category);
        obj["browserSubCategory"] = static_cast<int>(browserSubCategory);
        obj["browserDomain"] = browserDomain;
        obj["browserPageTitle"] = browserPageTitle;
        obj["startTime"] = startTime.toString(Qt::ISODate);
        obj["endTime"] = endTime.toString(Qt::ISODate);
        obj["durationSeconds"] = durationSeconds;
        return obj;
    }
    
    static ActivityRecord fromJson(const QJsonObject& obj) {
        ActivityRecord record;
        record.id = obj["id"].toInt();
        record.processName = obj["processName"].toString();
        record.windowTitle = obj["windowTitle"].toString();
        record.filePath = obj["filePath"].toString();
        record.category = static_cast<ActivityCategory>(obj["category"].toInt());
        record.browserSubCategory = static_cast<BrowserSubCategory>(obj["browserSubCategory"].toInt());
        record.browserDomain = obj["browserDomain"].toString();
        record.browserPageTitle = obj["browserPageTitle"].toString();
        record.startTime = QDateTime::fromString(obj["startTime"].toString(), Qt::ISODate);
        record.endTime = QDateTime::fromString(obj["endTime"].toString(), Qt::ISODate);
        record.durationSeconds = obj["durationSeconds"].toInt();
        return record;
    }
};

struct AppCategoryRule {
    QString name;
    ActivityCategory category;
    QStringList processPatterns;
    QStringList titlePatterns;
    
    QJsonObject toJson() const {
        QJsonObject obj;
        obj["name"] = name;
        obj["category"] = static_cast<int>(category);
        QJsonArray processArr;
        for (const QString& p : processPatterns) {
            processArr.append(p);
        }
        obj["processPatterns"] = processArr;
        QJsonArray titleArr;
        for (const QString& t : titlePatterns) {
            titleArr.append(t);
        }
        obj["titlePatterns"] = titleArr;
        return obj;
    }
    
    static AppCategoryRule fromJson(const QJsonObject& obj) {
        AppCategoryRule rule;
        rule.name = obj["name"].toString();
        rule.category = static_cast<ActivityCategory>(obj["category"].toInt());
        QJsonArray processArr = obj["processPatterns"].toArray();
        for (const QJsonValue& v : processArr) {
            rule.processPatterns.append(v.toString());
        }
        QJsonArray titleArr = obj["titlePatterns"].toArray();
        for (const QJsonValue& v : titleArr) {
            rule.titlePatterns.append(v.toString());
        }
        return rule;
    }
};
struct BrowserCategoryRule {
    QString name;
    BrowserSubCategory subCategory;
    QStringList domains;
    QStringList keywords;
    
    QJsonObject toJson() const {
        QJsonObject obj;
        obj["name"] = name;
        obj["subCategory"] = static_cast<int>(subCategory);
        QJsonArray domainsArr;
        for (const QString& d : domains) {
            domainsArr.append(d);
        }
        obj["domains"] = domainsArr;
        QJsonArray keywordsArr;
        for (const QString& k : keywords) {
            keywordsArr.append(k);
        }
        obj["keywords"] = keywordsArr;
        return obj;
    }
    
    static BrowserCategoryRule fromJson(const QJsonObject& obj) {
        BrowserCategoryRule rule;
        rule.name = obj["name"].toString();
        rule.subCategory = static_cast<BrowserSubCategory>(obj["subCategory"].toInt());
        QJsonArray domainsArr = obj["domains"].toArray();
        for (const QJsonValue& v : domainsArr) {
            rule.domains.append(v.toString());
        }
        QJsonArray keywordsArr = obj["keywords"].toArray();
        for (const QJsonValue& v : keywordsArr) {
            rule.keywords.append(v.toString());
        }
        return rule;
    }
};
struct DailySummary {
    QDate date;
    int totalSeconds = 0;
    QMap<ActivityCategory, int> categorySeconds;
    QMap<QString, int> appSeconds;
    QMap<BrowserSubCategory, int> browserSubSeconds;
    
    QJsonObject toJson() const {
        QJsonObject obj;
        obj["date"] = date.toString(Qt::ISODate);
        obj["totalSeconds"] = totalSeconds;
        QJsonObject catObj;
        for (auto it = categorySeconds.begin(); it != categorySeconds.end(); ++it) {
            catObj[QString::number(static_cast<int>(it.key()))] = it.value();
        }
        obj["categorySeconds"] = catObj;
        QJsonObject appObj;
        for (auto it = appSeconds.begin(); it != appSeconds.end(); ++it) {
            appObj[it.key()] = it.value();
        }
        obj["appSeconds"] = appObj;
        QJsonObject browserObj;
        for (auto it = browserSubSeconds.begin(); it != browserSubSeconds.end(); ++it) {
            browserObj[QString::number(static_cast<int>(it.key()))] = it.value();
        }
        obj["browserSubSeconds"] = browserObj;
        return obj;
    }
    
    static DailySummary fromJson(const QJsonObject& obj) {
        DailySummary summary;
        summary.date = QDate::fromString(obj["date"].toString(), Qt::ISODate);
        summary.totalSeconds = obj["totalSeconds"].toInt();
        QJsonObject catObj = obj["categorySeconds"].toObject();
        for (auto it = catObj.begin(); it != catObj.end(); ++it) {
            ActivityCategory cat = static_cast<ActivityCategory>(it.key().toInt());
            summary.categorySeconds[cat] = it.value().toInt();
        }
        QJsonObject appObj = obj["appSeconds"].toObject();
        for (auto it = appObj.begin(); it != appObj.end(); ++it) {
            summary.appSeconds[it.key()] = it.value().toInt();
        }
        QJsonObject browserObj = obj["browserSubSeconds"].toObject();
        for (auto it = browserObj.begin(); it != browserObj.end(); ++it) {
            BrowserSubCategory subCat = static_cast<BrowserSubCategory>(it.key().toInt());
            summary.browserSubSeconds[subCat] = it.value().toInt();
        }
        return summary;
    }
};

inline QString categoryToString(ActivityCategory category) {
    switch (category) {
        case ActivityCategory::Development: return "开发工具";
        case ActivityCategory::Browser: return "浏览器";
        case ActivityCategory::Office: return "办公软件";
        case ActivityCategory::Communication: return "社交通讯";
        case ActivityCategory::Media: return "多媒体";
        case ActivityCategory::Game: return "游戏";
        case ActivityCategory::System: return "系统工具";
        case ActivityCategory::Other: return "其他";
        default: return "未知";
    }
}

inline QString browserSubCategoryToString(BrowserSubCategory subCategory) {
    switch (subCategory) {
        case BrowserSubCategory::Work: return "工作相关";
        case BrowserSubCategory::Learning: return "学习相关";
        case BrowserSubCategory::Social: return "社交媒体";
        case BrowserSubCategory::Video: return "视频娱乐";
        case BrowserSubCategory::Shopping: return "购物";
        case BrowserSubCategory::News: return "新闻资讯";
        case BrowserSubCategory::Other: return "其他";
        default: return "未知";
    }
}
#endif

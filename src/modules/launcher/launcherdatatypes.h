#ifndef LAUNCHERDATATYPES_H
#define LAUNCHERDATATYPES_H

#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>
#include <QIcon>

enum class LauncherItemType {
    Application = 0,
    File = 1,
    Folder = 2,
    Url = 3,
    Command = 4
};

struct LauncherItem {
    int id = 0;
    QString name;
    QString path;
    QString arguments;
    QString workingDir;
    LauncherItemType type = LauncherItemType::Application;
    QString iconPath;
    QStringList keywords;
    int launchCount = 0;
    QDateTime lastLaunched;
    QDateTime addedTime;
    bool isPinned = false;

    LauncherItem() = default;

    QJsonObject toJson() const {
        QJsonObject obj;
        obj["id"] = id;
        obj["name"] = name;
        obj["path"] = path;
        obj["arguments"] = arguments;
        obj["workingDir"] = workingDir;
        obj["type"] = static_cast<int>(type);
        obj["iconPath"] = iconPath;
        QJsonArray keywordsArray;
        for (const QString& kw : keywords) {
            keywordsArray.append(kw);
        }
        obj["keywords"] = keywordsArray;
        obj["launchCount"] = launchCount;
        obj["lastLaunched"] = lastLaunched.toString(Qt::ISODate);
        obj["addedTime"] = addedTime.toString(Qt::ISODate);
        obj["isPinned"] = isPinned;
        return obj;
    }

    static LauncherItem fromJson(const QJsonObject& obj) {
        LauncherItem item;
        item.id = obj["id"].toInt();
        item.name = obj["name"].toString();
        item.path = obj["path"].toString();
        item.arguments = obj["arguments"].toString();
        item.workingDir = obj["workingDir"].toString();
        item.type = static_cast<LauncherItemType>(obj["type"].toInt());
        item.iconPath = obj["iconPath"].toString();
        QJsonArray keywordsArray = obj["keywords"].toArray();
        for (const QJsonValue& v : keywordsArray) {
            item.keywords.append(v.toString());
        }
        item.launchCount = obj["launchCount"].toInt();
        item.lastLaunched = QDateTime::fromString(obj["lastLaunched"].toString(), Qt::ISODate);
        item.addedTime = QDateTime::fromString(obj["addedTime"].toString(), Qt::ISODate);
        item.isPinned = obj["isPinned"].toBool();
        return item;
    }
};

#endif

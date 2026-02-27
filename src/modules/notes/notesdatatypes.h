#ifndef NOTESTDATATYPES_H
#define NOTESTDATATYPES_H

#include <QString>
#include <QDateTime>
#include <QColor>
#include <QJsonObject>

enum class NoteColor {
    Yellow = 0,
    Green = 1,
    Blue = 2,
    Pink = 3,
    Purple = 4
};

struct NoteData {
    int id;
    QString title;
    QString content;
    NoteColor color;
    QDateTime creationTime;
    QDateTime modificationTime;
    QDateTime reminderTime;
    bool hasReminder;
    
    NoteData()
        : id(0)
        , color(NoteColor::Yellow)
        , hasReminder(false)
    {}
    
    QJsonObject toJson() const {
        QJsonObject obj;
        obj["id"] = id;
        obj["title"] = title;
        obj["content"] = content;
        obj["color"] = static_cast<int>(color);
        obj["creationTime"] = creationTime.toString(Qt::ISODate);
        obj["modificationTime"] = modificationTime.toString(Qt::ISODate);
        obj["hasReminder"] = hasReminder;
        if (hasReminder) {
            obj["reminderTime"] = reminderTime.toString(Qt::ISODate);
        }
        return obj;
    }
    
    static NoteData fromJson(const QJsonObject &obj) {
        NoteData note;
        note.id = obj["id"].toInt();
        note.title = obj["title"].toString();
        note.content = obj["content"].toString();
        note.color = static_cast<NoteColor>(obj["color"].toInt());
        note.creationTime = QDateTime::fromString(obj["creationTime"].toString(), Qt::ISODate);
        note.modificationTime = QDateTime::fromString(obj["modificationTime"].toString(), Qt::ISODate);
        note.hasReminder = obj["hasReminder"].toBool();
        if (note.hasReminder) {
            note.reminderTime = QDateTime::fromString(obj["reminderTime"].toString(), Qt::ISODate);
        }
        return note;
    }
    
    QColor toQColor() const {
        switch (color) {
            case NoteColor::Yellow: return QColor(255, 245, 157);
            case NoteColor::Green: return QColor(200, 230, 201);
            case NoteColor::Blue: return QColor(187, 222, 251);
            case NoteColor::Pink: return QColor(248, 187, 208);
            case NoteColor::Purple: return QColor(225, 190, 231);
            default: return QColor(255, 245, 157);
        }
    }
    
    static QString colorToString(NoteColor c) {
        switch (c) {
            case NoteColor::Yellow: return QString::fromUtf8("黄色");
            case NoteColor::Green: return QString::fromUtf8("绿色");
            case NoteColor::Blue: return QString::fromUtf8("蓝色");
            case NoteColor::Pink: return QString::fromUtf8("粉色");
            case NoteColor::Purple: return QString::fromUtf8("紫色");
            default: return QString::fromUtf8("黄色");
        }
    }
};

#endif

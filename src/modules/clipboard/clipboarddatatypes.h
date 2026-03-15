#ifndef CLIPBOARDDATATYPES_H
#define CLIPBOARDDATATYPES_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>
#include <QIcon>

enum class ClipboardType {
    Text = 0,
    Image = 1,
    File = 2,
    Html = 3
};

struct ClipboardItem {
    int id;
    ClipboardType type;
    QString content;
    QString preview;
    QDateTime timestamp;
    bool isFavorite;
    QString sourceApp;

    QJsonObject toJson() const;
    static ClipboardItem fromJson(const QJsonObject& obj);

    QString typeToString() const;
    static QString typeToString(ClipboardType type);
};

#endif

#include "clipboarddatatypes.h"

QJsonObject ClipboardItem::toJson() const {
    QJsonObject obj;
    obj["id"] = id;
    obj["type"] = static_cast<int>(type);
    obj["content"] = content;
    obj["preview"] = preview;
    obj["timestamp"] = timestamp.toString(Qt::ISODate);
    obj["isFavorite"] = isFavorite;
    obj["sourceApp"] = sourceApp;
    return obj;
}

ClipboardItem ClipboardItem::fromJson(const QJsonObject& obj) {
    ClipboardItem item;
    item.id = obj["id"].toInt();
    item.type = static_cast<ClipboardType>(obj["type"].toInt());
    item.content = obj["content"].toString();
    item.preview = obj["preview"].toString();
    item.timestamp = QDateTime::fromString(obj["timestamp"].toString(), Qt::ISODate);
    item.isFavorite = obj["isFavorite"].toBool();
    item.sourceApp = obj["sourceApp"].toString();
    return item;
}

QString ClipboardItem::typeToString() const {
    return typeToString(type);
}

QString ClipboardItem::typeToString(ClipboardType type) {
    switch (type) {
        case ClipboardType::Text: return "Text";
        case ClipboardType::Image: return "Image";
        case ClipboardType::File: return "File";
        case ClipboardType::Html: return "HTML";
        default: return "Unknown";
    }
}

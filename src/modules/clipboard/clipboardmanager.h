#ifndef CLIPBOARDMANAGER_H
#define CLIPBOARDMANAGER_H

#include <QObject>
#include <QClipboard>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QDir>
#include <QApplication>
#include <QMimeData>
#include <QImage>
#include <QUrl>
#include <QDebug>

#include "clipboarddatatypes.h"

class ClipboardManager : public QObject {
    Q_OBJECT

public:
    static ClipboardManager* instance(QObject* parent = nullptr);

    void startMonitoring();
    void stopMonitoring();

    QList<ClipboardItem> getAllItems() const;
    QList<ClipboardItem> getItemsByType(ClipboardType type) const;
    QList<ClipboardItem> getFavorites() const;
    QList<ClipboardItem> search(const QString& keyword) const;

    void addToFavorites(int id);
    void removeFromFavorites(int id);
    void deleteItem(int id);
    void clearAll();
    void copyToClipboard(int id);
    ClipboardItem getItemById(int id) const;

    int getTotalCount() const;
    int getCountByType(ClipboardType type) const;

signals:
    void itemAdded(const ClipboardItem& item);
    void itemDeleted(int id);
    void itemUpdated(const ClipboardItem& item);
    void favoritesChanged();
    void countChanged();

private slots:
    void onClipboardChanged();

private:
    explicit ClipboardManager(QObject* parent = nullptr);
    ~ClipboardManager();

    void loadFromFile();
    void saveToFile();
    void processClipboardContent();
    int generateId();
    QString generatePreview(const QString& content, int maxLength = 100);
    bool isDuplicate(const QString& content) const;

    static ClipboardManager* m_instance;

    QList<ClipboardItem> m_items;
    QString m_lastContent;
    bool m_isMonitoring;
    int m_maxItems;
    QString m_dataFilePath;
};

#endif

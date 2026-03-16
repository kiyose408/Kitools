#ifndef LAUNCHERINDEXER_H
#define LAUNCHERINDEXER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>
#include <QFileSystemWatcher>

#include "launcherdatatypes.h"

class LauncherIndexer : public QObject {
    Q_OBJECT

public:
    static LauncherIndexer* instance(QObject* parent = nullptr);

    void startIndexing();
    void refreshIndex();

    QList<LauncherItem> getAllItems() const;
    QList<LauncherItem> search(const QString& query, int maxResults = 20) const;
    QList<LauncherItem> getPinnedItems() const;
    LauncherItem getItemById(int id) const;

    void launchItem(int id);
    void incrementLaunchCount(int id);

    void addCustomItem(const LauncherItem& item);
    void removeItem(int id);
    void pinItem(int id, bool pinned);

    int getTotalCount() const;

signals:
    void indexingStarted();
    void indexingProgress(int current, int total);
    void indexingFinished(int totalCount);
    void itemLaunched(const LauncherItem& item);
    void itemsChanged();

private slots:
    void onDirectoryChanged(const QString& path);

private:
    explicit LauncherIndexer(QObject* parent = nullptr);
    ~LauncherIndexer();

    void scanStartMenu();
    void scanDesktop();
    void scanCommonPaths();
    void scanDirectory(const QString& directory, LauncherItemType type, int depth = 0);
    void parseLnkFile(const QString& lnkPath);
    void parseExeFile(const QString& exePath);
    void parseUrlFile(const QString& urlPath);
    void parseMsiFile(const QString& msiPath);
    void parseScriptFile(const QString& scriptPath);

    void loadFromFile();
    void saveToFile();
    int generateId();
    bool isDuplicate(const QString& path) const;

    static LauncherIndexer* m_instance;

    QList<LauncherItem> m_items;
    QFileSystemWatcher* m_watcher;
    QString m_dataFilePath;
    QStringList m_scanPaths;
};

#endif

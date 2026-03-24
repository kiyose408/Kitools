#ifndef ACTIVITYMANAGER_H
#define ACTIVITYMANAGER_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QDate>

#include "timetrackerdatatypes.h"

class ActivityManager : public QObject {
    Q_OBJECT

public:
    static ActivityManager* instance(QObject* parent = nullptr);

    void addActivityRecord(const ActivityRecord& record);
    QList<ActivityRecord> getRecordsByDateRange(const QDate& start, const QDate& end) const;
    QList<ActivityRecord> getAllRecords() const;
    
    DailySummary getDailySummary(const QDate& date) const;
    QMap<QDate, DailySummary> getMonthlySummary(int year, int month) const;
    
    void setCategoryRule(const AppCategoryRule& rule);
    void removeCategoryRule(const QString& name);
    QList<AppCategoryRule> getCategoryRules() const;
    
    void setBrowserCategoryRule(const BrowserCategoryRule& rule);
    void removeBrowserCategoryRule(const QString& name);
    QList<BrowserCategoryRule> getBrowserCategoryRules() const;
    
    ActivityCategory categorizeActivity(const ActivityRecord& record) const;
    BrowserSubCategory categorizeBrowser(const QString& windowTitle) const;
    void parseBrowserTitle(const QString& windowTitle, QString& domain, QString& pageTitle) const;
    
    void clearRecords(const QDate& date);
    void clearAllRecords();

    int getTotalRecordsCount() const;
    qint64 getTotalDurationSeconds(const QDate& date) const;

signals:
    void recordAdded(const ActivityRecord& record);
    void recordsCleared(const QDate& date);
    void allRecordsCleared();

private:
    explicit ActivityManager(QObject* parent = nullptr);
    ~ActivityManager();

    void loadFromFile();
    void saveToFile();
    int generateId();
    void initDefaultCategoryRules();
    void initDefaultBrowserRules();
    void compressOldData();
    void saveCategoryRules();
    void loadCategoryRules();

    static ActivityManager* m_instance;
    
    QList<ActivityRecord> m_records;
    QMap<QString, AppCategoryRule> m_categoryRules;
    QMap<QString, BrowserCategoryRule> m_browserRules;
    QString m_dataFilePath;
    int m_maxRecords;
};

#endif

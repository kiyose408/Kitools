#include "activitymanager.h"
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QCoreApplication>
#include <QDir>
#include <QRegularExpression>

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
    
    initDefaultBrowserRules();
    qDebug() << "已初始化" << m_browserRules.size() << "条默认浏览器分类规则";
    
    loadCategoryRules();
    
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
    
    if (newRecord.category == ActivityCategory::Browser) {
        newRecord.browserSubCategory = categorizeBrowser(newRecord.windowTitle);
        parseBrowserTitle(newRecord.windowTitle, newRecord.browserDomain, newRecord.browserPageTitle);
        qDebug() << "浏览器子分类:" << browserSubCategoryToString(newRecord.browserSubCategory);
        qDebug() << "域名:" << newRecord.browserDomain << "页面:" << newRecord.browserPageTitle;
    }
    
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
            
            if (record.category == ActivityCategory::Browser) {
                summary.browserSubSeconds[record.browserSubCategory] += record.durationSeconds;
            }
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
        "git", "svn", "mercurial", "cmake", "make", "mingw","trae"
    };
    m_categoryRules[devRule.name] = devRule;
    
    AppCategoryRule browserRule;
    browserRule.name = "浏览器";
    browserRule.category = ActivityCategory::Browser;
    browserRule.processPatterns = QStringList{
        "chrome", "firefox", "edge", "msedge", "safari", 
        "opera", "browser", "360se", "qqbrowser", "sogouexplorer", "quark"
    };
    m_categoryRules[browserRule.name] = browserRule;
    
    AppCategoryRule officeRule;
    officeRule.name = "办公软件";
    officeRule.category = ActivityCategory::Office;
    officeRule.processPatterns = QStringList{
        "winword", "excel", "powerpnt", "outlook", "onenote",
        "wps", "et", "wpp", "notepad", "wordpad",
        "acrobat", "foxitreader", "sumatrapdf", "obsidian"
    };
    m_categoryRules[officeRule.name] = officeRule;
    
    AppCategoryRule commRule;
    commRule.name = "社交通讯";
    commRule.category = ActivityCategory::Communication;
    commRule.processPatterns = QStringList{
        "wechat", "qq", "tim", "dingtalk", "telegram",
        "discord", "slack", "teams", "skype", "zoom",
        "微信", "钉钉", "飞书","kook"
    };
    m_categoryRules[commRule.name] = commRule;
    
    AppCategoryRule mediaRule;
    mediaRule.name = "多媒体";
    mediaRule.category = ActivityCategory::Media;
    mediaRule.processPatterns = QStringList{
        "vlc", "potplayer", "kmplayer", "foobar2000", "aimp",
        "netease", "cloudmusic", "qqmusic", "spotify",
        "photoshop", "lightroom", "premiere", "aftereffects", "obs"
    };
    m_categoryRules[mediaRule.name] = mediaRule;
    
    AppCategoryRule gameRule;
    gameRule.name = "游戏";
    gameRule.category = ActivityCategory::Game;
    gameRule.processPatterns = QStringList{
        "steam", "epicgames", "origin", "uplay", "battlenet", "wegame",
        "leagueclient", "dota2", "csgo", "minecraft", "valorant",
        "delta", "deltaforce", "三角洲",
        "genshin", "yuanshen", "原神",
        "naraka", "永劫无间",
        "pubg", "tslgame",
        "apex", "r5apex",
        "overwatch", "守望先锋",
        "wow", "warcraft", "魔兽",
        "lol", "leagueoflegends",
        "crossfire", "穿越火线",
        "dnf", "地下城",
        "cf", "csol",
        "游戏", "game",
        "洛克王国"
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
    
    initDefaultBrowserRules();
}

void ActivityManager::initDefaultBrowserRules() {
    qDebug() << "初始化默认浏览器分类规则";
    
    BrowserCategoryRule workRule;
    workRule.name = "工作相关";
    workRule.subCategory = BrowserSubCategory::Work;
    workRule.domains = QStringList{
        "github.com", "gitlab.com", "bitbucket.org",
        "stackoverflow.com", "stackexchange.com",
        "docs.microsoft.com", "developer.mozilla.org",
        "npmjs.com", "pypi.org",
        "jira.", "confluence.",
        "notion.so", "figma.com",
        "office.com", "docs.google.com",
        "gitee.com"
    };
    workRule.keywords = QStringList{"文档", "API", "开发", "代码", "编程"};
    m_browserRules[workRule.name] = workRule;
    
    BrowserCategoryRule learningRule;
    learningRule.name = "学习相关";
    learningRule.subCategory = BrowserSubCategory::Learning;
    learningRule.domains = QStringList{
        "coursera.org", "udemy.com", "edx.org",
        "khanacademy.org", "codecademy.com",
        "leetcode.com", "luogu.com",
        "mooc.", "xuetangx.com",
        "icourse163.org", "bilibili.com/video"
    };
    learningRule.keywords = QStringList{"教程", "学习", "课程", "培训", "教育"};
    m_browserRules[learningRule.name] = learningRule;
    
    BrowserCategoryRule socialRule;
    socialRule.name = "社交媒体";
    socialRule.subCategory = BrowserSubCategory::Social;
    socialRule.domains = QStringList{
        "weibo.com", "twitter.com", "x.com",
        "facebook.com", "instagram.com",
        "linkedin.com", "zhihu.com",
        "tieba.baidu.com", "douban.com",
        "reddit.com"
    };
    socialRule.keywords = QStringList{"微博", "朋友圈", "动态"};
    m_browserRules[socialRule.name] = socialRule;
    
    BrowserCategoryRule videoRule;
    videoRule.name = "视频娱乐";
    videoRule.subCategory = BrowserSubCategory::Video;
    videoRule.domains = QStringList{
        "youtube.com", "youtu.be",
        "bilibili.com", "b23.tv",
        "youku.com", "iqiyi.com",
        "v.qq.com", "tv.sohu.com",
        "netflix.com", "disneyplus.com", "douyin.com"
    };
    videoRule.keywords = QStringList{"视频", "电影", "电视剧", "动漫", "综艺", "直播"};
    m_browserRules[videoRule.name] = videoRule;
    
    BrowserCategoryRule shoppingRule;
    shoppingRule.name = "购物";
    shoppingRule.subCategory = BrowserSubCategory::Shopping;
    shoppingRule.domains = QStringList{
        "taobao.com", "tmall.com", "jd.com",
        "amazon.com", "amazon.cn",
        "pinduoduo.com", "suning.com",
        "1688.com", "aliexpress.com"
    };
    shoppingRule.keywords = QStringList{"购物", "商城", "旗舰店"};
    m_browserRules[shoppingRule.name] = shoppingRule;
    
    BrowserCategoryRule newsRule;
    newsRule.name = "新闻资讯";
    newsRule.subCategory = BrowserSubCategory::News;
    newsRule.domains = QStringList{
        "news.", "sina.com.cn", "sohu.com",
        "163.com", "qq.com/news",
        "thepaper.cn", "guancha.cn",
        "cctv.com", "xinhuanet.com"
    };
    newsRule.keywords = QStringList{"新闻", "资讯", "头条", "热点"};
    m_browserRules[newsRule.name] = newsRule;
    
    qDebug() << "已初始化" << m_browserRules.size() << "条浏览器分类规则";
}

BrowserSubCategory ActivityManager::categorizeBrowser(const QString& windowTitle) const
{
    QString titleLower = windowTitle.toLower();
    
    qDebug() << "=== 浏览器分类调试 ===";
    qDebug() << "窗口标题:" << windowTitle;
    
    for (const BrowserCategoryRule& rule : m_browserRules) {
        for (const QString& domain : rule.domains) {
            QString domainLower = domain.toLower();
            if (titleLower.contains(domainLower)) {
                qDebug() << "✓ 匹配域名:" << domain << "-> 分类:" << rule.name;
                return rule.subCategory;
            }
        }
        
        for (const QString& keyword : rule.keywords) {
            if (titleLower.contains(keyword.toLower())) {
                qDebug() << "✓ 匹配关键词:" << keyword << "-> 分类:" << rule.name;
                return rule.subCategory;
            }
        }
    }
    
    if (windowTitle.contains("哔哩哔哩") || windowTitle.contains("B站") || 
        windowTitle.contains("bilibili", Qt::CaseInsensitive) || 
        titleLower.contains("b23.tv")) {
        qDebug() << "✓ 特殊匹配: B站相关 -> 分类: 视频娱乐";
        return BrowserSubCategory::Video;
    }
    
    if (windowTitle.contains("抖音") || titleLower.contains("douyin") || 
        titleLower.contains("tiktok")) {
        qDebug() << "✓ 特殊匹配: 抖音相关 -> 分类: 视频娱乐";
        return BrowserSubCategory::Video;
    }
    
    qDebug() << "✗ 未匹配到任何规则 -> 分类: 其他";
    return BrowserSubCategory::Other;
}

void ActivityManager::parseBrowserTitle(const QString& windowTitle, QString& domain, QString& pageTitle) const {
    QStringList separators = {" - ", " — ", " – ", " | "};
    
    QString remaining = windowTitle;
    QString browserSuffix;
    
    QStringList browserNames = {"Google Chrome", "Mozilla Firefox", "Microsoft Edge", "Opera", "Safari", "360浏览器", "QQ浏览器", "搜狗浏览器", "quark", "夸克"};
    for (const QString& browser : browserNames) {
        if (remaining.endsWith(browser)) {
            browserSuffix = browser;
            remaining.chop(browser.length());
            break;
        }
    }
    
    for (const QString& sep : separators) {
        int idx = remaining.lastIndexOf(sep);
        if (idx > 0) {
            pageTitle = remaining.left(idx).trimmed();
            domain = remaining.mid(idx + sep.length()).trimmed();
            break;
        }
    }
    
    if (pageTitle.isEmpty()) {
        pageTitle = remaining.trimmed();
    }
    
    QRegularExpression domainRegex(R"(([a-zA-Z0-9][-a-zA-Z0-9]*\.)+[a-zA-Z]{2,})");
    QRegularExpressionMatch match = domainRegex.match(windowTitle);
    if (match.hasMatch()) {
        domain = match.captured(0);
    }
    
    qDebug() << "解析浏览器标题:" << windowTitle << "-> 域名:" << domain << "页面:" << pageTitle;
}

void ActivityManager::setBrowserCategoryRule(const BrowserCategoryRule& rule) {
    m_browserRules[rule.name] = rule;
}

void ActivityManager::removeBrowserCategoryRule(const QString& name) {
    if (m_browserRules.contains(name)) {
        m_browserRules.remove(name);
        qDebug() << "已删除浏览器分类规则:" << name;
    }
}

QList<BrowserCategoryRule> ActivityManager::getBrowserCategoryRules() const {
    return m_browserRules.values();
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

void ActivityManager::saveCategoryRules() {
    qDebug() << "保存分类规则到文件...";

    QJsonObject root;

    QJsonObject appRulesObj;
    for (auto it = m_categoryRules.begin(); it != m_categoryRules.end(); ++it) {
        appRulesObj[it.key()] = it.value().toJson();
    }
    root["appRules"] = appRulesObj;

    QJsonObject browserRulesObj;
    for (auto it = m_browserRules.begin(); it != m_browserRules.end(); ++it) {
        browserRulesObj[it.key()] = it.value().toJson();
    }
    root["browserRules"] = browserRulesObj;

    QJsonDocument doc(root);
    QFile file(m_dataFilePath);
    if (file.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
        file.write(doc.toJson());
        file.close();
        qDebug() << "分类规则已保存";
    } else {
        qWarning() << "无法保存分类规则";
    }
}

void ActivityManager::loadCategoryRules() {
    qDebug() << "从文件加载分类规则...";

    QFile file(m_dataFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "分类规则文件不存在，使用默认规则";
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        return;
    }

    QJsonObject root = doc.object();

    QJsonObject appRulesObj = root["appRules"].toObject();
    for (auto it = appRulesObj.begin(); it != appRulesObj.end(); ++it) {
        m_categoryRules[it.key()] = AppCategoryRule::fromJson(it.value().toObject());
    }

    QJsonObject browserRulesObj = root["browserRules"].toObject();
    for (auto it = browserRulesObj.begin(); it != browserRulesObj.end(); ++it) {
        m_browserRules[it.key()] = BrowserCategoryRule::fromJson(it.value().toObject());
    }

    qDebug() << "已加载" << m_categoryRules.size() << "条应用分类规则";
    qDebug() << "已加载" << m_browserRules.size() << "条浏览器分类规则";
}

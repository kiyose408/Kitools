#ifndef LAUNCHERCONTROLLER_H
#define LAUNCHERCONTROLLER_H

#include <QObject>
#include <QWidget>

#include "launcherindexer.h"
#include "launchersettingspanel.h"

class LauncherController : public QObject {
    Q_OBJECT

public:
    explicit LauncherController(QObject* parent = nullptr);
    ~LauncherController();

    QWidget* getSettingsPanel();
    void startIndexing();
    void refreshIndex();

signals:
    void activated();
    void deactivated();

private:
    LauncherIndexer* m_indexer;
    LauncherSettingsPanel* m_settingsPanel;
};

#endif

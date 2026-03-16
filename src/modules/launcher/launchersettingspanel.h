#ifndef LAUNCHERSETTINGSPANEL_H
#define LAUNCHERSETTINGSPANEL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QLineEdit>
#include <QListWidget>
#include <QComboBox>
#include <QTabWidget>

#include "launcherindexer.h"

class LauncherSettingsPanel : public QWidget {
    Q_OBJECT

public:
    explicit LauncherSettingsPanel(QWidget* parent = nullptr);
    ~LauncherSettingsPanel();

signals:
    void backRequested();
    void refreshRequested();

private slots:
    void onRefreshClicked();
    void onSearchChanged(const QString& text);
    void onCategoryChanged(int index);
    void onItemDoubleClicked(QListWidgetItem* item);
    void onIndexingFinished(int totalCount);

private:
    void setupUI();
    void loadItems();
    void updateStats();
    QString getCurrentCategory() const;

    LauncherIndexer* m_indexer;
    QLineEdit* m_searchEdit;
    QComboBox* m_categoryCombo;
    QListWidget* m_itemsList;
    QLabel* m_statsLabel;
    QPushButton* m_refreshBtn;
    QString m_currentCategory;
};

#endif

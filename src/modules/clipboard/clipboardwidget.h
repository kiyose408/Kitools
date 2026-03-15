#ifndef CLIPBOARDWIDGET_H
#define CLIPBOARDWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QScrollArea>

#include "clipboardmanager.h"
#include "clipboarditemwidget.h"
#include "clipboarddatatypes.h"

class ClipboardWidget : public QWidget {
    Q_OBJECT

public:
    explicit ClipboardWidget(QWidget* parent = nullptr);
    ~ClipboardWidget();

    void refresh();

signals:
    void closeRequested();

private slots:
    void onSearchChanged(const QString& text);
    void onFilterChanged(int index);
    void onItemCopy(int id);
    void onItemFavorite(int id);
    void onItemDelete(int id);
    void onItemViewDetails(int id);
    void onItemAdded(const ClipboardItem& item);
    void onItemDeleted(int id);
    void onClearAll();
    void onRefresh();

private:
    void setupUI();
    void loadItems();
    void applyFilter();

    ClipboardManager* m_manager;
    QLineEdit* m_searchEdit;
    QComboBox* m_filterCombo;
    QPushButton* m_clearBtn;
    QPushButton* m_refreshBtn;
    QPushButton* m_closeBtn;
    QVBoxLayout* m_listLayout;
    QWidget* m_listContainer;

    QString m_currentSearch;
    int m_currentFilter;
};

#endif

#ifndef CLIPBOARDITEMWIDGET_H
#define CLIPBOARDITEMWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMenu>
#include <QAction>

#include "clipboarddatatypes.h"

class ClipboardItemWidget : public QWidget {
    Q_OBJECT

public:
    explicit ClipboardItemWidget(const ClipboardItem& item, QWidget* parent = nullptr);
    void updateItem(const ClipboardItem& item);

    int getItemId() const { return m_item.id; }

signals:
    void copyClicked(int id);
    void favoriteClicked(int id);
    void deleteClicked(int id);
    void viewDetails(int id);

protected:
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private slots:
    void onCopyClicked();
    void onFavoriteClicked();
    void onDeleteClicked();
    void showContextMenu(const QPoint& pos);

private:
    void setupUI();
    void updateUI();

    ClipboardItem m_item;
    QLabel* m_typeLabel;
    QLabel* m_previewLabel;
    QLabel* m_timeLabel;
    QLabel* m_favoriteLabel;
};

#endif

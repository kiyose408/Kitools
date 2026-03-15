#ifndef CLIPBOARDSETTINGSPANEL_H
#define CLIPBOARDSETTINGSPANEL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QSpinBox>
#include <QCheckBox>

#include "clipboardmanager.h"

class ClipboardSettingsPanel : public QWidget {
    Q_OBJECT

public:
    explicit ClipboardSettingsPanel(QWidget* parent = nullptr);
    ~ClipboardSettingsPanel();

signals:
    void openClipboardManager();
    void backRequested();

private slots:
    void onOpenClicked();
    void onClearHistory();
    void onMaxItemsChanged(int value);
    void onAutoStartChanged(bool checked);

private:
    void setupUI();

    ClipboardManager* m_manager;
    QSpinBox* m_maxItemsSpin;
    QCheckBox* m_autoStartCheck;
};

#endif

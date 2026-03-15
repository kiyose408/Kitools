#ifndef CLIPBOARDCONTROLLER_H
#define CLIPBOARDCONTROLLER_H

#include <QObject>
#include <QWidget>

#include "clipboardwidget.h"
#include "clipboardsettingspanel.h"

class ClipboardController : public QObject {
    Q_OBJECT

public:
    explicit ClipboardController(QObject* parent = nullptr);
    ~ClipboardController();

    QWidget* getSettingsPanel();
    void showManager();
    void hideManager();
    bool isManagerVisible() const;

signals:
    void activated();
    void deactivated();

private slots:
    void onManagerCloseRequested();

private:
    ClipboardWidget* m_managerWidget;
    ClipboardSettingsPanel* m_settingsPanel;
};

#endif

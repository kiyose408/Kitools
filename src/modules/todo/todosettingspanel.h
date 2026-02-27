#ifndef TODOSETTINGSPANEL_H
#define TODOSETTINGSPANEL_H

#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QColor>

class DesktopTodoWidget;

class TodoSettingsPanel : public QWidget
{
    Q_OBJECT

public:
    explicit TodoSettingsPanel(QWidget *parent = nullptr);
    ~TodoSettingsPanel();
    
    void setTodoWidget(DesktopTodoWidget *widget);
    
signals:
    void backClicked();
    void showTodoRequested();
    void hideTodoRequested();

private slots:
    void onShowTodoClicked();
    void onHideTodoClicked();
    void onModeChanged(int index);
    void onClearCompletedClicked();
    void onExportJsonClicked();
    void onExportCsvClicked();
    void onClearLogsClicked();
    void onBackClicked();
    void onBackgroundColorClicked();
    void onDarkModeToggled(bool checked);
    void onFontChanged(int index);

private:
    void setupUi();
    void setupConnections();
    void updateStats();

    DesktopTodoWidget *m_todoWidget;
    
    QPushButton *m_showBtn;
    QPushButton *m_hideBtn;
    QComboBox *m_modeCombo;
    QPushButton *m_clearCompletedBtn;
    QPushButton *m_exportJsonBtn;
    QPushButton *m_exportCsvBtn;
    QPushButton *m_clearLogsBtn;
    QPushButton *m_backBtn;
    QLabel *m_statsLabel;
    QLabel *m_logCountLabel;
    
    QPushButton *m_bgColorBtn;
    QPushButton *m_darkModeBtn;
    QComboBox *m_fontCombo;
    
    QColor m_backgroundColor;
    bool m_isDarkMode;
};

#endif

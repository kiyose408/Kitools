#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QCloseEvent>
#include <QCheckBox>

class TimerController;
class TodoController;
class NotesController;
class ClipboardController;
class LauncherController;
class TimeTrackerController;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;
    void changeEvent(QEvent *event) override;

private slots:
    void onTimerModuleClicked();
    void onTodoModuleClicked();
    void onNotesModuleClicked();
    void onClipboardModuleClicked();
    void onLauncherModuleClicked();
    void onTimeTrackerModuleClicked();
    void onBackToHome();
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void onShowWindow();
    void onQuitApplication();
    void onAutoStartChanged(int state);

private:
    void setupUi();
    void setupConnections();
    void setupTrayIcon();
    void createTrayMenu();
    void initializeAutoStartModules();

    QWidget *m_homeWidget;
    QStackedWidget *m_stackedWidget;
    QPushButton *m_timerModuleBtn;
    QPushButton *m_todoModuleBtn;
    QPushButton *m_notesModuleBtn;
    QPushButton *m_clipboardModuleBtn;
    QPushButton *m_launcherModuleBtn;
    QPushButton *m_timeTrackerModuleBtn;
    
    TimerController *m_timerController;
    QWidget *m_timerPanel;
    
    TodoController *m_todoController;
    QWidget *m_todoPanel;
    
    NotesController *m_notesController;
    QWidget *m_notesPanel;

    ClipboardController *m_clipboardController;
    QWidget *m_clipboardPanel;

    LauncherController *m_launcherController;
    QWidget *m_launcherPanel;

    TimeTrackerController *m_timeTrackerController;
    QWidget *m_timeTrackerPanel;

    QSystemTrayIcon *m_trayIcon;
    QMenu *m_trayMenu;
    QAction *m_showAction;
    QAction *m_quitAction;

    QCheckBox *m_autoStartCheckBox;
    
    bool m_forceQuit;
};

#endif

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QCloseEvent>

class TimerController;
class TodoController;
class NotesController;

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
    void onBackToHome();
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void onShowWindow();
    void onQuitApplication();

private:
    void setupUi();
    void setupConnections();
    void setupTrayIcon();
    void createTrayMenu();

    QWidget *m_homeWidget;
    QStackedWidget *m_stackedWidget;
    QPushButton *m_timerModuleBtn;
    QPushButton *m_todoModuleBtn;
    QPushButton *m_notesModuleBtn;
    
    TimerController *m_timerController;
    QWidget *m_timerPanel;
    
    TodoController *m_todoController;
    QWidget *m_todoPanel;
    
    NotesController *m_notesController;
    QWidget *m_notesPanel;
    
    QSystemTrayIcon *m_trayIcon;
    QMenu *m_trayMenu;
    QAction *m_showAction;
    QAction *m_quitAction;
    
    bool m_forceQuit;
};

#endif

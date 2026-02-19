#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QVBoxLayout>

class TimerController;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onTimerModuleClicked();

private:
    void setupUi();
    void setupConnections();

    QWidget *m_homeWidget;
    QStackedWidget *m_stackedWidget;
    QPushButton *m_timerModuleBtn;
    
    TimerController *m_timerController;
    QWidget *m_timerPanel;
};

#endif

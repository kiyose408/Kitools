#ifndef TODOCONTROLLER_H
#define TODOCONTROLLER_H

#include <QObject>
#include "desktoptodowidget.h"
#include "todosettingspanel.h"

class TodoController : public QObject
{
    Q_OBJECT

public:
    explicit TodoController(QObject *parent = nullptr);
    ~TodoController();
    
    QWidget* settingsPanel();
    DesktopTodoWidget* todoWidget();
    
    void showTodoWidget();
    void hideTodoWidget();

private:
    void setupConnections();

    TodoSettingsPanel *m_settingsPanel;
    DesktopTodoWidget *m_todoWidget;
    bool m_firstShow;
};

#endif

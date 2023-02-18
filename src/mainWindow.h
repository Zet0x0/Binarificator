#pragma once

#include "binarificatorTask.h"
#include <QScrollArea>

class MainWindow : public QWidget
{
private slots:
    void deductOnGoingTask();

private:
    QWidget *m_taskListWidget = new QWidget;
    QVBoxLayout *m_taskListWidgetLayout = new QVBoxLayout(m_taskListWidget);
    QPushButton *m_newTaskButton = new QPushButton("New Task");
    QScrollArea *m_taskListScrollArea = new QScrollArea;
    QVBoxLayout *m_layout = new QVBoxLayout(this);

    int m_onGoingTasks = 0;

    void closeEvent(QCloseEvent *);

public:
    static MainWindow *instance();
    MainWindow();

    static void addTask(const BinarificatorTask::Settings &);
};
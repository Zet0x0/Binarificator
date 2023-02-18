#include "mainWindow.h"
#include "taskCreationDialog.h"
#include <QCloseEvent>
#include <QMessageBox>

MainWindow *MainWindow::instance()
{
    static MainWindow *instance = new MainWindow;

    return instance;
}

MainWindow::MainWindow()
{
    connect(m_newTaskButton, &QPushButton::clicked, TaskCreationDialog::instance(),
            &TaskCreationDialog::open);

    m_taskListScrollArea->setWidget(m_taskListWidget);
    m_taskListScrollArea->setFrameShape(QFrame::Box);
    m_taskListScrollArea->setWidgetResizable(true);

    m_taskListWidgetLayout->addStretch(-1);

    m_layout->addWidget(m_taskListScrollArea);
    m_layout->addWidget(m_newTaskButton);

    resize(800, 600);
    show();
}

void MainWindow::addTask(const BinarificatorTask::Settings &settings)
{
    BinarificatorTaskWidget *binarificatorTaskWidget = new BinarificatorTaskWidget(settings);
    MainWindow *instance = MainWindow::instance();
    QVBoxLayout *taskListWidgetLayout = instance->m_taskListWidgetLayout;

    instance->m_onGoingTasks++;

    connect(binarificatorTaskWidget->task(), &BinarificatorTask::errorOccurred, instance,
            &MainWindow::deductOnGoingTask);
    connect(binarificatorTaskWidget->task(), &BinarificatorTask::finished, instance,
            &MainWindow::deductOnGoingTask);

    taskListWidgetLayout->insertWidget(taskListWidgetLayout->count() - 1, binarificatorTaskWidget);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_onGoingTasks != 0 and
        QMessageBox::question(
            this, "Quitting",
            QString("Are you sure you want to quit now? You have %0 on-going task%1 right now.")
                .arg(QString::number(m_onGoingTasks), (m_onGoingTasks == 1) ? "" : "s")) !=
            QMessageBox::Yes)
    {
        return event->ignore();
    }

    qApp->quit();
}

void MainWindow::deductOnGoingTask()
{
    m_onGoingTasks--;
}
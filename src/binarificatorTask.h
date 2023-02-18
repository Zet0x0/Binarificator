#pragma once

#include <QGridLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QStackedWidget>
#include <QThread>

class BinarificatorTask : public QObject
{
    Q_OBJECT

public:
    struct Settings
    {
        QSizeF outputSizeMultiplication;
        QSize outputSize;

        QColor backgroundColor;

        QString outputFile;
        QString inputFile;

        QFont font;
    };

private:
    QThread *m_thread = new QThread;

    void binarify(const Settings &);

signals:
    void errorOccurred(const QString &);

    void progressChanged(const int &);

    void finished();

public:
    BinarificatorTask(const Settings &);
};

class BinarificatorTaskWidget : public QFrame
{
private:
    QPushButton *m_discardButton = new QPushButton("Discard");
    QStackedWidget *m_stackedWidget = new QStackedWidget;
    QProgressBar *m_progressBar = new QProgressBar;
    QGridLayout *m_layout = new QGridLayout(this);
    QLabel *m_settingsLabel = new QLabel;
    QLabel *m_stateLabel = new QLabel;

    BinarificatorTask::Settings m_taskSettings;
    BinarificatorTask *m_task;

    const QString colorToRgbaString(const QColor &);

public:
    BinarificatorTaskWidget(const BinarificatorTask::Settings &);

    BinarificatorTask *task();
};
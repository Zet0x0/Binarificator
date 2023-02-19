#include "binarificatorTask.h"
#include <QImageReader>
#include <QImageWriter>
#include <QPainter>
#include <QRandomGenerator>

BinarificatorTask::BinarificatorTask(const Settings &settings)
{
    connect(m_thread, &QThread::started, this,
            [this, settings]
            {
                binarify(settings);
            });

    moveToThread(m_thread);
    m_thread->start();
}

void BinarificatorTask::binarify(const Settings &settings)
{
    QImageReader inputImageReader(settings.inputFile);
    QImage inputImage = inputImageReader.read();

    if (inputImage.isNull())
    {
        return emit errorOccurred("[Image Reader] " + inputImageReader.errorString());
    }

    inputImage =
        (settings.outputSize.isValid())
            ? inputImage.scaled(settings.outputSize, Qt::IgnoreAspectRatio,
                                Qt::SmoothTransformation)
            : inputImage.scaled(inputImage.width() * settings.outputSizeMultiplication.width(),
                                inputImage.height() * settings.outputSizeMultiplication.height(),
                                Qt::KeepAspectRatio, Qt::SmoothTransformation);

    const int inputImageHeight = inputImage.height();
    const int inputImageWidth = inputImage.width();

    QImage outputImage(inputImageWidth, inputImageHeight, QImage::Format_ARGB32);
    QImageWriter outputImageWriter(settings.outputFile);
    const QFontMetrics fontMetrics(settings.font);
    QPainter painter(&outputImage);

    const QSize _0Size = fontMetrics.boundingRect("0").size();
    const QSize _1Size = fontMetrics.boundingRect("1").size();

    const int yStep = (_0Size.height() > _1Size.height()) ? _0Size.height() : _1Size.height();
    const int xStep = (_0Size.width() > _1Size.width()) ? _0Size.width() : _1Size.width();

    const int totalHeightWithYStep = (inputImageHeight - (inputImageHeight % yStep));

    outputImage.fill(settings.backgroundColor);
    outputImageWriter.setCompression(0);
    outputImageWriter.setQuality(100);
    painter.setFont(settings.font);

    for (int y = 0; y < inputImageHeight; y += yStep)
    {
        for (int x = 0; x < inputImageWidth; x += xStep)
        {
            const QColor pixelColor = inputImage.pixelColor(x, y);

            if (!pixelColor.alpha())
            {
                continue;
            }

            painter.setPen(pixelColor);
            painter.drawText(x, y, QString::number(QRandomGenerator::global()->bounded(2)));
        }

        emit progressChanged((double)y / totalHeightWithYStep * 100);
    }

    emit progressChanged(-1);

    painter.end();

    if (!outputImageWriter.write(outputImage))
    {
        return emit errorOccurred("[Image Writer] " + outputImageWriter.errorString());
    }

    emit finished();
}

BinarificatorTaskWidget::BinarificatorTaskWidget(const BinarificatorTask::Settings &settings)
    : m_task(new BinarificatorTask(settings))
{
    connect(m_task, &BinarificatorTask::progressChanged, m_progressBar,
            [this](const int &progress)
            {
                if (progress == -1)
                {
                    m_progressBar->setRange(0, 0);
                }
                else
                {
                    if (m_progressBar->maximum() != 100)
                    {
                        m_progressBar->setRange(0, 100);
                    }

                    m_progressBar->setValue(progress);
                }
            });
    connect(m_task, &BinarificatorTask::finished, m_stackedWidget,
            [this, settings]
            {
                m_stateLabel->setText(
                    QString("Finished. Saved to: <b><a href=\"file:///%0\">%0</a></b>")
                        .arg(settings.outputFile));
                m_stackedWidget->setCurrentIndex(1);
                m_discardButton->setEnabled(true);
            });
    connect(m_task, &BinarificatorTask::errorOccurred, m_stackedWidget,
            [this](const QString &errorString)
            {
                m_stateLabel->setText("Error: " + errorString);
                m_stackedWidget->setCurrentIndex(1);
            });
    connect(m_task, &BinarificatorTask::destroyed, this, &BinarificatorTaskWidget::deleteLater);
    connect(m_discardButton, &QPushButton::clicked, m_task, &BinarificatorTask::deleteLater);

    m_settingsLabel->setText(
        QString("<h1>%0</h1>Input: <b><a href=\"file:///%1\">%1</a></b> | Output: <b><a "
                "href=\"file:///%2\">%2</a></b><br>Background: <b>%4</b> | Font: "
                "<b>%5</b><br>Output Size: <b>%6</b> | Output Size Multiplication: <b>%7</b>")
            .arg(settings.inputFile.split("/", Qt::SkipEmptyParts).last(), settings.inputFile,
                 settings.outputFile, colorToRgbaString(settings.backgroundColor),
                 QString("%0, %1pt").arg(settings.font.family()).arg(settings.font.pointSize()),
                 (settings.outputSize.isValid()) ? QString("(%0, %1)")
                                                       .arg(settings.outputSize.width())
                                                       .arg(settings.outputSize.height())
                                                 : "Unspecified",
                 (settings.outputSizeMultiplication.isValid())
                     ? QString("(%0, %1)")
                           .arg(settings.outputSizeMultiplication.width(), 0, 'f', 6)
                           .arg(settings.outputSizeMultiplication.height(), 0, 'f', 6)
                     : "Unspecified"));
    m_settingsLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    m_stateLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    m_progressBar->setAlignment(Qt::AlignCenter);
    m_settingsLabel->setOpenExternalLinks(true);
    m_stateLabel->setOpenExternalLinks(true);
    m_stateLabel->setAlignment(Qt::AlignTop);
    m_discardButton->setDisabled(true);
    m_progressBar->setRange(0, 0);

    m_stackedWidget->addWidget(m_progressBar);
    m_stackedWidget->addWidget(m_stateLabel);

    m_layout->addWidget(m_settingsLabel, 0, 0);
    m_layout->addWidget(m_stackedWidget, 1, 0);
    m_layout->addWidget(m_discardButton, 2, 0);

    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    setFrameShape(StyledPanel);
}

const QString BinarificatorTaskWidget::colorToRgbaString(const QColor &color)
{
    return QString("rgba(%0, %1, %2, %3)")
        .arg(color.red())
        .arg(color.green())
        .arg(color.blue())
        .arg(color.alpha());
}

BinarificatorTask *BinarificatorTaskWidget::task()
{
    return m_task;
}
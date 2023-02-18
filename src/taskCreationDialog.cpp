#include "taskCreationDialog.h"
#include "mainWindow.h"
#include <QColorDialog>
#include <QFileDialog>
#include <QFontDialog>
#include <QImageReader>
#include <QImageWriter>
#include <QMessageBox>

TaskCreationDialog *TaskCreationDialog::instance()
{
    static TaskCreationDialog *instance = new TaskCreationDialog;

    return instance;
}

TaskCreationDialog::TaskCreationDialog()
{
    m_ui.setupUi(this);

    const QSize sizeHint = this->sizeHint();

    setFixedHeight(sizeHint.height());
    resize(sizeHint);
}

void TaskCreationDialog::open()
{
    if (!m_signalsConnected)
    {
        connect(
            m_ui.addTaskButton, &QPushButton::clicked, MainWindow::instance(),
            [this]
            {
                const QString outputFile = m_ui.outputFileLine->text().trimmed();
                const QString inputFile = m_ui.inputFileLine->text().trimmed();

                if (inputFile.isEmpty())
                {
                    return (void)QMessageBox::critical(this, "Error", "Input file field is empty.");
                }

                if (outputFile.isEmpty())
                {
                    return (void)QMessageBox::critical(this, "Error",
                                                       "Output file field is empty.");
                }

                if (!QFile::exists(inputFile) or
                    !QImageReader::supportedImageFormats().contains(
                        inputFile.split(".", Qt::SkipEmptyParts).last()))
                {
                    return (void)QMessageBox::critical(this, "Error", "Invalid input file.");
                }

                if (!QImageWriter::supportedImageFormats().contains(
                        outputFile.split(".", Qt::SkipEmptyParts).last()))
                {
                    return (void)QMessageBox::critical(this, "Error", "Invalid output file path.");
                }

                BinarificatorTask::Settings settings;

                if (m_ui.multiplySizeRadioButton->isChecked())
                {
                    settings.outputSizeMultiplication =
                        QSizeF(m_ui.multiplyWidthBySpinBox->value(),
                               m_ui.multiplyHeightBySpinBox->value());
                }
                else
                {
                    settings.outputSize =
                        QSize(m_ui.customWidthSpinBox->value(), m_ui.customHeightSpinBox->value());
                }

                settings.backgroundColor =
                    m_ui.backgroundColorButton->property("selectedBackgroundColor").value<QColor>();
                settings.font = m_ui.fontButton->property("selectedFont").value<QFont>();
                settings.outputFile = outputFile;
                settings.inputFile = inputFile;

                MainWindow::addTask(settings);
                QMessageBox::information(this, "Success", "Successfully added a new task.");
                MainWindow::instance()->activateWindow();
            });
        connect(m_ui.backgroundColorButton, &QPushButton::clicked, this,
                [this]
                {
                    QPushButton *backgroundColorButton = m_ui.backgroundColorButton;
                    const QColor selectedBackgroundColor = QColorDialog::getColor(
                        backgroundColorButton->property("selectedBackgroundColor").value<QColor>(),
                        this, QString(), QColorDialog::ShowAlphaChannel);

                    if (!selectedBackgroundColor.isValid())
                    {
                        return;
                    }

                    backgroundColorButton->setText(QString("rgba(%0, %1, %2, %3)")
                                                       .arg(selectedBackgroundColor.red())
                                                       .arg(selectedBackgroundColor.green())
                                                       .arg(selectedBackgroundColor.blue())
                                                       .arg(selectedBackgroundColor.alpha()));
                    backgroundColorButton->setProperty("selectedBackgroundColor",
                                                       selectedBackgroundColor);
                });
        connect(m_ui.fontButton, &QPushButton::clicked, this,
                [this]
                {
                    QPushButton *fontButton = m_ui.fontButton;
                    bool ok;
                    const QFont selectedFont = QFontDialog::getFont(
                        &ok, fontButton->property("selectedFont").value<QFont>());

                    if (!ok)
                    {
                        return;
                    }

                    fontButton->setText(
                        QString("%0, %1pt")
                            .arg(selectedFont.family(), QString::number(selectedFont.pointSize())));
                    fontButton->setProperty("selectedFont", selectedFont);
                });
        connect(m_ui.multiplySizeRadioButton, &QRadioButton::toggled, this,
                [this](const bool &toggled)
                {
                    m_ui.multiplyHeightBySpinBox->setEnabled(toggled);
                    m_ui.multiplyWidthBySpinBox->setEnabled(toggled);
                    m_ui.multiplyHeightByLabel->setEnabled(toggled);
                    m_ui.multiplyWidthByLabel->setEnabled(toggled);

                    m_ui.customHeightSpinBox->setDisabled(toggled);
                    m_ui.customWidthSpinBox->setDisabled(toggled);
                    m_ui.customHeightLabel->setDisabled(toggled);
                    m_ui.customWidthLabel->setDisabled(toggled);
                });
        connect(m_ui.outputFileBrowseButton, &QToolButton::clicked, this,
                [this]
                {
                    const QString outputFile = QFileDialog::getSaveFileName(
                        this, QString(), QString(),
                        QString("Image File (*.%0)")
                            .arg(QImageWriter::supportedImageFormats().join("; *.")));

                    if (!outputFile.isEmpty())
                    {
                        m_ui.outputFileLine->setText(outputFile);
                    }
                });
        connect(m_ui.inputFileBrowseButton, &QToolButton::clicked, this,
                [this]
                {
                    const QString inputFile = QFileDialog::getOpenFileName(
                        this, QString(), QString(),
                        QString("Image File (*.%0)")
                            .arg(QImageReader::supportedImageFormats().join("; *.")));

                    if (!inputFile.isEmpty())
                    {
                        m_ui.inputFileLine->setText(inputFile);
                    }
                });

        m_signalsConnected = true;
    }

    if (!isHidden() and !isActiveWindow())
    {
        activateWindow();
    }
    else
    {
        QDialog::open();
    }
}
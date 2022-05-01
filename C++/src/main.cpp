#include <QRandomGenerator>
#include <QElapsedTimer>
#include <QApplication>
#include <QColorDialog>
#include <QImageReader>
#include <QImageWriter>
#include <QFileDialog>
#include <QFontDialog>
#include <QMessageBox>
#include <QPainter>

QString convertMilliseconds(int milliseconds)
{
    int hours = milliseconds / 3600000;
    int minutes = (milliseconds - hours * 3600000) / 60000;

    return QString("%0:%1:%2").arg(QString::number(hours).rightJustified(2, '0'), QString::number(minutes).rightJustified(2, '0'), QString::number((milliseconds - hours * 3600000 - minutes * 60000) / 1000).rightJustified(2, '0'));
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    qInstallMessageHandler([](QtMsgType, const QMessageLogContext &, const QString &message)
                           {
                               if (!message.startsWith("\n"))
                               {
                                   printf("%s\r", message.toStdString().c_str());
                               }
                               else
                               {
                                   printf(message.toStdString().c_str());
                               } });

    QString imageFileName = QFileDialog::getOpenFileName(0, "", "", QString("Image File (*.%0)").arg(QImageReader::supportedImageFormats().join("; *.")));

    if (imageFileName.isEmpty())
    {
        return 1;
    }

    QString resultFileName = QFileDialog::getSaveFileName(0, "", "", QString("Image File (*.%0)").arg(QImageWriter::supportedImageFormats().join("; *.")));

    if (resultFileName.isEmpty())
    {
        return 1;
    }

    bool ok;
    QFontMetrics fontMetrics(QFontDialog::getFont(&ok, QFont("Consolas", 10), 0, "", QFontDialog::MonospacedFonts));

    QImage image(imageFileName);

    if (image.isNull())
    {
        return 1;
    }

    if (QMessageBox::question(0, "", "Would you like to resize the image to make zeros and ones less visible? This could affect performance!") == QMessageBox::Yes)
    {
        image = image.scaled(
            image.width() * fontMetrics.boundingRect("0").width(),
            image.height() * fontMetrics.boundingRect("0").height(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation);
    }

    QImage result(image.size(), image.format());
    QPainter painter(&result);

    if (QMessageBox::question(0, "", "Would you like to change the backround from transparent to something else?") == QMessageBox::Yes)
    {
        QColor fillColor(QColorDialog::getColor(Qt::black, 0, "", QColorDialog::ShowAlphaChannel));

        if (fillColor.isValid())
        {
            result.fill(fillColor);
        }
    }

    QElapsedTimer timer;
    timer.start();

    for (int y = 0; y < image.height(); y += fontMetrics.boundingRect("0").height())
    {
        for (int x = 0; x < image.width(); x += fontMetrics.boundingRect("0").width())
        {
            QColor pixelColor = image.pixelColor(x, y);

            if (!pixelColor.alpha())
            {
                continue;
            }

            painter.setPen(pixelColor);
            painter.drawText(x, y, QString::number(QRandomGenerator::global()->bounded(2)));

            qDebug().noquote() << "Rendering..."
                               << QString("%0%").arg(QString::number(double(y * image.width() + x) / double(image.width() * image.height()) * 100.0, 'f', 2).rightJustified(6, ' '))
                               << "|"
                               << QString("X(%0/%1);").arg(QString::number(x).rightJustified(QString::number(image.width()).length(), ' '), QString::number(image.width()))
                               << QString("Y(%0/%1);").arg(QString::number(y).rightJustified(QString::number(image.height()).length(), ' '), QString::number(image.height()))
                               << "|"
                               << "Time elapsed:" << convertMilliseconds(timer.elapsed());
        }
    }

    painter.end();

    bool resultSaved = result.save(resultFileName, 0, 100);

    qDebug() << ((resultSaved) ? "\nDone\n" : "\nCould not save the image\n");
    QMessageBox::information(0, "", (resultSaved) ? "Done" : "Could not save the image");

    return 0;
}

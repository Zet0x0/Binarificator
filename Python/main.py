# -*- coding: utf-8 -*-

from PyQt6.QtWidgets import (QFontDialog, QFileDialog, QApplication,
                             QMessageBox, QColorDialog)
from PyQt6.QtGui import (QImage, QFontMetrics, QFont, QPainter, QImageReader,
                         QImageWriter)
from PyQt6.QtCore import Qt, QRandomGenerator, QElapsedTimer


def convertMilliseconds(milliseconds: int) -> str:
    hours = milliseconds // 3600000
    minutes = (milliseconds - hours * 3600000) // 60000

    return f"{str(hours).rjust(2, '0')}:{str(minutes).rjust(2, '0')}:{str((milliseconds - hours * 3600000 - minutes * 60000) // 1000).rjust(2, '0')}"


app = QApplication([])
app.setApplicationName("Binarificator")

imageFileName = QFileDialog.getOpenFileName(
    None, "", "",
    f"Image File (*.{'; *.'.join(x.data().decode('utf-8', 'ignore') for x in QImageReader.supportedImageFormats())})"
)[0]

if not imageFileName: exit(1)

resultFileName = QFileDialog.getSaveFileName(
    None, "", "",
    f"Image File (*.{'; *.'.join(x.data().decode('utf-8', 'ignore') for x in QImageWriter.supportedImageFormats())})"
)[0]

if not resultFileName: exit(1)

font = QFontDialog.getFont(QFont("Consolas", 10))[0]
fontMetrics = QFontMetrics(font)

image = QImage(imageFileName)

if image.isNull(): exit(1)

if QMessageBox.question(
        None, "",
        "Would you like to resize the image to make zeros and ones less visible? This could affect performance!"
) == QMessageBox.StandardButton.Yes:
    image = image.scaled(
        image.width() * fontMetrics.boundingRect("0").width(),
        image.height() * fontMetrics.boundingRect("0").height(),
        Qt.AspectRatioMode.KeepAspectRatio,
        Qt.TransformationMode.SmoothTransformation)

result = QImage(image.size(), QImage.Format.Format_ARGB32)
painter = QPainter(result)
timer = QElapsedTimer()

if QMessageBox.question(
        None, "",
        "Would you like to change the backround from transparent to something else?"
) == QMessageBox.StandardButton.Yes:
    color = QColorDialog.getColor(
        0, None, "", QColorDialog.ColorDialogOption.ShowAlphaChannel)

    if color.isValid(): result.fill(color)

painter.setFont(font)
timer.start()

for y in range(
        0, image.height(),
        fontMetrics.boundingRect("0").height()
        if fontMetrics.boundingRect("0").height() >
        fontMetrics.boundingRect("1").height() else
        fontMetrics.boundingRect("1").height()):
    for x in range(
            0, image.width(),
            fontMetrics.boundingRect("0").width()
            if fontMetrics.boundingRect("0").width() >
            fontMetrics.boundingRect("1").width() else
            fontMetrics.boundingRect("1").width()):
        color = image.pixelColor(x, y)

        if not color.alpha(): continue

        painter.setPen(color)
        painter.drawText(x, y, str(QRandomGenerator.global_().bounded(2)))

        print(
            f"Rendering... {format(round((y * image.width() + x) / (image.width() * image.height()) * 100, 2), '.2f').rjust(6, ' ')}% | X({str(x).rjust(len(str(image.width())), ' ')}/{image.width()}); Y({str(y).rjust(len(str(image.height())), ' ')}/{image.height()}) | Time elapsed: {convertMilliseconds(timer.elapsed())}",
            end="\r")

painter.end()

print("\nDone" if result.save(resultFileName, "", 100
                              ) else "\nCould not save the image")

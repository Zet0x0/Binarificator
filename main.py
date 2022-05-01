# -*- coding: utf-8 -*-

from PyQt6.QtWidgets import (QFontDialog, QFileDialog, QApplication,
                             QMessageBox, QColorDialog)
from PyQt6.QtGui import (QImage, QFontMetrics, QFont, QPainter, QImageReader,
                         QImageWriter)
from time import perf_counter as performanceCounter
from random import randint as randInt
from PyQt6.QtCore import Qt


def convertSeconds(seconds: float) -> str:
    return f"{str(int(seconds // 3600)).rjust(2, '0')}:{str(int(seconds % 3600 // 60)).rjust(2, '0')}:{str(int(seconds % 60)).rjust(2, '0')}"


app = QApplication([])

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

font = QFontDialog.getFont(QFont("Consolas", 10), None, "",
                           QFontDialog.FontDialogOption.MonospacedFonts)[0]

if not font: exit(1)

fontMetrics = QFontMetrics(font)

image = QImage(imageFileName)

if QMessageBox.question(
        None, "",
        "Would you like to resize the image to make zeros and ones less visible? This could affect performance!"
) == QMessageBox.StandardButton.Yes:
    image = image.scaled(
        image.width() * fontMetrics.boundingRectChar("0").width(),
        image.height() * fontMetrics.boundingRectChar("0").height(),
        Qt.AspectRatioMode.KeepAspectRatio,
        Qt.TransformationMode.SmoothTransformation)

result = QImage(image.size(), image.format())
painter = QPainter(result)

if QMessageBox.question(
        None, "",
        "Would you like to change the backround from transparent to something else?"
) == QMessageBox.StandardButton.Yes:
    color = QColorDialog.getColor(
        0, None, "", QColorDialog.ColorDialogOption.ShowAlphaChannel)

    if color.isValid(): painter.fillRect(result.rect(), color)

startTime = performanceCounter()

for y in range(0, image.height(), fontMetrics.boundingRectChar("0").height()):
    for x in range(0, image.width(),
                   fontMetrics.boundingRectChar("0").width()):
        color = image.pixelColor(x, y)

        if not color.alpha(): continue

        painter.setPen(color)
        painter.drawText(x, y, str(randInt(0, 1)))

        print(
            f"Rendering... {format(round((y * image.width() + x) / (image.width() * image.height()) * 100, 2), '.2f').rjust(6, ' ')}% | X({str(x).rjust(len(str(image.width())), ' ')}/{image.width()}); Y({str(y).rjust(len(str(image.height())), ' ')}/{image.height()}) | Time elapsed: {convertSeconds(performanceCounter() - startTime)}",
            end="\r")

painter.end()

print("\nDone" if result.save(resultFileName, "", 100
                              ) else "\nCould not save the image")

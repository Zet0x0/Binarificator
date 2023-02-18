#pragma once

#include "ui_taskCreationDialog.h"

class TaskCreationDialog : public QDialog
{
public slots:

    void open();

private:
    bool m_signalsConnected = false;

    Ui::TaskCreationDialog m_ui;

public:
    static TaskCreationDialog *instance();
    TaskCreationDialog();
};
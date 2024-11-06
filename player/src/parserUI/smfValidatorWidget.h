#pragma once

#include <QDialog>

// TODO: Think about sending file name or else

#include "onvif_validator.h"

namespace Ui {
    class SMFValidationWidget;
}

class SMFValidationWidget final : public QDialog {
private:
    Q_OBJECT

public:
    SMFValidationWidget(QWidget* parent, const QString& file_name, const QString& codecString);

    virtual ~SMFValidationWidget();

    void validationCallback(ValidationResult validationResult); 

private:
    Ui::SMFValidationWidget* m_ui;
};

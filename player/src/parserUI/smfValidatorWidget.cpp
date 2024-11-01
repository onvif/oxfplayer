#include "smfValidatorWidget.h"

#include "ui_smfValidatorWidget.h"

SMFValidationWidget* widget = nullptr;

void validationFinished(ValidationResult validationResult) {
    if (widget)
        widget->accept();
    widget->validationCallback(validationResult);
}

SMFValidationWidget::SMFValidationWidget(QWidget* parent) : QDialog(parent), m_ui(new Ui::SMFValidationWidget()) {
    m_ui->setupUi(this);

    QObject::connect(m_ui->ok_btn, &QPushButton::clicked, this, [this]() {
        widget = nullptr;
        this->accept();
    });

    ::widget = this;

    validation_callback(validationFinished);

    validate((gchar*)"test", (gchar*)"test", (gchar*)"test");

    // uncomment this to see how it will looks like
    //ValidationResult validation_result;
    //validation_result.public_key_is_valid = true;
    //validation_result.video_is_valid = true;
    //validation_result.vendor_info.serial_number = "N/A";
    //validation_result.vendor_info.firmware_version = "v0.0.4";
    //validation_result.vendor_info.manufacturer = "Signed Media Framework";
    //validation_result.gop_info.valid_gops_count = 50;
    //validation_result.gop_info.valid_gops_with_missing_nalu_count = 0;
    //validation_result.gop_info.invalid_gops_count = 0;
    //validation_result.gop_info.gops_without_signature_count = 0;
    //validationCallback(validation_result);
}

SMFValidationWidget::~SMFValidationWidget() {
    widget = nullptr;
    delete m_ui;
}

void SMFValidationWidget::validationCallback(ValidationResult validationResult) {
    m_ui->stackedWidget->setCurrentIndex(1);
    QString message;
    message += QString{ "PUBLIC KEY IS " } + (validationResult.public_key_is_valid ? QString{ "VALID" } : QString{ "INVALID" }) + QString{ "<br/>" };
    message += QString{ "VIDEO IS " } + (validationResult.public_key_is_valid ? QString{ "VALID" } : QString{ "INVALID" }) + QString{ "<br/>" };
    message += QString{ "<br/>" };
    message += QString{ "Vendor Info:<br/>" };
    message += QString{ "Serial Number: " } +
               (validationResult.vendor_info.serial_number ? QString{ validationResult.vendor_info.serial_number } : QString{ "N/A" }) +
               QString{ "<br/>" };
    message += QString{ "Firmware Version: " } +
               (validationResult.vendor_info.firmware_version ? QString{ validationResult.vendor_info.firmware_version } : QString{ "N/A" }) +
               QString{ "<br/>" };
    message += QString{ "Manufacturer: " } +
               (validationResult.vendor_info.manufacturer ? QString{ validationResult.vendor_info.manufacturer } : QString{ "N/A" }) +
               QString{ "<br/>" };
    message += QString{ "<br/>" };
    message += QString{ "GOP Info:<br/>" };
    message += QString{ "Number of valid GOPs: " } + QString::number(validationResult.gop_info.valid_gops_count) + QString{ "<br/>" };
    message += QString{ "Number of valid GOPs with missing NALUs: " } + QString::number(validationResult.gop_info.valid_gops_with_missing_nalu_count) + QString{ "<br/>" };
    message += QString{ "Number of invalid GOPs: " } + QString::number(validationResult.gop_info.invalid_gops_count) + QString{ "<br/>" };
    message += QString{ "Number of GOPs without signature: " } + QString::number(validationResult.gop_info.gops_without_signature_count) + QString{ "<br/>" };
    m_ui->textEdit->setText(message);
}
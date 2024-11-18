#include "smfValidatorWidget.h"

#include <QFileDialog>
#include <QSettings>

#include "defines.h"
#include "ui_smfValidatorWidget.h"
#include <qtemporaryfile.h>

namespace {

    SMFValidationWidget* widget = nullptr;

    void validationFinished(ValidationResult validationResult) {
        if (widget)
            widget->validationCallback(validationResult);
    }

    QString GetCertPath() {
#ifdef WIN32
        return QSettings(QDir::homePath() + WINP_APP_DATA_ROAMING + COMPANY_NAME + "/" + PRODUCT_NAME + "/" + CONFIG_FILE_NAME, QSettings::IniFormat)
#endif // WIN32
#ifdef __linux__
        return QSettings(QDir::homePath() + "/." + PRODUCT_NAME + "/" + CONFIG_FILE_NAME, QSettings::IniFormat)
#endif //__linux__
        .value("SMFPath", "").toString();
    }

    void SetCertPath(const QString& path) {
#ifdef WIN32
        QSettings(QDir::homePath() + WINP_APP_DATA_ROAMING + COMPANY_NAME + "/" + PRODUCT_NAME + "/" + CONFIG_FILE_NAME, QSettings::IniFormat)
#endif // WIN32
#ifdef __linux__
        QSettings(QDir::homePath() + "/." + PRODUCT_NAME + "/" + CONFIG_FILE_NAME, QSettings::IniFormat)
#endif //__linux__
        .setValue("SMFPath", path);
    }

} // namespace

SMFValidationWidget::SMFValidationWidget(QWidget* parent, const QString& file_name, const QString& codecString)
    : QDialog(parent), m_ui(new Ui::SMFValidationWidget()) {
    m_ui->setupUi(this);

    m_ui->certificate_path_txt->setText(GetCertPath());
    m_ui->validate_btn->setText(QObject::tr("Validate"));
    m_ui->validate_btn->show();

    m_ui->codec_cbx->addItem("H.264", "h264");
    m_ui->codec_cbx->addItem("H.265", "h265");
    m_ui->codec_cbx->setCurrentIndex(m_ui->codec_cbx->findData(codecString));

    QObject::connect(m_ui->certificate_path_btn, &QPushButton::clicked, this, [this]() {
        const QString certificateFilePath =
            QFileDialog::getOpenFileName(nullptr, "Select certificate file..", m_ui->certificate_path_txt->text(), BASE64_FORMAT);
        m_ui->certificate_path_txt->setText(certificateFilePath);
        SetCertPath(certificateFilePath);
    });

    QObject::connect(m_ui->validate_btn, &QPushButton::clicked, this, [this, file_name]() {
        if (m_ui->stackedWidget->currentIndex() == 0) {
            m_ui->validate_btn->hide();
            m_ui->stackedWidget->setCurrentIndex(1);

            m_tmpFile = new QTemporaryFile();
            m_tmpFile->open();
            ::widget = this;
            validation_callback(validationFinished);
            validate((gchar*)m_ui->codec_cbx->currentData().value<QString>().toStdString().data(),
                     (gchar*)m_ui->certificate_path_txt->text().toStdString().data(), (gchar*)file_name.toStdString().data(),
                     m_ui->bulk_cb->isChecked(), m_tmpFile->fileName().toUtf8().constData());
        }
        else if (m_ui->stackedWidget->currentIndex() == 2) {
            m_ui->validate_btn->setText(QObject::tr("Validate"));
            m_ui->validate_btn->show();
            m_ui->stackedWidget->setCurrentIndex(0);
        }
    });

    QObject::connect(m_ui->close_btn, &QPushButton::clicked, this, [this]() {
        widget = nullptr;
        this->accept();
    });
}

SMFValidationWidget::~SMFValidationWidget() {
    widget = nullptr;
    delete m_ui;
}

void SMFValidationWidget::validationCallback(ValidationResult validationResult) {
    m_ui->validate_btn->setText(QObject::tr("Again"));
    m_ui->validate_btn->show();
    m_ui->stackedWidget->setCurrentIndex(2);
    widget = nullptr;

    QString message;

    {
        QFile validationResultsFile(m_tmpFile->fileName());
        if (!validationResultsFile.open(QIODevice::ReadOnly)) {
            message = "Failed to open results file...";
        } else
            message = validationResultsFile.readAll();
        delete m_tmpFile;
        m_tmpFile = nullptr;
    }

    m_ui->textEdit->setText(message);
}
#include "addpatientdialog.h"
#include "ui_addpatientdialog.h"
#include <QMessageBox>

AddPatientDialog::AddPatientDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddPatientDialog)
{
    ui->setupUi(this);
    connect(ui->patient_button_box, &QDialogButtonBox::accepted, this, &AddPatientDialog::on_addPatientButton_clicked);
    connect(ui->patient_button_box, &QDialogButtonBox::rejected, this, &AddPatientDialog::reject);
}

AddPatientDialog::~AddPatientDialog()
{
    delete ui;
}

AddPatientDialog::AddPatientDialog(const std::shared_ptr<Patient>& patient, QWidget *parent)
    : AddPatientDialog(parent) {
    ui->patient_ID_number_line->setText(patient->getId());
    ui->patient_name_line->setText(patient->getName());
    ui->patient_surname_line->setText(patient->getSurname());
    ui->patient_phone_line->setText(patient->getPhone());
    ui->patient_email_line->setText(patient->getEmail());
}

void AddPatientDialog::on_addPatientButton_clicked() {
    auto id = ui->patient_ID_number_line->text();
    auto name = ui->patient_name_line->text();
    auto surname = ui->patient_surname_line->text();
    auto email = ui->patient_email_line->text();
    auto phone = ui->patient_phone_line->text();
    auto patient = std::make_shared<Patient>(name, surname, email, phone, id);
    // if any of the fields is empty, show error message
    if(id.isEmpty() || name.isEmpty() || surname.isEmpty() || email.isEmpty()) {
        QMessageBox::critical(this, "Error", "All fields are required!");
        return;
    }
    emit patientChanged(patient);
}

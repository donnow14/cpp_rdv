#include "adddoctordialog.h"
#include "ui_adddoctordialog.h"
#include <QMessageBox>

AddDoctorDialog::AddDoctorDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddDoctorDialog)
{
    ui->setupUi(this);
    connect(ui->doctor_button_box, &QDialogButtonBox::accepted, this, &AddDoctorDialog::on_addDoctorButton_clicked);
    connect(ui->doctor_button_box, &QDialogButtonBox::rejected, this, &AddDoctorDialog::reject);
}

AddDoctorDialog::~AddDoctorDialog()
{
    delete ui;
}

AddDoctorDialog::AddDoctorDialog(const std::shared_ptr<Doctor>& doctor, QWidget *parent)
    : AddDoctorDialog(parent) {
    ui->doctor_ID_number_line->setText(doctor->getId());
    ui->doctor_name_line->setText(doctor->getName());
    ui->doctor_surname_line->setText(doctor->getSurname());
    ui->doctor_speciality_line->setText(doctor->getSpecialization());
    ui->doctor_email_line->setText(doctor->getEmail());
}

void AddDoctorDialog::on_addDoctorButton_clicked() {
    auto id = ui->doctor_ID_number_line->text();
    auto name = ui->doctor_name_line->text();
    auto surname = ui->doctor_surname_line->text();
    auto specialization = ui->doctor_speciality_line->text();
    auto email = ui->doctor_email_line->text();
    auto doctor = std::make_shared<Doctor>(name, surname, specialization, email, id);
    // if any of the fields is empty, show error message
    if(id.isEmpty() || name.isEmpty() || surname.isEmpty() || specialization.isEmpty() || email.isEmpty()) {
        QMessageBox::critical(this, "Error", "All fields are required!");
        return;
    }
    emit doctorChanged(doctor);
}

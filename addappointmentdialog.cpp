#include "addappointmentdialog.h"
#include "ui_addappointmentdialog.h"
#include <QMessageBox>

AddAppointmentDialog::AddAppointmentDialog(const QVector<std::shared_ptr<Doctor>>& doctors, const QVector<std::shared_ptr<Patient>>& patients, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddAppointmentDialog) {
    ui->setupUi(this);

    // create in-dialog copies of doctors and patients
    this->doctors = doctors;
    this->patients = patients;
    // populate doctors combo box
    for(const auto& doctor : doctors) {
        auto doctorName = QString("%1 %2").arg(doctor->getName(), doctor->getSurname());
        ui->doctor_combobox->addItem(doctorName, QVariant::fromValue(doctor->getId()));
    }

    // populate patients combo box
    for(const auto& patient : patients) {
        auto patientName = QString("%1 %2").arg(patient->getName(), patient->getSurname());
        ui->patient_combobox->addItem(patientName, QVariant::fromValue(patient->getId()));
    }

    connect(ui->appointment_button_box, &QDialogButtonBox::accepted, this, &AddAppointmentDialog::on_add_button_clicked);
    connect(ui->appointment_button_box, &QDialogButtonBox::rejected, this, &AddAppointmentDialog::reject);
}

AddAppointmentDialog::AddAppointmentDialog(const QVector<std::shared_ptr<Doctor>>& doctors, const QVector<std::shared_ptr<Patient>>& patients, const std::shared_ptr<Doctor>& doctor, const std::shared_ptr<Patient>& patient, const QDateTime& dateTime, int duration, QWidget *parent)
    : AddAppointmentDialog(doctors, patients, parent) {
    ui->date_time_edit->setDateTime(dateTime);
    ui->hour_spinbox->setValue(duration);
    // find doctor and patient
    for(int i = 0; i < ui->doctor_combobox->count(); i++) {
        if(ui->doctor_combobox->itemData(i).toString() == doctor->getId()) {
            ui->doctor_combobox->setCurrentIndex(i);
            break;
        }
    }
    for(int i = 0; i < ui->patient_combobox->count(); i++) {
        if(ui->patient_combobox->itemText(i) == QString("%1 %2").arg(patient->getName(), patient->getSurname())) {
            ui->patient_combobox->setCurrentIndex(i);
            break;
        }
    }
}

AddAppointmentDialog::~AddAppointmentDialog() {
    delete ui;
}

void AddAppointmentDialog::on_add_button_clicked() {
    auto doctorId = ui->doctor_combobox->currentData().toString();
    auto patientId = ui->patient_combobox->currentData().toString();
    auto dateTime = ui->date_time_edit->dateTime();
    auto duration = ui->hour_spinbox->value();

    // if any of the fields is empty, show error message
    if(doctorId.isEmpty() || patientId.isEmpty()) {
        QMessageBox::critical(this, "Error", "All fields are required!");
        return;
    }

    // find doctor and patient
    std::shared_ptr<Doctor> doctor = nullptr;
    std::shared_ptr<Patient> patient = nullptr;

    for(const auto& d : doctors) {
        if(d->getId() == doctorId) {
            doctor = d;
            break;
        }
    }

    for(const auto& p : patients) {
        if(p->getId() == patientId) {
            patient = p;
            break;
        }
    }

    auto appointment = std::make_shared<Appointment>(patient, doctor, dateTime, duration);
    emit appointmentChanged(appointment);
}

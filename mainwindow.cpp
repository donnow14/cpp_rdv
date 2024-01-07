#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSystemTrayIcon>
#include <QSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>
#include "adddoctordialog.h"
#include "addpatientdialog.h"
#include "addappointmentdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // create database connection to SQLite database (create if not exists)
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("database.sqlite");
    if (!db.open()) {
        QMessageBox::critical(this, "Error", "Cannot open database.");
        return;
    }

    // create database tables if not exists
    QSqlQuery query(db);
    query.exec("CREATE TABLE IF NOT EXISTS doctors (name TEXT, surname TEXT, specialization TEXT, email TEXT, id TEXT PRIMARY KEY)");
    query.exec("CREATE TABLE IF NOT EXISTS patients (name TEXT, surname TEXT, email TEXT, phone TEXT, id TEXT PRIMARY KEY)");
    query.exec("CREATE TABLE IF NOT EXISTS appointments (patient_id TEXT, doctor_id TEXT, date_time TEXT, duration INTEGER)");

    // get all doctors from database
    query.exec("SELECT * FROM doctors");
    while(query.next()) {
        auto doctor = std::make_shared<Doctor>(query.value(0).toString(), query.value(1).toString(), query.value(2).toString(), query.value(3).toString(), query.value(4).toString());
        doctors.append(doctor);
    }

    // get all patients from database
    query.exec("SELECT * FROM patients");
    while(query.next()) {
        auto patient = std::make_shared<Patient>(query.value(0).toString(),
                                                 query.value(1).toString(),
                                                 query.value(2).toString(),
                                                 query.value(3).toString(),
                                                 query.value(4).toString());
        patients.append(patient);
    }

    // get all appointments from database
    query.exec("SELECT * FROM appointments");
    while(query.next()) {
        std::shared_ptr<Patient> patient;
        std::shared_ptr<Doctor> doctor;
        for (auto &p : patients) {
            if (p->getId() == query.value(0).toString()) {
                patient = p;
                break;
            }
        }
        for (auto &d : doctors) {
            if (d->getId() == query.value(1).toString()) {
                doctor = d;
                break;
            }
        }
        // if patient or doctor is not found, skip this appointment
        if (!patient || !doctor) {
            continue;
        }
        auto appointment = std::make_shared<Appointment>(patient, doctor, QDateTime::fromString(query.value(2).toString(), "yyyy-MM-dd hh:mm:ss"), query.value(3).toInt());
        appointments.append(appointment);
    }

    // start a thread to send desktop notifications. The thread will scan all appointments and send notifications for those that are in the same minute as the current time
    this->appointmentThread = new QThread();
    connect(this->appointmentThread, &QThread::started, [this]() {
        while (true) {
            for (auto &appointment : appointments) {
                auto now = QDateTime::currentDateTime();
                auto doctorName = QString("%1 %2").arg(appointment->getDoctor()->getName(), appointment->getDoctor()->getSurname());
                auto patientName = QString("%1 %2").arg(appointment->getPatient()->getName(), appointment->getPatient()->getSurname());
                auto appointmentStart = appointment->getDateTime();
                if (now.secsTo(appointmentStart) == 0) {
                    emit desktopNotification("Appointment", QString("Appointment of %1 with %2 is imminent.").arg(patientName, doctorName));
                }
            }
            QThread::sleep(1);
        }
    });
    this->appointmentThread->start();

    // connect desktop notification signal
    connect(this, &MainWindow::desktopNotification, this, &MainWindow::sendDesktopNotification);

    // update all tables
    updateDoctors();
    updatePatients();
    updateAppointments();

    // make table edit buttons disabled when a row is deselected
    connect(ui->doctorsTable, &QTableWidget::itemSelectionChanged, [this]() {
        if (ui->doctorsTable->selectedItems().size() == 0) {
            ui->edit_doctor_button->setEnabled(false);
            ui->delete_doctor_button->setEnabled(false);
        } else {
            ui->edit_doctor_button->setEnabled(true);
            ui->delete_doctor_button->setEnabled(true);
        }
    });

    connect(ui->patientsTable, &QTableWidget::itemSelectionChanged, [this]() {
        if (ui->patientsTable->selectedItems().size() == 0) {
            ui->edit_patient_button->setEnabled(false);
            ui->delete_patient_button->setEnabled(false);
        } else {
            ui->edit_patient_button->setEnabled(true);
            ui->delete_patient_button->setEnabled(true);
        }
    });

    connect(ui->appointmentsTable, &QTableWidget::itemSelectionChanged, [this]() {
        if (ui->appointmentsTable->selectedItems().size() == 0) {
            ui->edit_appointment_button->setEnabled(false);
            ui->delete_appointment_button->setEnabled(false);
        } else {
            ui->edit_appointment_button->setEnabled(true);
            ui->delete_appointment_button->setEnabled(true);
        }
    });
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::sendDesktopNotification(const QString& title, const QString& message) {
    QSystemTrayIcon* trayIcon = new QSystemTrayIcon(this);
    trayIcon->show();
    trayIcon->showMessage(title, message, QSystemTrayIcon::Information, 5000);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    // save all appointments to database
    QSqlQuery query(db);
    query.exec("DELETE FROM appointments");
    for (auto &appointment : appointments) {
        query.prepare("INSERT INTO appointments (patient_id, doctor_id, date_time, duration) VALUES (:patient_id, :doctor_id, :date_time, :duration)");
        query.bindValue(":patient_id", appointment->getPatient()->getId());
        query.bindValue(":doctor_id", appointment->getDoctor()->getId());
        query.bindValue(":date_time", appointment->getDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        query.bindValue(":duration", appointment->getDuration());
        query.exec();
    }
    // save all patients to database
    query.exec("DELETE FROM patients");
    for (auto &patient : patients) {
        query.prepare("INSERT INTO patients (name, surname, email, phone, id) VALUES (:name, :surname, :email, :phone, :id)");
        query.bindValue(":name", patient->getName());
        query.bindValue(":surname", patient->getSurname());
        query.bindValue(":email", patient->getEmail());
        query.bindValue(":phone", patient->getPhone());
        query.bindValue(":id", patient->getId());
        query.exec();
    }
    // save all doctors to database
    query.exec("DELETE FROM doctors");
    for (auto &doctor : doctors) {
        query.prepare("INSERT INTO doctors (name, surname, specialization, email, id) VALUES (:name, :surname, :specialization, :email, :id)");
        query.bindValue(":name", doctor->getName());
        query.bindValue(":surname", doctor->getSurname());
        query.bindValue(":specialization", doctor->getSpecialization());
        query.bindValue(":email", doctor->getEmail());
        query.bindValue(":id", doctor->getId());
        query.exec();
    }
    // stop the thread
    this->appointmentThread->terminate();
    // close db
    db.close();
}


void MainWindow::updateDoctors() {
    // delete all doctors from table
    ui->doctorsTable->setRowCount(0);
    // populate doctors table (first column is id, second is full name)
    ui->doctorsTable->setRowCount(doctors.size());
    ui->doctorsTable->setColumnCount(2);
    ui->doctorsTable->setHorizontalHeaderLabels(QStringList() << "ID" << "Full name");
    for (int i = 0; i < doctors.size(); i++) {
        ui->doctorsTable->setItem(i, 0, new QTableWidgetItem(doctors[i]->getId()));
        ui->doctorsTable->setItem(i, 1, new QTableWidgetItem(QString("%1 %2").arg(doctors[i]->getName(), doctors[i]->getSurname())));
    }
    // make the table full width
    ui->doctorsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // make the table row-selectable
    ui->doctorsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    // make table non editable
    ui->doctorsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void MainWindow::updatePatients() {
    // delete all patients from table
    ui->patientsTable->setRowCount(0);
    // populate patients table (first column is id, second is full name)
    ui->patientsTable->setRowCount(patients.size());
    ui->patientsTable->setColumnCount(2);
    ui->patientsTable->setHorizontalHeaderLabels(QStringList() << "ID" << "Full name");
    for (int i = 0; i < patients.size(); i++) {
        ui->patientsTable->setItem(i, 0, new QTableWidgetItem(patients[i]->getId()));
        ui->patientsTable->setItem(i, 1, new QTableWidgetItem(QString("%1 %2").arg(patients[i]->getName(), patients[i]->getSurname())));
    }
    // make the table full width
    ui->patientsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // make the table row-selectable
    ui->patientsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    // make table non editable
    ui->patientsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void MainWindow::updateAppointments() {
    // delete all appointments from table
    ui->appointmentsTable->setRowCount(0);
    // populate appointments table (first column is patient full name, second is doctor full name, third is date and time)
    ui->appointmentsTable->setRowCount(appointments.size());
    ui->appointmentsTable->setColumnCount(3);
    ui->appointmentsTable->setHorizontalHeaderLabels(QStringList() << "Patient" << "Doctor" << "Date and time");
    for (int i = 0; i < appointments.size(); i++) {
        auto patientName = QString("%1 %2").arg(appointments[i]->getPatient()->getName(), appointments[i]->getPatient()->getSurname());
        auto doctorName = QString("%1 %2").arg(appointments[i]->getDoctor()->getName(), appointments[i]->getDoctor()->getSurname());
        ui->appointmentsTable->setItem(i, 0, new QTableWidgetItem(patientName));
        ui->appointmentsTable->setItem(i, 1, new QTableWidgetItem(doctorName));
        ui->appointmentsTable->setItem(i, 2, new QTableWidgetItem(appointments[i]->getDateTime().toString("yyyy-MM-dd hh:mm:ss")));
    }
    // make the table full width
    ui->appointmentsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // make the table row-selectable
    ui->appointmentsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    // make table non editable
    ui->appointmentsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void MainWindow::on_add_doctor_button_clicked() {
    auto dialog = new AddDoctorDialog(this);
    connect(dialog, &AddDoctorDialog::doctorChanged, [this, dialog](const std::shared_ptr<Doctor>& doctor) {
        doctors.append(doctor);
        updateDoctors();
        // dismiss the dialog
        dialog->close();
        delete dialog;
    });
    dialog->exec();
}

void MainWindow::on_edit_doctor_button_clicked() {
    // get the selected doctor
    auto selectedDoctor = ui->doctorsTable->selectedItems();
    if (selectedDoctor.size() == 0) {
        return;
    }
    auto selectedDoctorId = selectedDoctor[0]->text();
    std::shared_ptr<Doctor> oldDoctor = nullptr;
    for (auto &d : doctors) {
        if (d->getId() == selectedDoctorId) {
            oldDoctor = d;
            break;
        }
    }
    if (!oldDoctor) {
        return;
    }
    // show the dialog
    auto dialog = new AddDoctorDialog(oldDoctor, this);
    connect(dialog, &AddDoctorDialog::doctorChanged, [this, dialog, oldDoctor](const std::shared_ptr<Doctor>& doctor) {
        // update the doctor
        for (int i = 0; i < doctors.size(); i++) {
            if (doctors[i]->getId() == oldDoctor->getId()) {
                doctors[i] = doctor;
                break;
            }
        }
        updateDoctors();
        // dismiss the dialog
        dialog->close();
        delete dialog;
    });
    dialog->exec();
}

void MainWindow::on_delete_doctor_button_clicked() {
    // get the selected doctor
    auto selectedDoctor = ui->doctorsTable->selectedItems();
    if (selectedDoctor.size() == 0) {
        return;
    }
    auto selectedDoctorId = selectedDoctor[0]->text();
    for (auto &d: doctors) {
        if (d->getId() == selectedDoctorId) {
            doctors.removeOne(d);
        }
    }
    updateDoctors();
}

void MainWindow::on_add_patient_button_clicked() {
    auto dialog = new AddPatientDialog(this);
    connect(dialog, &AddPatientDialog::patientChanged, [this, dialog](const std::shared_ptr<Patient>& patient) {
        patients.append(patient);
        updatePatients();
        // dismiss the dialog
        dialog->close();
        delete dialog;
    });
    dialog->exec();
}

void MainWindow::on_edit_patient_button_clicked() {
    auto selectedPatient = ui->patientsTable->selectedItems();
    if (selectedPatient.size() == 0) {
        return;
    }
    auto selectedPatientId = selectedPatient[0]->text();
    std::shared_ptr<Patient> oldPatient = nullptr;
    for (auto &p : patients) {
        if (p->getId() == selectedPatientId) {
            oldPatient = p;
            break;
        }
    }
    if (!oldPatient) {
        return;
    }
    auto dialog = new AddPatientDialog(oldPatient, this);
    connect(dialog, &AddPatientDialog::patientChanged, [this, dialog, oldPatient](const std::shared_ptr<Patient>& patient) {
        // update the patient
        for (int i = 0; i < patients.size(); i++) {
            if (patients[i]->getId() == oldPatient->getId()) {
                patients[i] = patient;
                break;
            }
        }
        updatePatients();
        // dismiss the dialog
        dialog->close();
        delete dialog;
    });
    dialog->exec();
}

void MainWindow::on_delete_patient_button_clicked() {
    auto selectedPatient = ui->patientsTable->selectedItems();
    if (selectedPatient.size() == 0) {
        return;
    }
    auto selectedPatientId = selectedPatient[0]->text();
    for (auto &p: patients) {
        if (p->getId() == selectedPatientId) {
            patients.removeOne(p);
        }
    }
    updatePatients();
}

void MainWindow::on_add_appointment_button_clicked() {
    auto dialog = new AddAppointmentDialog(doctors, patients, this);
    connect(dialog, &AddAppointmentDialog::appointmentChanged, [this, dialog](const std::shared_ptr<Appointment>& appointment) {
        // check whether the appointment is conflicting with another appointment (for doctor)
        for (auto &a : appointments) {
            auto appointmentStart = appointment->getDateTime();
            if (a->getDoctor()->getId() == appointment->getDoctor()->getId() && a->getDateTime().secsTo(appointmentStart) >= 0 && a->getDateTime().secsTo(appointmentStart) < a->getDuration() * 3600) {
                QMessageBox::critical(this, "Error", "The doctor is not available at the selected time.");
                return;
            }
        }
        // check whether the appointment is conflicting with another appointment (for patient)
        for (auto &a : appointments) {
            auto appointmentStart = appointment->getDateTime();
            if (a->getPatient()->getId() == appointment->getPatient()->getId() && a->getDateTime().secsTo(appointmentStart) >= 0 && a->getDateTime().secsTo(appointmentStart) < a->getDuration() * 3600) {
                QMessageBox::critical(this, "Error", "The patient is not available at the selected time.");
                return;
            }
        }
        appointments.append(appointment);
        updateAppointments();
        // dismiss the dialog
        dialog->close();
        delete dialog;
    });
    dialog->exec();
}

void MainWindow::on_edit_appointment_button_clicked() {
    // get the selected appointment
    auto selectedAppointment = ui->appointmentsTable->selectedItems();
    if (selectedAppointment.size() == 0) {
        return;
    }
    auto selectedAppointmentPatientName = selectedAppointment[0]->text();
    auto selectedAppointmentDoctorName = selectedAppointment[1]->text();
    auto selectedAppointmentDateTime = selectedAppointment[2]->text();
    std::shared_ptr<Appointment> oldAppointment = nullptr;
    for (auto &a : appointments) {
        auto patientName = QString("%1 %2").arg(a->getPatient()->getName(), a->getPatient()->getSurname());
        auto doctorName = QString("%1 %2").arg(a->getDoctor()->getName(), a->getDoctor()->getSurname());
        auto dateTime = a->getDateTime().toString("yyyy-MM-dd hh:mm:ss");
        if (patientName == selectedAppointmentPatientName && doctorName == selectedAppointmentDoctorName && dateTime == selectedAppointmentDateTime) {
            oldAppointment = a;
            break;
        }
    }
    if (!oldAppointment) {
        return;
    }
    // show the dialog
    auto dialog = new AddAppointmentDialog(doctors, patients, oldAppointment->getDoctor(), oldAppointment->getPatient(), oldAppointment->getDateTime(), oldAppointment->getDuration(), this);
    connect(dialog, &AddAppointmentDialog::appointmentChanged, [this, dialog, oldAppointment](const std::shared_ptr<Appointment>& appointment) {
        // update the appointment
        for (int i = 0; i < appointments.size(); i++) {
            auto patientName = QString("%1 %2").arg(appointments[i]->getPatient()->getName(), appointments[i]->getPatient()->getSurname());
            auto doctorName = QString("%1 %2").arg(appointments[i]->getDoctor()->getName(), appointments[i]->getDoctor()->getSurname());
            auto id = appointments[i]->getId();
            auto dateTime = appointments[i]->getDateTime().toString("yyyy-MM-dd hh:mm:ss");
            if(appointments[i]->getId() == oldAppointment->getId()) {
                // check whether the appointment is conflicting with another appointment (for doctor)
                for (auto &a : appointments) {
                    auto appointmentStart = appointment->getDateTime();
                    if (a->getDoctor()->getId() == appointment->getDoctor()->getId() && a->getDateTime().secsTo(appointmentStart) >= 0 && a->getDateTime().secsTo(appointmentStart) < a->getDuration() * 3600 && a->getId() != id) {
                        QMessageBox::critical(this, "Error", "The doctor is not available at the selected time.");
                        return;
                    }
                }
                // check whether the appointment is conflicting with another appointment (for patient)
                for (auto &a : appointments) {
                    auto appointmentStart = appointment->getDateTime();
                    if (a->getPatient()->getId() == appointment->getPatient()->getId() && a->getDateTime().secsTo(appointmentStart) >= 0 && a->getDateTime().secsTo(appointmentStart) < a->getDuration() * 3600 && a->getId() != id) {
                        QMessageBox::critical(this, "Error", "The patient is not available at the selected time.");
                        return;
                    }
                }
                appointments[i] = appointment;
                break;
            }
        }
        updateAppointments();
        // dismiss the dialog
        dialog->close();
        delete dialog;
    });
    dialog->exec();
}

void MainWindow::on_delete_appointment_button_clicked() {
    // get the selected appointment
    auto selectedAppointment = ui->appointmentsTable->selectedItems();
    if (selectedAppointment.size() == 0) {
        return;
    }
    auto selectedAppointmentPatientName = selectedAppointment[0]->text();
    auto selectedAppointmentDoctorName = selectedAppointment[1]->text();
    auto selectedAppointmentDateTime = selectedAppointment[2]->text();
    for (auto &a : appointments) {
        auto patientName = QString("%1 %2").arg(a->getPatient()->getName(), a->getPatient()->getSurname());
        auto doctorName = QString("%1 %2").arg(a->getDoctor()->getName(), a->getDoctor()->getSurname());
        auto dateTime = a->getDateTime().toString("yyyy-MM-dd hh:mm:ss");
        if (patientName == selectedAppointmentPatientName && doctorName == selectedAppointmentDoctorName && dateTime == selectedAppointmentDateTime) {
            appointments.removeOne(a);
        }
    }
    updateAppointments();
}

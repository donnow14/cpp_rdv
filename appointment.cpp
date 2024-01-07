#include "appointment.h"
#include <QUuid>

Appointment::Appointment(std::shared_ptr<Patient> patient, std::shared_ptr<Doctor> doctor, const QDateTime& dateTime, int duration) {
    this->patient = patient;
    this->doctor = doctor;
    this->dateTime = dateTime;
    this->duration = duration;
    // randomly generate id
    this->id = QUuid::createUuid().toString();
}

std::shared_ptr<Patient> Appointment::getPatient() const {
    return patient;
}

std::shared_ptr<Doctor> Appointment::getDoctor() const {
    return doctor;
}

QDateTime Appointment::getDateTime() const {
    return dateTime;
}

int Appointment::getDuration() const {
    return duration;
}

QString Appointment::getId() const {
    return id;
}

void Appointment::setPatient(std::shared_ptr<Patient> patient) {
    this->patient = patient;
}

void Appointment::setDoctor(std::shared_ptr<Doctor> doctor) {
    this->doctor = doctor;
}

void Appointment::setDateTime(const QDateTime& dateTime) {
    this->dateTime = dateTime;
}

void Appointment::setDuration(int duration) {
    this->duration = duration;
}

void Appointment::setId(const QString& id) {
    this->id = id;
}

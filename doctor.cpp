#include "doctor.h"

Doctor::Doctor(const QString& name, const QString& surname, const QString& specialization, const QString& email, const QString& id) {
    this->name = name;
    this->surname = surname;
    this->specialization = specialization;
    this->email = email;
    this->id = id;
}

QString Doctor::getName() const {
    return name;
}

QString Doctor::getSurname() const {
    return surname;
}

QString Doctor::getSpecialization() const {
    return specialization;
}

QString Doctor::getEmail() const {
    return email;
}

QString Doctor::getId() const {
    return id;
}

void Doctor::setName(const QString& name) {
    this->name = name;
}

void Doctor::setSurname(const QString& surname) {
    this->surname = surname;
}

void Doctor::setSpecialization(const QString& specialization) {
    this->specialization = specialization;
}

void Doctor::setEmail(const QString& email) {
    this->email = email;
}

void Doctor::setId(const QString& id) {
    this->id = id;
}

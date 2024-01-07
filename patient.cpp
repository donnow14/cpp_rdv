#include "patient.h"

Patient::Patient(const QString& name, const QString& surname, const QString& email, const QString& phone, const QString& id) {
    this->name = name;
    this->surname = surname;
    this->email = email;
    this->phone = phone;
    this->id = id;
}

QString Patient::getName() const {
    return name;
}

QString Patient::getSurname() const {
    return surname;
}

QString Patient::getEmail() const {
    return email;
}

QString Patient::getPhone() const {
    return phone;
}

QString Patient::getId() const {
    return id;
}

void Patient::setName(const QString& name) {
    this->name = name;
}

void Patient::setSurname(const QString& surname) {
    this->surname = surname;
}

void Patient::setEmail(const QString& email) {
    this->email = email;
}

void Patient::setPhone(const QString& phone) {
    this->phone = phone;
}

void Patient::setId(const QString& id) {
    this->id = id;
}

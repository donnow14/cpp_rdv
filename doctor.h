#ifndef DOCTOR_H
#define DOCTOR_H

#include <QString>

class Doctor {
private:
    QString name;
    QString surname;
    QString specialization;
    QString email;
    QString id;
public:
    Doctor(const QString& name, const QString& surname, const QString& specialization, const QString& email, const QString& id);
    QString getName() const;
    QString getSurname() const;
    QString getSpecialization() const;
    QString getEmail() const;
    QString getId() const;
    void setName(const QString& name);
    void setSurname(const QString& surname);
    void setSpecialization(const QString& specialization);
    void setEmail(const QString& email);
    void setId(const QString& id);
};

#endif // DOCTOR_H

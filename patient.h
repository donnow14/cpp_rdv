#ifndef PATIENT_H
#define PATIENT_H

#include <QString>
#include <QCloseEvent>

class Patient {
private:
    QString name;
    QString surname;
    QString email;
    QString phone;
    QString id;
public:
    Patient(const QString& name, const QString& surname, const QString& email, const QString& phone, const QString& id);
    QString getName() const;
    QString getSurname() const;
    QString getEmail() const;
    QString getPhone() const;
    QString getId() const;
    void setName(const QString& name);
    void setSurname(const QString& surname);
    void setEmail(const QString& email);
    void setPhone(const QString& phone);
    void setId(const QString& id);
};

#endif // PATIENT_H

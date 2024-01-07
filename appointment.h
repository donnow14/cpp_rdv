#ifndef APPOINTMENT_H
#define APPOINTMENT_H

#include <memory>
#include <QDateTime>
#include "patient.h"
#include "doctor.h"

class Appointment {
private:
    std::shared_ptr<Patient> patient;
    std::shared_ptr<Doctor> doctor;
    QDateTime dateTime;
    QString id;
    int duration;
public:
    Appointment(std::shared_ptr<Patient> patient, std::shared_ptr<Doctor> doctor, const QDateTime& dateTime, int duration);
    std::shared_ptr<Patient> getPatient() const;
    std::shared_ptr<Doctor> getDoctor() const;
    QDateTime getDateTime() const;
    int getDuration() const;
    QString getId() const;
    void setPatient(std::shared_ptr<Patient> patient);
    void setDoctor(std::shared_ptr<Doctor> doctor);
    void setDateTime(const QDateTime& dateTime);
    void setDuration(int duration);
    void setId(const QString& id);
};

#endif // APPOINTMENT_H

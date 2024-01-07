#ifndef ADDAPPOINTMENTDIALOG_H
#define ADDAPPOINTMENTDIALOG_H

#include <QDialog>
#include <QVector>
#include <memory>
#include <doctor.h>
#include <patient.h>
#include <appointment.h>

namespace Ui {
class AddAppointmentDialog;
}

class AddAppointmentDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddAppointmentDialog(const QVector<std::shared_ptr<Doctor>>& doctors, const QVector<std::shared_ptr<Patient>>& patients, QWidget *parent = nullptr);
    AddAppointmentDialog(const QVector<std::shared_ptr<Doctor>>& doctors, const QVector<std::shared_ptr<Patient>>& patients, const std::shared_ptr<Doctor>& doctor, const std::shared_ptr<Patient>& patient, const QDateTime& dateTime, int duration, QWidget *parent = nullptr);
    ~AddAppointmentDialog();

signals:
    void appointmentChanged(const std::shared_ptr<Appointment>& appointment);

private:
    Ui::AddAppointmentDialog *ui;
    QVector<std::shared_ptr<Doctor>> doctors;
    QVector<std::shared_ptr<Patient>> patients;

private slots:
    void on_add_button_clicked();
};

#endif // ADDAPPOINTMENTDIALOG_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QVector>
#include <QSqlDatabase>
#include <memory>
#include "doctor.h"
#include "patient.h"
#include "appointment.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QVector<std::shared_ptr<Doctor>> doctors;
    QVector<std::shared_ptr<Patient>> patients;
    QVector<std::shared_ptr<Appointment>> appointments;

    QThread* appointmentThread;

    QSqlDatabase db;

    void closeEvent(QCloseEvent *event) override;

    void updateDoctors();
    void updatePatients();
    void updateAppointments();

signals:
    void desktopNotification(const QString& title, const QString& message);

private slots:
    void on_add_appointment_button_clicked();
    void on_edit_appointment_button_clicked();
    void on_delete_appointment_button_clicked();

    void on_add_doctor_button_clicked();
    void on_edit_doctor_button_clicked();
    void on_delete_doctor_button_clicked();

    void on_add_patient_button_clicked();
    void on_edit_patient_button_clicked();
    void on_delete_patient_button_clicked();

    void sendDesktopNotification(const QString& title, const QString& message);
};
#endif // MAINWINDOW_H

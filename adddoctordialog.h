#ifndef ADDDOCTORDIALOG_H
#define ADDDOCTORDIALOG_H

#include "doctor.h"
#include <QDialog>
#include <memory>

namespace Ui {
class AddDoctorDialog;
}

class AddDoctorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddDoctorDialog(QWidget *parent = nullptr);
    AddDoctorDialog(const std::shared_ptr<Doctor>& doctor, QWidget *parent = nullptr);
    ~AddDoctorDialog();

signals:
    void doctorChanged(const std::shared_ptr<Doctor>& doctor);

private slots:
    void on_addDoctorButton_clicked();

private:
    Ui::AddDoctorDialog *ui;
};

#endif // ADDDOCTORDIALOG_H

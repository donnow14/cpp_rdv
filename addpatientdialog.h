#ifndef ADDPATIENTDIALOG_H
#define ADDPATIENTDIALOG_H

#include <QDialog>
#include <memory>
#include <patient.h>

namespace Ui {
class AddPatientDialog;
}

class AddPatientDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddPatientDialog(QWidget *parent = nullptr);
    AddPatientDialog(const std::shared_ptr<Patient>& patient, QWidget *parent = nullptr);
    ~AddPatientDialog();

signals:
    void patientChanged(const std::shared_ptr<Patient>& patient);

private slots:
    void on_addPatientButton_clicked();

private:
    Ui::AddPatientDialog *ui;
};

#endif // ADDPATIENTDIALOG_H

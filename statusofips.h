#ifndef STATUSOFIPS_H
#define STATUSOFIPS_H

#include <QDialog>

namespace Ui {
class statusOfIPs;
}

class statusOfIPs : public QDialog
{
    Q_OBJECT

public:
    int totalIps;
    explicit statusOfIPs(QWidget *parent = nullptr);
    ~statusOfIPs();

private:
    Ui::statusOfIPs *ui;

private slots:
    void totalSuccessfulConnSlot(int pc,int totalNoOfIps);
    void on_pushButton_clicked();
    void closeWindow();

signals:
    void interruptOccurSignal();
};

#endif // STATUSOFIPS_H

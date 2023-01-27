#ifndef DIALOGFOREDITBTN_H
#define DIALOGFOREDITBTN_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QtSql>

namespace Ui {
class dialogForEditBtn;
}

class dialogForEditBtn : public QDialog
{
    Q_OBJECT

public:
    QSqlDatabase deviceInfoData;
    explicit dialogForEditBtn(QWidget *parent = nullptr);
    ~dialogForEditBtn();

private slots:
    void on_pushButton_clicked();

private:
    Ui::dialogForEditBtn *ui;
};

#endif // DIALOGFOREDITBTN_H


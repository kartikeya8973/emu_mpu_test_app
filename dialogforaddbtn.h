#ifndef DIALOGFORADDBTN_H
#define DIALOGFORADDBTN_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QtSql>
#include <QMessageBox>


namespace Ui {
class dialogForAddBtn;
}

class dialogForAddBtn : public QDialog
{
    Q_OBJECT

public:
    QSqlDatabase deviceInfoData;
    explicit dialogForAddBtn(QWidget *parent = nullptr);
    ~dialogForAddBtn();

public slots:
    void on_pushButton_clicked();




private:
    Ui::dialogForAddBtn *ui;
};

#endif // DIALOGFORADDBTN_H

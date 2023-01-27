#ifndef DIALOGFORDELETEBTN_H
#define DIALOGFORDELETEBTN_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QtSql>
#include <QTimer>

namespace Ui {

class dialogForDeleteBtn;

}

class dialogForDeleteBtn : public QDialog
{
    Q_OBJECT

public:
    QSqlDatabase deviceInfoData;

    explicit dialogForDeleteBtn(QWidget *parent = nullptr);
    ~dialogForDeleteBtn();

private slots:
    void on_pushButton_clicked();

private:
    Ui::dialogForDeleteBtn *ui;


};

#endif // DIALOGFORDELETEBTN_H

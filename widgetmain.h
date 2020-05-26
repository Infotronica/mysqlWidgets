#ifndef WIDGETMAIN_H
#define WIDGETMAIN_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>
#include <QDate>
#include <QRegExpValidator>
#include <QMovie>

namespace Ui {
class WidgetMain;
}

class WidgetMain : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetMain(QWidget *parent = 0);
    ~WidgetMain();

    QString urlBase,folderWS,urlFolder,urlWS;
    QString bdMain,usrBD,pwdBD,archivoWS;
    QString iRecord,firstName,lastName,birthDate,errorSQL;
    QRegExpValidator regExpValidatorAlfanumeric;
    QFont fontUpper;
    QMovie movieWait;

    void inicializeApp();
    void msgOk(QString msg);
    int msgYesNo(QString msg);
    QByteArray doSQLQuery(QString sql);
    QString sqlFilter(QString str);
    void listRecords();
    void newRecord();
    void addRecord();
    void updateRecord();
    void saveRecord();
    void startBusy();
    void stopBusy();

private slots:
    void on_btnUpdateList_clicked();

    void on_lineEditFilter_returnPressed();

    void on_tableWidget_doubleClicked(const QModelIndex &index);

    void on_btnSave_clicked();

    void on_btnNew_clicked();

private:
    Ui::WidgetMain *ui;
};

#endif // WIDGETMAIN_H

#include "widgetmain.h"
#include "ui_widgetmain.h"

WidgetMain::WidgetMain(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetMain)
{
    ui->setupUi(this);
}

WidgetMain::~WidgetMain()
{
    delete ui;
}

void WidgetMain::inicializeApp()
{
    /*
        archivoWS name of the intermediary file between this application and the database
        urlBase - The url base of server
        folderWS - the remote folder containing the webservice.php file
        bdMain - The name of the database
        usrBD - the user who can use the database
        pwdBD - the password of the user who can use the database
    */

    archivoWS="webservice.php";
    urlBase="http://localhost";
    folderWS="webservice";
    bdMain="database_names";
    usrBD="root";
    pwdBD="";

    /*
        urlFolder = "http://localhost/webservice/"
        urlWS = "http://localhost/webservice/webservice.php"
     */

    urlFolder=urlBase+"/"+folderWS+"/";
    urlWS=urlFolder+archivoWS;

    regExpValidatorAlfanumeric.setRegExp(QRegExp("[a-zA-Z0123456789ñÑáéíóúÁÉÍÓÚüÜ_@#$& -]{1,}"));
    ui->lineEditFirstName->setValidator(&regExpValidatorAlfanumeric);
    ui->lineEditLastName->setValidator(&regExpValidatorAlfanumeric);
    ui->lineEditFilter->setValidator(&regExpValidatorAlfanumeric);

    errorSQL="error sql";
    movieWait.setFileName(":/vid/lg.messenger-typing-preloader.gif");
    ui->labelMovie->setMovie(&movieWait);

    fontUpper=font();
    fontUpper.setCapitalization(QFont::AllUppercase);
    ui->lineEditFirstName->setFont(fontUpper);
    ui->lineEditLastName->setFont(fontUpper);
    ui->lineEditFilter->setFont(fontUpper);
}

void WidgetMain::msgOk(QString msg)
{
    QMessageBox msgBox;

    msgBox.setText(msg);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.addButton("Ok",QMessageBox::YesRole);
    msgBox.setDefaultButton(QMessageBox::Yes);
    msgBox.exec();
}

int WidgetMain::msgYesNo(QString msg) // show a message asking YES or NO
{
    QMessageBox msgBox;
    int mb;

    msgBox.setText(msg);
    msgBox.setIcon(QMessageBox::Question);
    msgBox.addButton("No",QMessageBox::NoRole); // return 0
    msgBox.addButton("Yes",QMessageBox::YesRole); // return 1
    msgBox.setDefaultButton(QMessageBox::Yes);
    mb=msgBox.exec();
    return mb;
}

QByteArray WidgetMain::doSQLQuery(QString sql)
{
    QNetworkAccessManager networkManager;
    QNetworkReply *networkReply;
    QNetworkRequest networkRequest;
    QByteArray postData;
    QUrlQuery urlQuery;
    QEventLoop qEventLoop;

    /*
        This function makes sql requests to the database server, the way to do it is
        accessing a file called webservice.php, to which 4 data are sent, which are the following:
            1.- The sql statement in the "sql" variable
            2.- The name of the database in the variable "bdMain"
            3.- The name of the user who can use the database in the variable "usrBD"
            4.- The user's password in the variable "pwdBD"

        urlWS contains the address to the webservice.php file, in this case its value is
        "http://localhost/webservice/webservice.php"
    */

    networkRequest.setUrl(QUrl(urlWS));
    urlQuery.addQueryItem("sql",sql); // the "urlQuery" object stores the 4 parameters sent to webservice.php
    urlQuery.addQueryItem("bd",bdMain);
    urlQuery.addQueryItem("usr_bd",usrBD);
    urlQuery.addQueryItem("pwd_bd",pwdBD);
    postData.append(urlQuery.toString()); // pass parameters to a set of bytes

    // The networkRequest variable does sets the format of the data exchange and its length
    networkRequest.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded;charset=UTF-8");
    networkRequest.setHeader(QNetworkRequest::ContentLengthHeader,postData.length());

    /*
        Here we contact webservice.php passing the 4 parameters to make the query
        in the database, the variable "networkReply" receives the information that the file returns
        webservice.php
    */
    networkReply = networkManager.post(networkRequest,postData);

    /*
        Wait for the webservice.php to finish its work WITHOUT interrupting the fluidity of the application, that is, the
        application does not freeze while waiting for the completion of this operation
    */
    connect(networkReply,SIGNAL(finished()),&qEventLoop,SLOT(quit()));
    qEventLoop.exec();
    disconnect(networkReply,SIGNAL(finished()),&qEventLoop,SLOT(quit()));

    postData=networkReply->readAll(); // Receive the data provided by webservice.php
    delete networkReply; // Delete "networkReply" object

    return postData;
}

QString WidgetMain::sqlFilter(QString str)
{
    str.remove(QRegExp("[´`'\"]")); // preventing this characters on a string
    return str;
}

void WidgetMain::listRecords()
{
    QByteArray qByteArray;
    QString sql,qString,filter;
    QStringList qStringList;
    QJsonDocument jsonDocument;
    QJsonArray jsonArray;
    QJsonObject jsonObject;
    int j,rowCount;

    startBusy();
    filter=ui->lineEditFilter->text();
    filter=sqlFilter(filter);
    sql="select * from `list names` where ";
    sql+=" `first name` like '%"+filter+"%' or ";
    sql+=" `last name` like '%"+filter+"%' or ";
    sql+=" `birth date` like '%"+filter+"%'";
    qByteArray=doSQLQuery(sql);

    qString=QString::fromUtf8(qByteArray); // copy to a QString
    ui->plainTextEdit->setPlainText(qString);

    ui->tableWidget->setColumnCount(4); // we need three columns
    qStringList << "INDEX" << "FIRST NAME" << "LAST NAME" << "BIRTH DATE"; // column headers
    ui->tableWidget->setHorizontalHeaderLabels(qStringList); // set the column headers
    ui->tableWidget->setColumnWidth(0,50); // width of first column
    ui->tableWidget->setColumnWidth(1,250); // width of second column
    ui->tableWidget->setColumnWidth(2,250); // width of second column
    ui->tableWidget->setColumnWidth(3,100); // width of second column

    jsonDocument = QJsonDocument::fromJson(qByteArray); // put the returned data in a jsonDocument object
    jsonArray=jsonDocument.array(); // parse to QJsonArray object
    if (jsonArray.isEmpty())
    {
        stopBusy();
        return;
    }

    rowCount=jsonArray.count(); // number of elements in json array
    ui->tableWidget->setRowCount(rowCount); // create the items on tableWidget
    for (j=0; j<rowCount; j++)
    {
        jsonObject=jsonArray.at(j).toObject(); // json array element to json object
        qString=jsonObject.value("idName").toString(); // get the request data
        ui->tableWidget->setItem(j,0,new QTableWidgetItem(qString)); // set the cell item

        qString=jsonObject.value("first name").toString(); // get the request data
        ui->tableWidget->setItem(j,1,new QTableWidgetItem(qString)); // set the cell item

        qString=jsonObject.value("last name").toString(); // get the request data
        ui->tableWidget->setItem(j,2,new QTableWidgetItem(qString)); // set the cell item

        qString=jsonObject.value("birth date").toString(); // get the request data
        ui->tableWidget->setItem(j,3,new QTableWidgetItem(qString)); // set the cell item
    }

    stopBusy();
}

void WidgetMain::newRecord()
{
    iRecord="";
    firstName="";
    lastName="";
    birthDate="";

    ui->lineEditFirstName->setText(firstName);
    ui->lineEditLastName->setText(lastName);
    ui->dateEdit->setDate(QDate::currentDate());
    ui->lineEditFirstName->setFocus();
}

void WidgetMain::addRecord()
{
    QByteArray postData;
    QString sql;

    startBusy();
    sql="insert into `list names` set ";
    sql+="`first name`='"+firstName+"',";
    sql+="`last name`='"+lastName+"',";
    sql+="`birth date`='"+birthDate+"'";
    postData=doSQLQuery(sql);
    stopBusy();

    iRecord=QString::fromUtf8(postData);
    if (iRecord==errorSQL)
    {
        iRecord="";
        msgOk("Error adding record");
        return;
    }
    listRecords();
}

void WidgetMain::updateRecord()
{
    QByteArray postData;
    QString sql,qString;

    startBusy();
    sql="update `list names` set ";
    sql+="`first name`='"+firstName+"',";
    sql+="`last name`='"+lastName+"',";
    sql+="`birth date`='"+birthDate+"' ";
    sql+=" where idName="+iRecord;
    postData=doSQLQuery(sql);
    stopBusy();

    qString=QString::fromUtf8(postData);
    if (qString==errorSQL)
    {
        msgOk("Error updating record");
        return;
    }
    listRecords();
}

void WidgetMain::saveRecord()
{
    if (ui->lineEditFirstName->text()=="")
    {
        msgOk("Must type the first name");
        return;
    }
    if (ui->lineEditLastName->text()=="")
    {
        msgOk("Must type the last name");
        return;
    }

    firstName=sqlFilter(ui->lineEditFirstName->text().trimmed().toUpper());
    lastName=sqlFilter(ui->lineEditLastName->text().trimmed().toUpper());
    birthDate=ui->dateEdit->date().toString("dd-MM-yyyy");

    ui->lineEditFirstName->setText(firstName);
    ui->lineEditLastName->setText(lastName);

    if (iRecord=="")
    {
        addRecord();
    }
    else
    {
        updateRecord();
    }
}

void WidgetMain::startBusy()
{
    movieWait.start();
    ui->labelMovie->show();
}

void WidgetMain::stopBusy()
{
    movieWait.stop();
    ui->labelMovie->hide();
}

void WidgetMain::on_btnUpdateList_clicked()
{
    listRecords();
}

void WidgetMain::on_lineEditFilter_returnPressed()
{
    listRecords();
}

void WidgetMain::on_tableWidget_doubleClicked(const QModelIndex &index)
{
    int selectedRow;

    selectedRow=index.row();
    if (selectedRow==-1)
    {
        return;
    }

    iRecord=ui->tableWidget->item(selectedRow,0)->text();
    firstName=ui->tableWidget->item(selectedRow,1)->text();
    lastName=ui->tableWidget->item(selectedRow,2)->text();
    birthDate=ui->tableWidget->item(selectedRow,3)->text();

    ui->lineEditFirstName->setText(firstName);
    ui->lineEditLastName->setText(lastName);

    if (birthDate=="")
    {
        ui->dateEdit->setDate(QDate::currentDate());
    }
    else
    {
        ui->dateEdit->setDate(QDate::fromString(birthDate, "dd-MM-yyyy"));
    }
}

void WidgetMain::on_btnSave_clicked()
{
    saveRecord();
}

void WidgetMain::on_btnNew_clicked()
{
    newRecord();
}

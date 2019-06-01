#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    binacpp = new Binacpp(this);
    MessageProcess msgProc;
    if(!loadConfig(msgProc)){
        qDebug() << msgProc.msg;
    } else
        qDebug() << "config load success";
    binacpp->setKeys(userData.secret_key, userData.api_key);
    ui->dtEndHistory->setDateTime(QDateTime::currentDateTime());

    QDateTime dtime;
    dtime.setSecsSinceEpoch(QDateTime::currentSecsSinceEpoch());
    QTextCharFormat charFormat = ui->textEdit->currentCharFormat() ;
    charFormat.setFontStyleHint(QFont::Monospace);
    ui->textEdit->setCurrentCharFormat(charFormat);

    QString themes = "color: #929292; "
                              "selection-color: green;"
                              "selection-background-color: black;"
                              "background-color: #323232";
    this->setStyleSheet(themes);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QByteArray MainWindow::downloadCandle(QString dtStart, QString dtEnd, QString req)
{
    QString request = req;
    request.append("&startTime=");
    request.append(dtStart);
    request.append("&endTime=");
    request.append(dtEnd);
    QEventLoop loop;
    QNetworkReply *reply;
    QByteArray buffer;
    QNetworkAccessManager *n_man = new QNetworkAccessManager(this);
    reply = n_man->get(QNetworkRequest(QUrl(request)));
    if(reply == nullptr)
    {
        buffer.append("reply is NULL");
        qDebug() << buffer;
    }
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    if(reply->error() == QNetworkReply::NoError)
    {
        buffer = reply->readAll();

    } else {
        buffer = reply->errorString().toUtf8() + " reply error";
    }
    delete reply;
    delete n_man;
    return buffer;
}

QString MainWindow::getQuoteAsset(QString a_pair)
{
    QString quote;
    if(a_pair.contains(QRegExp("ETH$")))
        quote = "ETH";
    else if(a_pair.contains(QRegExp("BTC$")))
        quote = "BTC";
    else if(a_pair.contains(QRegExp("USDT$")))
        quote = "USDT";
    else if(a_pair.contains(QRegExp("BNB$")))
        quote = "BNB";
    else
    {
        quote = "non";
        qDebug() << "Error: does not exist BASE asset";
    }
    return quote;
}

QString MainWindow::getBaseAsset(QString a_pair)
{
    return a_pair.remove(QRegExp(getQuoteAsset(a_pair)));
}

void MainWindow::on_pushButton_7_clicked()
{
    ui->textEdit->clear();
}

bool MainWindow::loadConfig(MessageProcess &msgProc)
{
    QString pathConfig;
#if defined(Q_OS_WIN)
    pathConfig = "config.json";
#elif defined(Q_OS_LINUX)
    pathConfig = "./config.json";
#endif
    QFile fileConfig(pathConfig);
    if(!fileConfig.open(QFile::ReadOnly | QFile::Text))
    {
        msgProc.msg = "Not open file config for loading";
        msgProc.result = false;
        return false;
    }
    QJsonDocument jdoc = QJsonDocument::fromJson(fileConfig.readAll());
    fileConfig.close();
    QJsonObject obj = jdoc.object();
    userData.api_key = obj["api_key"].toString();
    userData.secret_key = obj["secret_key"].toString();
    ui->leApiKey->setText(userData.api_key);
    ui->leSecretKey->setText(userData.secret_key);
    return true;
}

void MainWindow::getAllTradesOfPair()
{
    ui->textEdit->clear();
    tradesList.clear();
    filledTrades.clear();
    qint64 sTime = ui->dtStartHistory->dateTime().toMSecsSinceEpoch();
    QJsonParseError parsError;
    QJsonArray jarray;
    do {


        QJsonDocument jdoc = QJsonDocument::fromJson(binacpp->getMyAllTrades(ui->lePair->text(), ui->leCount->text(), QString::number(sTime)), &parsError);
        if(parsError.error != QJsonParseError::NoError)
            break;
        else if(jdoc.array().isEmpty())
            break;

        if(sTime > static_cast <qint64> (jdoc.array()[0].toObject()["time"].toDouble())) {
            qDebug() << "time prep > next";
            return ;
        }
        for(int i = 0; i < jdoc.array().size(); i++)
            jarray.append(jdoc.array()[i].toObject());

        sTime = static_cast <qint64> (jdoc.array()[jdoc.array().size()-1].toObject()["time"].toDouble()) + 1;
        qDebug() << "next stage";

    } while(sTime < ui->dtEndHistory->dateTime().toMSecsSinceEpoch()) ;
    curArray = QJsonDocument(jarray).toJson();
    parsAllMyTrades(curArray);
    qDebug() << "stop";
}

bool MainWindow::parsAllMyTrades(QByteArray array)
{
    QJsonParseError parsError;
    QJsonDocument jdoc = QJsonDocument::fromJson(array, &parsError);
    if(parsError.error != QJsonParseError::NoError) {
        qDebug() << "parseError" << parsError.errorString();
        return false;
    }
    QJsonArray jarray = jdoc.array();
    if(jarray.isEmpty())
        return false;
    for(int i = 0; i < jarray.size(); i++) {
        QJsonObject jobj = jarray[i].toObject();
        TradesPair trPair;
        trPair.symbol = jobj["symbol"].toString();
        trPair.origQty = jobj["origQty"].toString();
        trPair.executedQty = jobj["executedQty"].toString();
        trPair.cummulativeQuoteQty = jobj["cummulativeQuoteQty"].toString();
        trPair.price = jobj["price"].toString();
        if(qFuzzyIsNull(trPair.price.toDouble()))
            trPair.price = QString::number(trPair.cummulativeQuoteQty.toDouble() / trPair.origQty.toDouble(), 'f', 8);
        trPair.status = jobj["status"].toString();
        trPair.side = jobj["side"].toString();
        trPair.time = QDateTime::fromMSecsSinceEpoch(static_cast <qint64> (jobj["time"].toDouble()));
        trPair.isWorking = jobj["isWorking"].toBool();

        tradesList.append(trPair);
    }
    count = 0;
    for(int i = 0; i < tradesList.size(); i++)
    {
        QString type;
        if(tradesList[i].status == "FILLED" || tradesList[i].status == "PARTIALLY_FILLED") {
            ui->textEdit->append(QString("%1 %2 %3 %4 %5 %6 %7")
                                 .arg(count, -5)
                                 .arg(tradesList[i].price, -15)
                                 .arg(tradesList[i].origQty, -15)
                                 .arg(tradesList[i].cummulativeQuoteQty, -15)
                                 .arg(tradesList[i].side, 7)
                                 .arg(tradesList[i].time.toString("dd:MM:yy hh:mm:ss"))
                                 .arg(tradesList[i].status, 10));
            if(tradesList[i].status == "FILLED")
                count++;
            filledTrades.prepend(tradesList[i]);

        } else if(tradesList[i].status != "CANCELED"){
            //qDebug() << tradesList[i].status;
        }
    }
    return true;
}

double MainWindow::parsCurPrice(QByteArray array)
{
    QJsonParseError parsError;
    QJsonDocument document = QJsonDocument::fromJson(array, &parsError);
    if(parsError.error != QJsonParseError::NoError)
    {
        qDebug() << "DataUserStream::sockMsgReceived::parsError = " << parsError.errorString();
        qDebug() << array;
        return 0;
    }
    QJsonObject obj = document.object();

    return (obj["bidPrice"].toString().toDouble() + obj["askPrice"].toString().toDouble()) / 2.;
}

void MainWindow::on_pushButton_2_clicked()
{
    tradesList.clear();
    filledTrades.clear();
}

void MainWindow::on_pushButton_3_clicked()
{
    QFile saveFile(ui->lePair->text());
    if(!saveFile.open(QFile::WriteOnly | QFile::Text)) {
        qDebug() << "not open file to write";
        return;
    }
    saveFile.write(curArray);
    saveFile.close();
    qDebug() << "write success";
}

void MainWindow::on_pushButton_5_clicked()
{
    QString path = ui->lePair->text();
    QFile fileHist(path);
    if(!fileHist.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "not open file to read";
        return ;
    }
    parsAllMyTrades(fileHist.readAll());
    fileHist.close();
    qDebug() << "read success";
}

void MainWindow::on_pushButton_6_clicked()
{
    QString pathConfig;
#if defined(Q_OS_WIN)
    pathConfig = "config.json";
#elif defined(Q_OS_LINUX)
    pathConfig = "./config.json";
#endif
    QFile fileConfig(pathConfig);
    if(!fileConfig.open(QFile::WriteOnly | QFile::Text))
    {
        qDebug() << "Not open file config for loading";
        return;
    }
    QJsonObject jobj;
    jobj["api_key"] = ui->leApiKey->text();
    jobj["secret_key"] = ui->leSecretKey->text();
    QJsonDocument jdoc(jobj);
    fileConfig.write(jdoc.toJson());
    fileConfig.close();
}

void MainWindow::on_pushButton_8_clicked()
{
    QPair<QString, QString> pair;

    QString quote = "0.0";
    QString base = "0.0";
    QString commission = "0.0";
    QString volume = "0.0";

    for(int i = 0; i < filledTrades.size(); i++)
    {
        if(filledTrades[i].side == "BUY") {
            base = QString::number(base.toDouble() + filledTrades[i].origQty.toDouble());
            quote = QString::number(quote.toDouble() - filledTrades[i].cummulativeQuoteQty.toDouble());
            volume = QString::number(volume.toDouble() + filledTrades[i].cummulativeQuoteQty.toDouble());
        } else {
            base = QString::number(base.toDouble() - filledTrades[i].origQty.toDouble());
            quote = QString::number(quote.toDouble() + filledTrades[i].cummulativeQuoteQty.toDouble());
            volume = QString::number(volume.toDouble() + filledTrades[i].cummulativeQuoteQty.toDouble());
        }
    }
    commission = QString::number(volume.toDouble() / 100.0 * 0.075);
    qDebug() << "quote=" << quote << " base=" << base << " commission=" << commission << " volume=" << volume;
    ui->textEdit->append(QString("quote=%1\tbase=%2\tcommission=%3\tvolume=%4")
                         .arg(quote)
                         .arg(base)
                         .arg(commission)
                         .arg(volume));
    QString totalProfit = QString::number(quote.toDouble() - commission.toDouble());
    if(quote.toDouble() < 0) {
        qDebug() << "quote still negative";
    }
//    totalSpent = QString::number(totalSpent.toDouble() + commission.toDouble());
//    QString needPrice = QString::number(totalSpent.toDouble() / base.toDouble());
    qDebug() << ui->lePair->text() << "totalProfit =" << totalProfit;
    ui->textEdit->append("totalProfit =" + totalProfit);
    qDebug() << "at current prices" <<  QString::number(parsCurPrice(binacpp->getBookTicker(ui->lePair->text().toUtf8())) * base.toDouble()) << " count trades =" << count;
//    QString totalSpent;
//    if(quote.toDouble() < 0)
//        totalSpent = QString::number(quote.toDouble() * -1.0);
//    totalSpent = QString::number(totalSpent.toDouble() + commission.toDouble());
//    QString needPrice = QString::number(totalSpent.toDouble() / base.toDouble());
//    qDebug() << "totalSpent =" << totalSpent << " needPrice =" << needPrice;
}

void MainWindow::on_pushButton_clicked()
{
    getAllTradesOfPair();
}

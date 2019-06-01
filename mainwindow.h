#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDir>
#include <QDateTime>
#include <math.h>
#include "global.h"
#include "binacpp.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_pushButton_7_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_clicked();

private:
    bool loadConfig(MessageProcess &msgProc);
    void getAllTradesOfPair();
    bool parsAllMyTrades(QByteArray array);
    double parsCurPrice(QByteArray array);

    int count;
    QFile configFile;
    QByteArray downloadCandle(QString dtStart, QString dtEnd, QString req);
    UserData userData;
    Binacpp *binacpp;
    QList<TradesPair> tradesList;
    QList<TradesPair> filledTrades;
    QByteArray curArray;

    QString getQuoteAsset(QString a_pair);
    QString getBaseAsset(QString a_pair);
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H

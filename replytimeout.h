#ifndef REPLYTIMEOUT_H
#define REPLYTIMEOUT_H

#include <QObject>
#include <QTimer>
#include <QNetworkReply>

class ReplyTimeout : public QObject
{
    Q_OBJECT
public:
    ReplyTimeout(QNetworkReply *reply, const int timeout);

signals:
    void timeout(QNetworkReply* reply);  // 超时信号 - 供进一步处理

private slots:
    void onTimeout();
};

#endif // REPLYTIMEOUT_H

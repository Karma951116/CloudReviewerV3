#include "replytimeout.h"


ReplyTimeout::ReplyTimeout(QNetworkReply *reply, const int timeout)
{
    Q_ASSERT(reply);
    if (reply && reply->isRunning()) {  // 启动单次定时器
        QTimer::singleShot(5000, this, SLOT(onTimeout()));
    }
}

void ReplyTimeout::onTimeout()
{
    QNetworkReply *reply = static_cast<QNetworkReply*>(parent());
    if (reply == nullptr) {
        this->deleteLater();
    }
    else {
        emit timeout(reply);
    }
}

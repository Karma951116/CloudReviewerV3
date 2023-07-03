#include "formattransformer.h"
#include <QDateTime>
#include <QTime>

FormatTransformer::FormatTransformer(QObject *parent) : QObject(parent)
{

}

QString FormatTransformer::strToBase64(QString str)
{
    QByteArray ba = str.toUtf8();
    return ba.toBase64();
}

QString FormatTransformer::base64ToStr(QString base64Str)
{
    return QString::fromUtf8(QByteArray::fromBase64(base64Str.toUtf8()));
}

int FormatTransformer::time2Sec(QString time)
{
    QTime tmp = QTime::fromString(time);
    if (tmp.isNull()) {
        return -1;
    }
    int hour = tmp.hour() == -1 ? 0 : tmp.hour();
    int min = tmp.minute() == -1 ? 0 : tmp.minute();
    int sec = tmp.second() == -1 ? 0 : tmp.second();
    return hour * 3600 + min * 60 + sec;
}

QString FormatTransformer::sec2Time(int seconds)
{
    if (seconds == NULL) {
        return "";
    }
    QString time=QTime(0, 0, 0).addSecs(seconds).toString(QString::fromLatin1("HH:mm:ss"));
    return time;
}

QString FormatTransformer::toDateTime(QJsonValue time)
{
    qint64 timeStamp = time.toDouble();
    return QDateTime::fromMSecsSinceEpoch(timeStamp).toString("yyyy/MM/dd hh:mm:ss");
}

QString FormatTransformer::getMediaTypeBySuffix(QString suffix)
{
    if(suffix.contains("mp4") ||
       suffix.contains("mov")) {
        return "video";
    }
    else if (suffix.contains("wav") ||
             suffix.contains("aac") ||
             suffix.contains("mp3")) {
        return "audio";
    }
    else if (suffix.contains("jpg") ||
             suffix.contains("jpeg") ||
             suffix.contains("png")) {
        return "picture";
    }
    else {
        return "other";
    }
}

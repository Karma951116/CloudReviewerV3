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

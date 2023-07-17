#include "formattransformer.h"
#include <QDateTime>
#include <QJsonDocument>
#include <QFile>
#include <QTime>
#include <QBuffer>
#include <QDebug>
#include <QUuid>
#include "def.h"

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

QString FormatTransformer::timestamp2RelTime(QString time)
{
    double curTime = QDateTime::currentDateTime().toSecsSinceEpoch();
    double logTime = QDateTime::fromString(time, "yyyy/MM/dd hh:mm:ss").toSecsSinceEpoch();

    int diff_sec = curTime - logTime;
    int deff_min = diff_sec / 60;
    if (deff_min < 60) {
        return QString("%1 分钟前").arg(deff_min);
    }
    // >24h
    else if (deff_min / 60 / 24 > 0) {
        return QString("%1 天前").arg(deff_min / 60 / 24);
    }
    else {
        return QString("%1 小时前").arg(deff_min / 60);
    }
}

QString FormatTransformer::image2Base64(QImage image)
{
    QByteArray ba;
    QBuffer buf(&ba);
    buf.open(QIODevice::WriteOnly);

    image.save(&buf, "png");
    QByteArray ba2 = ba.toBase64();
    QString b64str = QString::fromLatin1(ba2);

    return b64str;
}

QString FormatTransformer::image2Base64FromLocalFile(QString filePath)
{
    QImage img(filePath);
    QByteArray ba;
    QBuffer buf(&ba);
    buf.open(QIODevice::WriteOnly);

    img.save(&buf, "png");
    QByteArray ba2 = ba.toBase64();
    QString b64str = QString::fromLatin1(ba2);

    return b64str;
}

bool FormatTransformer::exportCommentsJson(QJsonArray comments, QString exportPath)
{
    QJsonDocument doc = QJsonDocument(comments);
    QString filePath = exportPath.remove("file:///");
    QFile file(filePath);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        qDebug() << QString("Fail to open the file:" + filePath);
        return false;
    }
    //write and close
    file.write(doc.toJson());
    file.close();
    return true;
}

QString FormatTransformer::toDateTime(QJsonValue time)
{
    qint64 timeStamp = time.toDouble();
    return QDateTime::fromMSecsSinceEpoch(timeStamp).toString("yyyy/MM/dd hh:mm:ss");
}

QString FormatTransformer::getMediaTypeBySuffix(QString suffix)
{
    if(VideoSuffix.contains(suffix)) {
        return "video";
    }
    else if (AudioSuffix.contains(suffix)) {
        return "audio";
    }
    else if (ImageSuffix.contains(suffix)) {
        return "image";
    }
    else {
        return "other";
    }
}

QString FormatTransformer::uuidGenerate32()
{
    QUuid uuid = QUuid::createUuid();
    return uuid.toString().remove("{").remove("}").remove("-").toUpper();
}

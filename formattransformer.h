#ifndef FORMATTRANSFORMER_H
#define FORMATTRANSFORMER_H

#include <QObject>
#include <QImage>
#include <QJsonArray>

class FormatTransformer : public QObject
{
    Q_OBJECT
public:
    explicit FormatTransformer(QObject *parent = nullptr);

    Q_INVOKABLE QString strToBase64(QString str);
    Q_INVOKABLE QString base64ToStr(QString base64Str);
    Q_INVOKABLE int time2Sec(QString time);
    Q_INVOKABLE QString sec2Time(int seconds);
    Q_INVOKABLE QString timestamp2RelTime(QString time);
    Q_INVOKABLE QString image2Base64(QImage image);
    Q_INVOKABLE QString image2Base64FromLocalFile(QString filePath);

    static QString toDateTime(QJsonValue time);
    static QString getMediaTypeBySuffix(QString suffix);
signals:

};

#endif // FORMATTRANSFORMER_H

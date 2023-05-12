#ifndef REPLYPARSER_H
#define REPLYPARSER_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QImage>
#include <def.h>
#include <hlsindex.h>

class ReplyParser : public QObject
{
    Q_OBJECT
public:
    enum Level{
        ROOT,
        TASK,
        FOLDER
    };
    Q_ENUMS(Level);
    explicit ReplyParser(QObject *parent = nullptr);

    void pinDataProcess(QByteArray data);
    void loginDataProcess(QByteArray data);
    void companyProcess(QByteArray data);
    void projectProcess(QByteArray data);
    void projectFileProcess(QByteArray data);
    void reviewStatusProcess(QByteArray data);
    void networkImageProcess(ReplyMeta meta, QByteArray data);
    void fileInfoProcess(QByteArray data);
    void m3u8IndexProcess(QByteArray data);
    void tsFileProcess(ReplyMeta meta, QByteArray data);
    void mediaInfoProcess(QByteArray data);

signals:
    void pinReplyDone(bool success, QString msg = nullptr);
    void loginReplyDone(bool success);
    void companyReplyDone(bool success, QJsonArray companies = QJsonArray());
    void projectReplyDone(bool success, QJsonArray projects = QJsonArray());
    void projectFileReplyDone(bool success, QJsonObject fileArr = QJsonObject());
    void reviewStatusReplyDone(bool success, QString msg);
    void networkImageReplyDone(bool success, QImage image, QString uuid);
    void fileInfoReplyDone(bool success, QString status = NULL, QString netDiskPath = nullptr);
    void commentReplyDone(bool success, QJsonArray comments = QJsonArray());
    void m3u8ReplyDone(bool success, HlsIndex* index);
    void mediaInfoReplyDone(bool success, int nbFrames, int frameRate);
    void tsFetched(TsFile* ts);


    void updateTokens(QString accessToken, QString refreshToken);
    void updateUser(QJsonObject userData);
public slots:
    void onReplied(ReplyMeta meta, QByteArray content);

private:
    QJsonObject originFileArr_;
};

#endif // REPLYPARSER_H

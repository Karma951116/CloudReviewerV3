#ifndef HTTPFUNCTIONS_H
#define HTTPFUNCTIONS_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QJsonArray>
#include <QNetworkReply>
#include <QJsonObject>
#include <QImage>
#include <QPixmap>

#include "def.h"
#include "hlsindex.h"

class HttpFunctions : public QObject
{
    Q_OBJECT
public:
    //Q_ENUM(DataType);
    explicit HttpFunctions();

    Q_INVOKABLE bool postLogin(QString username, QString password);
    Q_INVOKABLE bool postPin(QString phone);

    Q_INVOKABLE bool getUserCompany();
    Q_INVOKABLE bool getCompanyProjects(int companyId);
    Q_INVOKABLE bool getProjectFiles(int projectId, QString projectUuid,
                                     int orderingRule, int currentPage, int pageSize);
    Q_INVOKABLE bool postReviewStatus(QString auditContentUuid, QString versionUuid,
                                      QJsonArray auditFileFolderUuid, QString status);
    Q_INVOKABLE bool getFileInfo(QString auditFileFolderUuid);
    Q_INVOKABLE void getIndex(QString url, QString auditFileFolderUuid);
    Q_INVOKABLE void getMediaInfo(QString auditFileFolderUuid);
    Q_INVOKABLE void getTs(QString tsName, TsFile* ts, QString url);

    Q_INVOKABLE bool getNetworkImage(QString imageUrl, QString uuid);

private:
    QNetworkAccessManager* manager_;
    QNetworkRequest request_;
    QMap<QNetworkReply*, ReplyMeta> map_;
    QString session_id_;
    Protocol protocal_type_;
    QString address_;
    QString port_;
    QString accessToken_;
    QString refreshToken_;

    QString urlHead();
    void getSessionId(QNetworkReply* reply);

signals:
    void replied(ReplyMeta meta, QByteArray content);
public slots:
    void onUpdateTokens(QString accessToken, QString refreshToken);
private slots:
    void onRequestFinished(QNetworkReply* reply);
};

#endif // HTTPFUNCTIONS_H

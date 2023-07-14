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
    explicit HttpFunctions();

    Q_INVOKABLE bool postLogin(QString username, QString password);
    Q_INVOKABLE bool postPin(QString phone);
    Q_INVOKABLE bool postSendComment(QString auditFileFolderUuid, QString parentUuid, QString commentSendUserUuid,
                                     QString commentReceivingUserUuid, QJsonArray stakeholder,
                                     QString commentDetails, QString operationType,
                                     QString start, QString end, QString printscreen, QJsonArray attachment);
    Q_INVOKABLE bool postReviewStatus(QString auditContentUuid, QString versionUuid,
                                      QJsonArray auditFileFolderUuid, QString status);

    Q_INVOKABLE bool getUserCompany();
    Q_INVOKABLE bool getCompanyProjects(int companyId);
    Q_INVOKABLE bool getProjectFiles(int projectId, QString projectUuid,
                                     int orderingRule, int currentPage, int pageSize);
    Q_INVOKABLE bool getFileInfo(QString auditFileFolderUuid);
    Q_INVOKABLE bool getFileStakeHolder(QString auditContentUuid);
    Q_INVOKABLE bool getFileVersions(QString auditContentUuid);
    Q_INVOKABLE void getIndex(QString auditFileFolderUuid);
    Q_INVOKABLE void getMediaInfo(QString auditFileFolderUuid);
    Q_INVOKABLE void getTs(QString tsName, TsFile* ts, QString auditFileFolderUuid);

    Q_INVOKABLE bool getNetworkImage(QString imageUrl, QString uuid);
    Q_INVOKABLE bool getRefreshComment(QString auditFileFolderUuid);



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
    void replied(ReplyMeta meta, QByteArray content,
                 QNetworkReply::NetworkError = QNetworkReply::NoError);
public slots:
    void onUpdateTokens(QString accessToken, QString refreshToken);
    void onRequestTsFile(QString tsName, TsFile* Ts, QString auditFileFolderUuid);
private slots:
    void onRequestFinished(QNetworkReply* reply);
    //void onTsTimeout(QNetworkReply* reply);
};

#endif // HTTPFUNCTIONS_H

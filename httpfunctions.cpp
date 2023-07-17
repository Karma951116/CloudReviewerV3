#include <QJsonDocument>
#include <QUrlQuery>
#include <QFile>
#include <QNetworkCookie>
#include <QNetworkProxy>
#include <QNetworkConfiguration>
#include <QRegularExpression>

#include "httpfunctions.h"
#include "replytimeout.h"


HttpFunctions::HttpFunctions()
    : manager_(new QNetworkAccessManager(this)),
      protocal_type_(HTTP)

{
    connect(manager_, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(onRequestFinished(QNetworkReply*)));
    QNetworkConfiguration config = manager_->configuration();
    config.setConnectTimeout(3000);
#ifdef DEBUG
    address_ = "127.0.0.1";
    port_ = "6344";
#endif
#ifdef PROD
    address_ = "192.168.10.110";
    port_ = "8069";
#endif
}

bool HttpFunctions::postLogin(QString username, QString password)
{
    QString url = urlHead();
#ifdef DEBUG
    url += "/cloud_review_test/vue_api/jwt/login";
#endif
#ifdef PROD
    url += "/vue_api/jwt/login";
#endif
    QJsonObject params;
    params.insert("username", username);
    params.insert("password", password);
    request_.setRawHeader("Content-Type", "application/json");
    request_.setUrl(QUrl(url));
    QJsonObject paramItem;
    paramItem.insert("params", params);
    map_.insert(manager_->post(request_, QJsonDocument(paramItem).toJson()), ReplyMeta{LOGIN, 0, 0});
    return true;
}

bool HttpFunctions::postPin(QString phone)
{
    QString url = urlHead();
#ifdef DEBUG
    url += "/cloud_review_test/cloudmovie/interrogationRoom/V1/sendComment";
#endif
#ifdef PROD
    url += "/cloudmovie/interrogationRoom/V1/sendComment";
#endif
    QJsonObject params;
    params.insert("phone", phone);
    request_.setRawHeader("Content-Type", "application/json");
    request_.setUrl(QUrl(url));
    QJsonObject paramItem;
    paramItem.insert("params", params);
    map_.insert(manager_->post(request_, QJsonDocument(paramItem).toJson()), ReplyMeta{PIN, 0, 0});
    return true;
}

bool HttpFunctions::postSendComment(QString auditFileFolderUuid, QString parentUuid, QString commentSendUserUuid, QString commentReceivingUserUuid, QJsonArray stakeholder, QString commentDetails, QString operationType, QString start, QString end, QString printscreen, QJsonArray attachment)
{
    QString url = urlHead();
#ifdef DEBUG
    url += "/web/send_login_phone_code";
#endif
#ifdef PROD
    url += "/web/send_login_phone_code";
#endif
    request_.setRawHeader("Content-Type", "application/json");
    request_.setUrl(url);
    QJsonObject params;
    params.insert("auditFileFolderUuid", auditFileFolderUuid);
    QJsonObject commentRecords;
    commentRecords.insert("parent", parentUuid);
    commentRecords.insert("commentSendUserUuid", commentSendUserUuid);
    commentRecords.insert("commentTheReceivingUserUuid", commentReceivingUserUuid);
    commentRecords.insert("stakeholder", stakeholder);
    commentRecords.insert("commentDetails", commentDetails);
    commentRecords.insert("operationType", operationType);
    commentRecords.insert("start", start);
    commentRecords.insert("end", end);

    //TODO: 图像处理,BASE64或其他形式上传
    commentRecords.insert("printscreen", printscreen);
    commentRecords.insert("attachment", attachment);

    params.insert("commentRecord", commentRecords);
    QJsonObject paramItem;
    paramItem.insert("params", params);
    map_.insert(manager_->post(request_, QJsonDocument(paramItem).toJson()), ReplyMeta{COMMENT_UPLOAD, 0, 0});
    return true;
}

bool HttpFunctions::getUserCompany()
{
    QString url = urlHead();
#ifdef DEBUG
    url += "/cloud_review_test/cloudmovie/interrogationRoom/V1/getCustomerCompany";
    request_.setRawHeader("Cookie", "cmc_token=" + accessToken_.toUtf8());
#endif
#ifdef PROD
    url += "/cloudmovie/interrogationRoom/V1/getCustomerCompany";
    request_.setRawHeader("Cookie", session_id_.toUtf8());
#endif
    request_.setRawHeader("Content-Type", "text/html");
    QUrl qUrl;
    QUrlQuery qUrlQuery;
    qUrl.setUrl(url);
    qUrl.setQuery(qUrlQuery);
    request_.setUrl(qUrl);
    map_.insert(manager_->get(request_), ReplyMeta{USER_COMPANY, 0, 0});
    return true;
}

bool HttpFunctions::getCompanyProjects(int companyId)
{
    QString url = urlHead();
    QUrl qUrl;
    QUrlQuery qUrlQuery;
    request_.setRawHeader("Content-Type", "text/html");
#ifdef DEBUG
    url += "/cloud_review_test/cloudmovie/interrogationRoom/V1/getProjectList";
    qUrlQuery.addQueryItem("allowCompanies", QString::number(companyId));
    request_.setRawHeader("Cookie", "cmc_token=" + accessToken_.toUtf8());
#endif
#ifdef PROD
    url += "/cloudmovie/interrogationRoom/V1/getProjectList";
    qUrlQuery.addQueryItem("allowCompanies", "[" + QString::number(companyId) + "]");
    request_.setRawHeader("Cookie", session_id_.toUtf8());
#endif
    qUrl.setUrl(url);
    qUrl.setQuery(qUrlQuery);
    request_.setUrl(qUrl);
    map_.insert(manager_->get(request_), ReplyMeta{COMPANY_PROJECTS, 0, 0});
    return true;
}

bool HttpFunctions::getProjectFiles(int projectId, QString projectUuid,
                                    int orderingRule, int currentPage, int pageSize)
{
    QString url = urlHead();
#ifdef DEBUG
    url += "/cloud_review_test/cloudmovie/interrogationRoom/V1/getInterrogationRoomFileList";
    request_.setRawHeader("Cookie", "cmc_token=" + accessToken_.toUtf8());
#endif
#ifdef PROD
    url += "/cloudmovie/interrogationRoom/V1/getInterrogationRoomFileList";
    request_.setRawHeader("Cookie", session_id_.toUtf8());
#endif
    request_.setRawHeader("Content-Type", "text/html");
    QUrl qUrl;
    QUrlQuery qUrlQuery;
    qUrlQuery.addQueryItem("projectId", QString::number(projectId));
    qUrlQuery.addQueryItem("projectUuid", projectUuid);
    qUrlQuery.addQueryItem("orderingRule", QString::number(orderingRule));
    qUrlQuery.addQueryItem("currentPage", QString::number(currentPage));
    qUrlQuery.addQueryItem("pageSize", QString::number(pageSize));
    qUrl.setUrl(url);
    qUrl.setQuery(qUrlQuery);
    request_.setUrl(qUrl);
    map_.insert(manager_->get(request_), ReplyMeta{PROJECT_FILES, 0, 0});
    return true;
}

bool HttpFunctions::postReviewStatus(QString auditContentUuid, QString versionUuid, QJsonArray auditFileFolderUuid, QString status)
{
    QString url = urlHead();
#ifdef DEBUG
    url += "/cloud_review_test/cloudmovie/interrogationRoom/V1/alterFilesStatus";
    request_.setRawHeader("Cookie", "cmc_token=" + accessToken_.toUtf8());
#endif
#ifdef PROD
    url += "/cloudmovie/interrogationRoom/V1/alterFilesStatus";
    request_.setRawHeader("Cookie", session_id_.toUtf8());
#endif
    request_.setRawHeader("Content-Type", "application/json");
    QJsonObject params;
    params.insert("auditContentUuid", auditContentUuid);
    params.insert("versionUuid", versionUuid);
    params.insert("auditFileFolderUuid", auditFileFolderUuid);
    params.insert("status", status);
    QJsonObject paramItem;
    paramItem.insert("params", params);
    map_.insert(manager_->post(request_, QJsonDocument(paramItem).toJson()), ReplyMeta{REVIEW_STATUS, 0, 0});
    return true;
}

bool HttpFunctions::getFileInfo(QString auditFileFolderUuid)
{
    QString url = urlHead();
#ifdef DEBUG
    url += "/cloud_review_test/cloudmovie/interrogationRoom/V1/getFileInformation";
    request_.setRawHeader("Cookie", "cmc_token=" + accessToken_.toUtf8());
#endif
#ifdef PROD
    url += "/cloudmovie/interrogationRoom/V1/getFileInformation";
    request_.setRawHeader("Cookie", session_id_.toUtf8());
#endif
    QUrl qUrl;
    qUrl.setUrl(url);
    QUrlQuery qUrlQuery;
    qUrlQuery.addQueryItem("auditFileFolderUuid", auditFileFolderUuid);
    qUrl.setQuery(qUrlQuery);
    request_.setUrl(qUrl);
    map_.insert(manager_->get(request_), ReplyMeta{FILE_INFO, 0, 0});
    return true;
}

bool HttpFunctions::getFileStakeHolder(QString auditContentUuid)
{
    QString url = urlHead();
#ifdef DEBUG
    url += "/cloud_review_test/cloudmovie/interrogationRoom/V1/getFileStakeholder";
    request_.setRawHeader("Cookie", "cmc_token=" + accessToken_.toUtf8());
#endif
#ifdef PROD
    url += "/cloudmovie/interrogationRoom/V1/getFileStakeholder";
    request_.setRawHeader("Cookie", session_id_.toUtf8());
#endif
    QUrl qUrl;
    qUrl.setUrl(url);
    QUrlQuery qUrlQuery;
    qUrlQuery.addQueryItem("auditContentUuid", auditContentUuid);
    qUrl.setQuery(qUrlQuery);
    request_.setUrl(qUrl);
    map_.insert(manager_->get(request_), ReplyMeta{FILE_STAKEHOLDER, 0, 0});
    return true;
}

bool HttpFunctions::getFileVersions(QString auditContentUuid)
{
    QString url = urlHead();
#ifdef DEBUG
    url += "/cloud_review_test/cloudmovie/interrogationRoom/V1/getTheHistoricalVersion";
    request_.setRawHeader("Cookie", "cmc_token=" + accessToken_.toUtf8());
#endif
#ifdef PROD
    url += "/cloudmovie/interrogationRoom/V1/getTheHistoricalVersion";
    request_.setRawHeader("Cookie", session_id_.toUtf8());
#endif
    QUrl qUrl;
    qUrl.setUrl(url);
    QUrlQuery qUrlQuery;
    qUrlQuery.addQueryItem("auditContentUuid", auditContentUuid);
    qUrl.setQuery(qUrlQuery);
    request_.setUrl(qUrl);
    map_.insert(manager_->get(request_), ReplyMeta{FILE_VERSION, 0, 0});
    return true;
}

void HttpFunctions::getIndex(QString auditFileFolderUuid)
{
    QString url = urlHead();
    QUrl qUrl;
#ifdef DEBUG
    if (auditFileFolderUuid == "DA90E58E90AD483BAF81471F27216613") {
        //qUrl.setUrl("https://s7.fsvod1.com/20221010/ewaGwIfh/1500kb/hls/index.m3u8");
        url += "/cloud_review_test/cloudmovie/interrogationRoom/V1/index.m3u8";
        qUrl.setUrl(url);
    }
    else if (auditFileFolderUuid == "984E2DAF396847DF97B9EA54D56FFA4A") {
        qUrl.setUrl("https://cdn14.yzzy-tv-cdn.com/20230406/17211_5d5f0e69/2000k/hls/index.m3u8");
    }
#endif
#ifdef PROD
    url += "/cloudmovie/interrogationRoom/V1/index.m3u8";
    qUrl.setUrl(url);
#endif
    QUrlQuery qUrlQuery;
    qUrlQuery.addQueryItem("auditFileFolderUuid", auditFileFolderUuid);
    qUrl.setQuery(qUrlQuery);
    request_.setRawHeader("Content-Type", "text/html");
    request_.setRawHeader("Cookie", session_id_.toUtf8());
    request_.setUrl(qUrl);
    map_.insert(manager_->get(request_), ReplyMeta{M3U8, 0, auditFileFolderUuid});
}

void HttpFunctions::getMediaInfo(QString auditFileFolderUuid)
{
    QString url = urlHead();
    QUrl qUrl;
#ifdef DEBUG
    url += "/cloud_review_test/cloudmovie/interrogationRoom/V1/index";
    qUrl.setUrl(url);
#endif
#ifdef PROD
    url += "/cloudmovie/interrogationRoom/V1/index.txt"
    qUrl.setUrl(url);
#endif
    QUrlQuery qUrlQuery;
    qUrlQuery.addQueryItem("auditFileFolderUuid", auditFileFolderUuid);
    qUrl.setQuery(qUrlQuery);
    request_.setRawHeader("Content-Type", "text/plain");
    request_.setRawHeader("Cookie", session_id_.toUtf8());
    request_.setUrl(qUrl);
    map_.insert(manager_->get(request_), ReplyMeta{MEDIA_INFO, 0, 0});
}

void HttpFunctions::getTs(QString tsName, TsFile *ts, QString auditFileFolderUuid)
{
    QString url = urlHead();
    QUrl qUrl;
#ifdef DEBUG
    if (auditFileFolderUuid == "DA90E58E90AD483BAF81471F27216613") {
        //qUrl.setUrl("https://s7.fsvod1.com" + tsName);
        url += "/cloud_review_test/cloudmovie/interrogationRoom/V1/ts";
        qUrl.setUrl(url);
        QUrlQuery qUrlQuery;
        qUrlQuery.addQueryItem("fileName", tsName);
        qUrl.setQuery(qUrlQuery);
    }
    else if (auditFileFolderUuid == "984E2DAF396847DF97B9EA54D56FFA4A") {
        qUrl.setUrl("https://cdn14.yzzy-tv-cdn.com/20230406/17211_5d5f0e69/2000k/hls/" + tsName);
    }
#endif
#ifdef PROD
    url += "/cloudmovie/interrogationRoom/V1/index.ts"
    qUrl.setUrl(url);
    QUrlQuery qUrlQuery;
    qUrlQuery.addQueryItem("auditFileFolderUuid", auditFileFolderUuid);
    qUrlQuery.addQueryItem("TsName", tsName);
    qUrl.setQuery(qUrlQuery);
#endif
    request_.setRawHeader("Content-Type", "video/mp2t");
    request_.setRawHeader("Cookie", session_id_.toUtf8());
    request_.setUrl(qUrl);
    QNetworkReply* reply = manager_->get(request_);
    map_.insert(reply, ReplyMeta{TS, ts, 0});
    ts->setFetchState(TsFile::TsState::FETCHING);
}

bool HttpFunctions::getNetworkImage(QString imageUrl, QString uuid)
{
    QString url = urlHead();
    QUrl qUrl;
#ifdef DEBUG
    url += "/cloud_review_test/cloudmovie/interrogationRoom/V1/NetImage";
    qUrl.setUrl(url);
    request_.setRawHeader("Cookie", "cmc_token=" + accessToken_.toUtf8());
#endif
#ifdef PROD
    url += "/cloudmovie/interrogationRoom/V1/refreshTheCommentInformation";
    qUrl.setUrl(url);
    request_.setRawHeader("Cookie", session_id_.toUtf8());
#endif
    request_.setUrl(qUrl);
    map_.insert(manager_->get(request_), ReplyMeta{NETWORK_IMAGE, 0, uuid});
    return true;
}

QString HttpFunctions::getInternalImageUrl(QString url)
{
    QString retUrl = urlHead();
#ifdef DEBUG
    retUrl += url;
    //retUrl += "&Cookie=\"cmc_token=" + accessToken_.toUtf8() +"\"";
    //request_.setRawHeader("Cookie", "cmc_token=" + accessToken_.toUtf8());
#endif
#ifdef PROD
    retUrl += url;
    //retUrl += "&Cookie=" + session_id_.toUtf8();
    //request_.setRawHeader("Cookie", session_id_.toUtf8());
#endif
    return retUrl;
}

bool HttpFunctions::getRefreshComment(QString auditFileFolderUuid)
{
    QUrl qUrl;
#ifdef DEBUG
    qUrl.setUrl("/cloud_review_test/cloudmovie/interrogationRoom/V1/refreshTheCommentInformation");
    request_.setRawHeader("Cookie", "cmc_token=" + accessToken_.toUtf8());
#endif
#ifdef PROD
    qUrl.setUrl("/cloudmovie/interrogationRoom/V1/refreshTheCommentInformation");
    request_.setRawHeader("Cookie", session_id_.toUtf8());
#endif
    QUrlQuery qUrlQuery;
    qUrlQuery.addQueryItem("auditFileFolderUuid", auditFileFolderUuid);
    qUrl.setQuery(qUrlQuery);
    request_.setUrl(qUrl);
    map_.insert(manager_->get(request_), ReplyMeta{REFRESH_COMMENT, 0, 0});
    return true;
}


QString HttpFunctions::urlHead()
{
    QString url = protocal_type_ == HTTP ? "http" : "https";
    url += "://" + address_ + ":" + port_;
    return url;
}

void HttpFunctions::getSessionId(QNetworkReply *reply)
{
    QString tmp = reply->rawHeader("Set-Cookie");
    if (tmp.isEmpty()) {
        return;
    }
    QRegularExpression reg("(session_id=)([\\w]*)");
    QRegularExpressionMatch match = reg.match(tmp);
    if (match.hasMatch()) {
        session_id_ = match.captured(0);
    }
}

void HttpFunctions::onUpdateTokens(QString accessToken, QString refreshToken)
{
    accessToken_ = accessToken;
    refreshToken_ = refreshToken;
}

void HttpFunctions::onRequestTsFile(QString tsName, TsFile *Ts, QString auditFileFolderUuid)
{
    getTs(tsName, Ts, auditFileFolderUuid);
}


void HttpFunctions::onRequestFinished(QNetworkReply *reply)
{
    getSessionId(reply);
    ReplyMeta meta = map_.value(reply);
    if (meta.type == NETWORK_IMAGE) {
        netImageReply(reply->readAll(), meta.userData2);
    }
    replied(meta, reply->readAll(), reply->error());
}

//void HttpFunctions::onTsTimeout(QNetworkReply *reply)
//{
//    TsFile* file = (TsFile*)map_.value(reply).userData;
//    getTs(file->fileName(), file);
//    reply->abort();
//    reply->deleteLater();
//}

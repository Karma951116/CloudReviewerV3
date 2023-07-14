#include "replyparser.h"
#include "formattransformer.h"

#include <QByteArray>
#include <QJsonDocument>
#include <QSettings>
#include <QFile>
#include <QDebug>

ReplyParser::ReplyParser(QObject *parent) : QObject(parent)
{

}

void ReplyParser::pinDataProcess(QByteArray data) {
    QJsonParseError json_error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &json_error);
    QJsonObject root = doc.object();
    if (!root.contains("type") || !root.contains("message")) {
        pinReplyDone(false);
        return;
    }
    if (root["type"].toString() == "success") {
        pinReplyDone(true, root["message"].toString());
        return;
    }
    else {
        pinReplyDone(false, root["message"].toString());
        return;
    }
}

void ReplyParser::loginDataProcess(QByteArray data)
{
    QJsonParseError json_error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &json_error);
    QJsonObject root = doc.object();
    if (root["type"].toString() != "success") {
       loginReplyDone(false);
       return;
    }
    QJsonObject result = root["result"].toObject();
    if (result["string"].toString() == "登录成功") {
        QString accessToken = result["accessToken"].toString();
        QString refreshToken = result["refreshToken"].toString();
        QJsonObject userData = result["userData"].toObject();
        updateTokens(accessToken, refreshToken);
        updateUser(userData);
        loginReplyDone(true);
    }
    else {
        loginReplyDone(false);
    }
}

void ReplyParser::companyProcess(QByteArray data)
{
    QJsonParseError json_error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &json_error);
    QJsonObject root = doc.object();
    if (root["type"].toString() != "success") {
        companyReplyDone(false);
        return;
    }
    QJsonObject result = root["result"].toObject();
    QJsonArray companyList = result["companyList"].toArray();
    if (companyList.size() <= 0) {
        companyReplyDone(false);
        return;
    }
    companyReplyDone(true, companyList);
}

void ReplyParser::projectProcess(QByteArray data)
{
    QJsonParseError json_error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &json_error);
    QJsonObject root = doc.object();
    if (root["type"].toString() != "success") {
        projectReplyDone(false);
        return;
    }
    QJsonObject result = root["result"].toObject();
    QJsonArray projectList = result["projectList"].toArray();
    if (projectList.size() < 0) {
        projectReplyDone(false);
        return;
    }
    projectReplyDone(true, projectList);
}

void ReplyParser::projectFileProcess(QByteArray data)
{
    QJsonParseError json_error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &json_error);
    QJsonObject root = doc.object();
    if (root.value("type") != "success") {
        projectFileReplyDone(false);
    }
    QJsonArray tasks = root["result"].toObject()["data"].toObject()["cmcTaskList"].toArray();
    QJsonObject taskArr;
    for(int i = 0; i < tasks.size(); i++) {

        QJsonObject task = tasks[i].toObject();
        QJsonObject reformTask;
        //reformTask["auditContentUuid"] = task["auditContentUuid"].toString();
        reformTask["versionId"] = task["currentVersion"].toObject()["versionId"].toInt();
        reformTask["versionUuid"] = task["currentVersion"].toObject()["versionUuid"].toString();
        reformTask["versionName"] = task["currentVersion"].toObject()["versionName"].toString();
        reformTask["isTask"] = true;

        QJsonArray fileFolderList = task["currentVersion"].toObject()["auditFileFolderList"].toArray();
        QJsonObject fileArr;
        for (int j = 0; j < fileFolderList.size(); j++) {
            QJsonObject card;
            //card["auditContentUuid"] = task["auditContentUuid"].toString();
            card["versionId"] = task["currentVersion"].toObject()["versionId"].toInt();
            card["versionUuid"] = task["currentVersion"].toObject()["versionUuid"].toString();
            card["versionName"] = task["currentVersion"].toObject()["versionName"].toString();
            card["isTask"] = false;
            QJsonObject file = fileFolderList[j].toObject();
            QJsonObject fileInfo;
            card["auditFileFolderUuid"] = file["auditFileFolderUuid"];
            card["status"] = file["status"];
            // 非文件夹，写文件本身内容
            if (file["folder"].isNull()) {
                card["uuid"] = file["c"].toObject()["uuid"].toString();
                card["uploadUuid"] = file["attachment"].toObject()["uploadUuid"].toString();
                card["name"] = file["attachment"].toObject()["name"].toString();
                card["fileSize"] = file["attachment"].toObject()["fileSize"].toString();
                card["fileType"] = file["attachment"].toObject()["fileType"].toInt();
                card["createDate"] = FormatTransformer::toDateTime(file["attachment"].toObject()["createDate"]);
                card["fileImage"] = file["attachment"].toObject()["fileImage"].toString();
                card["duration"] = file["attachment"].toObject()["duration"].toString();
                card["fileSuffix"] = FormatTransformer::getMediaTypeBySuffix(
                                        file["attachment"].toObject()["fileSuffix"].toString());
            }
            else {
                card["uuid"] = file["folder"].toObject()["uuid"].toString();
                card["uploadUuid"] = file["folder"].toObject()["uploadUuid"].toString();
                card["name"] = file["folder"].toObject()["name"].toString();
                card["fileSize"] = file["folder"].toObject()["fileSize"].toString();
                card["fileType"] = file["folder"].toObject()["fileType"].toInt();
                card["createDate"] = FormatTransformer::toDateTime(file["folder"].toObject()["createDate"]);
                card["fileImage"] = "";
                card["duration"] = file["folder"].toObject()["duration"].toString();
                card["fileSuffix"] = "folder";
                if (!file["attachment"].isNull()) {
                    QJsonObject attachment = file["attachment"].toObject();
                    attachment["createDate"] = FormatTransformer::toDateTime(attachment["createDate"]);
                    attachment["fileSuffix"] = FormatTransformer::getMediaTypeBySuffix(
                                                attachment["fileSuffix"].toString());
                    attachment["isTask"] = false;
                    card["attachment"] = attachment;
                }
            }
            fileArr[file["auditFileFolderUuid"].toString()] = card;
        }
        reformTask["files"] = fileArr;
        taskArr[task["auditContentUuid"].toString()] = reformTask;
    }
    projectFileReplyDone(true, taskArr);
}

void ReplyParser::reviewStatusProcess(QByteArray data)
{
    QJsonParseError json_error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &json_error);
    QJsonObject root = doc.object();
    if (root.value("type") != "success") {

        reviewStatusReplyDone(false, root.value("message").toString());
    }
    else {
        reviewStatusReplyDone(true, root.value("message").toString());
    }
}

void ReplyParser::networkImageProcess(ReplyMeta meta, QByteArray data)
{
    QImage img;
    img.loadFromData(data);
    if (!img.isNull()) {
        networkImageReplyDone(true, img.copy(), meta.userData2);
    }
    else {
        networkImageReplyDone(false, img, meta.userData2);
    }
}

void ReplyParser::fileInfoProcess(QByteArray data)
{
    QJsonParseError json_error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &json_error);
    QJsonObject root = doc.object();
    if (root.value("type") != "success") {
        fileInfoReplyDone(false);
        commentReplyDone(false);
        return;
    }
    QJsonObject result = root.value("result").toObject();
    QJsonObject folder = result.value("auditFileFolder").toObject();
    QString netDiskPath = folder.value("netDiskPath").toString();
    QString status = folder.value("status").toString();

    QJsonArray comments = result.value("commentFile").toArray();
    QJsonArray rebuildArr = commentDataProcess(comments);
    fileInfoReplyDone(true, status, netDiskPath);
    commentReplyDone(true, rebuildArr);
}

void ReplyParser::m3u8IndexProcess(ReplyMeta meta, QByteArray data)
{
    HlsIndex* index = new HlsIndex();
    index->setData(data.data());
    index->setUrl("http://192.168.10.110:8069/cloudmovie/interrogationRoom/V1/index.m3u8");
    index->setAuditFileFolderUuid(meta.userData2);
    QRegExp re("#EXTINF:.*\\n.*\\n");
    // 逗号后的换行符方便处理
    QString tmp = QString::fromLocal8Bit(data);
    re.indexIn(tmp);
    QString capture = re.cap(0).replace(",\n", ",");
    QStringList targetList = capture.split("\n", QString::SkipEmptyParts);
    if (targetList.isEmpty()) {
        m3u8ReplyDone(false, index);
    }
    double indexDuration = 0;
    QList<double>* timeTable = index->getTimeTable();
    for (int i = 0; i < targetList.size(); i++) {
        QString target = targetList[i];
        if (target.contains("#EXT-X-ENDLIST")) {
            continue;
        }
        TsFile* file = new TsFile();
        QStringList info = target.split(",");
        file->setTsIndex(i);
        file->setDuration(info[0].remove("#EXTINF:").toDouble());
        file->setFileName(info[1].trimmed());
        file->setUrl(index->getUrl() + file->fileName());
        file->setFetchState(TsFile::TsState::NOT_FETCHED);
        file->setDecodeState(TsFile::DecodeState::NOT_DECODE);
        connect(file, SIGNAL(fetchChanged(TsFile::TsState)),
                index, SLOT(onFileFetchChanged(TsFile::TsState)));
        index->insertTs(file);
        indexDuration += file->duration();
        timeTable->append(indexDuration);
    }
    index->setDuration(indexDuration);
    m3u8ReplyDone(true, index);
}

void ReplyParser::tsFileProcess(ReplyMeta meta, QByteArray data)
{
    TsFile* ts = (TsFile*)meta.userData;
    ts->setDataLength(data.length());
    ts->setData((uint8_t*)malloc(ts->dataLength()));
    memcpy(ts->data(), (uint8_t*)data.data(), ts->dataLength());
    ts->setFetchState(TsFile::TsState::FETCHED);
//    QFile file("E:\\ts_test\\" + ts->fileName() + ".ts");
//    file.open(QIODevice::WriteOnly);
//    file.write(data);
//    file.close();
    qDebug() << "Fetched Ts: " << ts->fileName();
    tsFetched(ts);
}

void ReplyParser::mediaInfoProcess(QByteArray data)
{
    QString content = QString::fromLocal8Bit(data);
    //qDebug() << content;
    QRegExp re("nb_frames=\\d*");
    re.indexIn(content);
    int nbFrames = re.cap(0).replace("nb_frames=", "").toInt();
    re.setPattern("avg_frame_rate=\\d*");
    re.indexIn(content);
    int frameRate = re.cap(0).replace("avg_frame_rate=", "").toInt();
    re.setPattern("sample_rate=\\d*");
    re.indexIn(content);
    QString sampleRate = re.cap(0).replace("sample_rate=", "");
    re.setPattern("channels=\\d*");
    re.indexIn(content);
    QString chennels = re.cap(0).replace("channels=", "");

    QJsonObject ret, video, audio;
    video["nbFrames"] = nbFrames;
    video["frameRate"] = frameRate;
    audio["sampleRate"] = sampleRate;
    audio["chennels"] = chennels;
    ret["video"] = video;
    ret["audio"] = audio;
    mediaInfoReplyDone(true, ret);
}

void ReplyParser::stakeholderProcess(QByteArray data)
{
    QJsonParseError json_error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &json_error);
    QJsonObject root = doc.object();
    if (root.value("type") != "success") {
        stakeholderReplyDone(false);
        return;
    }
    QJsonArray stakeholderList = root["result"].toArray();
    if (stakeholderList.size() < 0) {
        stakeholderReplyDone(false);
        return;
    }
    stakeholderReplyDone(true, stakeholderList);
}

void ReplyParser::commentUploadProcess(QByteArray data)
{
    QJsonParseError json_error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &json_error);
    QJsonObject root = doc.object();
    if (root.value("type") != "success") {
        commentUploadReplyDone(false, root.value("message").toString());
    }
    else {
        commentUploadReplyDone(true, root.value("message").toString());
    }
}

void ReplyParser::commentRefreshProcess(QByteArray data)
{
    QJsonParseError json_error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &json_error);
    QJsonObject root = doc.object();
    if (root.value("type") != "success") {
        return;
    }
    QJsonArray result = root.value("result").toArray();
    QJsonArray rebuildArr = commentDataProcess(result);
    commentReplyDone(true, rebuildArr);
}

void ReplyParser::versionProcess(QByteArray data)
{
    QJsonParseError json_error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &json_error);
    QJsonObject root = doc.object();
    if (root.value("type") != "success") {
        stakeholderReplyDone(false);
        return;
    }
    QJsonObject result = root["result"].toObject();
    QString auditContentUuid = result.value("auditContentUuid").toString();
    QJsonArray versionsList = result.value("versionList").toArray();

    QJsonArray rebuildArr;
    for (int i = 0; i < versionsList.size(); i++) {
        QJsonObject rebuildObj;
        QJsonObject obj = versionsList[i].toObject();
        rebuildObj["auditContentUuid"] = auditContentUuid;
        rebuildObj["versionId"] = obj.value("versionId").toInt();
        rebuildObj["versionUuid"] = obj.value("versionUuid").toString();
        rebuildObj["versionName"] = obj.value("versionName").toString();
        rebuildObj["auditFileFolderUuid"] = obj.value("auditFileFolder").toObject().value("auditFileFolderUuid").toString();
        rebuildObj["uuid"] = obj.value("auditFileFolder").toObject().value("auditFileFolder").toObject().value("uuid").toString();
        rebuildObj["fileSuffix"] = FormatTransformer::getMediaTypeBySuffix(
                    obj.value("auditFileFolder").toObject().value("auditFileFolder").toObject().value("fileSuffix").toString());
        rebuildObj["name"] = obj.value("auditFileFolder").toObject().value("auditFileFolder").toObject().value("name").toString();
        rebuildObj["uploadUuid"] = obj.value("auditFileFolder").toObject().value("auditFileFolder").toObject().value("uploadUuid").toString();
        rebuildObj["netDiskPath"] = obj.value("auditFileFolder").toObject().value("auditFileFolder").toObject().value("netDiskPath").toString();
        rebuildObj["status"] = obj.value("auditFileFolder").toObject().value("auditFileFolder").toObject().value("status").toString();
        rebuildObj["imageUrl"] = obj.value("auditFileFolder").toObject().value("auditFileFolder").toObject().value("image").toString();
        rebuildArr.append(rebuildObj);
    }
    versionReplyDone(true, rebuildArr);
}


void ReplyParser::onReplied(ReplyMeta meta, QByteArray content, QNetworkReply::NetworkError error)
{
    switch (meta.type) {
    case DataType::PIN:
        pinDataProcess(content);
        break;
    case DataType::LOGIN:
        loginDataProcess(content);
        break;
    case DataType::USER_COMPANY:
        companyProcess(content);
        break;
    case DataType::COMPANY_PROJECTS:
        projectProcess(content);
        break;
    case DataType::PROJECT_FILES:
        projectFileProcess(content);
        break;
    case DataType::NETWORK_IMAGE:
        networkImageProcess(meta, content);
        break;
    case DataType::REVIEW_STATUS:
        reviewStatusProcess(content);
        break;
    case DataType::FILE_INFO:
        fileInfoProcess(content);
        break;
    case DataType::M3U8:
        m3u8IndexProcess(meta, content);
        break;
    case DataType::TS:
        if (error == QNetworkReply::TimeoutError) {
            qDebug() << "ts timeout";
            TsFile* ts = (TsFile*)meta.userData;
            ts->setFetchState(TsFile::TsState::NOT_FETCHED);
        }
        tsFileProcess(meta, content);
        break;
    case DataType::MEDIA_INFO:
        mediaInfoProcess(content);
        break;
    case DataType::FILE_STAKEHOLDER:
        stakeholderProcess(content);
        break;
    case DataType::COMMENT_UPLOAD:
        commentUploadProcess(content);
        break;
    case DataType::REFRESH_COMMENT:
        commentRefreshProcess(content);
        break;
    case DataType::FILE_VERSION:
        versionProcess(content);
        break;
    }
}

QJsonArray ReplyParser::commentDataProcess(QJsonArray comments)
{
    QJsonArray parentArr;
    QJsonArray rebuildArr;
    for (int i = 0; i < comments.size(); i++) {
        QJsonObject comment = comments[i].toObject();
        // 筛选主评论（无父级评论）
        if (comment["parent"].isBool()) {
            comment["commentParent"] = comment["parent"].toBool();
            comment.remove("parent");
            parentArr.append(comment);
        }
    }
    for(int i = 0; i < parentArr.size(); i++) {
        QJsonObject comment = parentArr[i].toObject();
        comment["createDate"] = FormatTransformer::toDateTime(comment["createDate"]);

        QJsonArray childrens;
        for (int j = 0; j < comments.size(); j++) {
            QJsonObject prototype = comments[j].toObject();
            if (prototype["parent"].isBool()){
                continue;
            }
            if (prototype["parent"].toString() != comment["commentUuid"].toString()) {
                continue;
            }
            QJsonObject childrenComment;
            childrenComment["childUuid"] = prototype["commentUuid"].toString();
            childrenComment["childParent"] = prototype["parent"].toString();
            childrenComment["childSender"] = prototype["commentSendUser"].toObject();
            childrenComment["childReceiver"] = prototype["commentTheReceivingUser"].toObject();
            childrenComment["childDetails"] = prototype["commentDetails"].toString();
            childrenComment["childCreateDate"] = FormatTransformer::toDateTime(prototype["createDate"]);
            childrenComment["childOpType"] = prototype["operationType"].toInt();
            childrenComment["childPrintscreen"] = prototype["printscreen"].toString();
            childrenComment["childStakeholder"] = prototype["stakeholder"].toObject();
            childrenComment["childStart"] = prototype["start"].toString();
            childrenComment["childEnd"] = prototype["end"].toString();
            childrens.append(childrenComment);
        }
        comment["childrens"] = childrens;
        rebuildArr.append(comment);
    }
    return rebuildArr;
}

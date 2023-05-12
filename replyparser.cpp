#include "replyparser.h"
#include "formattransformer.h"

#include <QByteArray>
#include <QJsonDocument>
#include <QSettings>
#include <QFile>


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
    if (projectList.size() <= 0) {
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
    char* tmp = (char*) meta.userData;
    QByteArray uuid = QByteArray::fromRawData(tmp, 32);
    if (!img.isNull()) {
        networkImageReplyDone(true, img.copy(), QString::fromLocal8Bit(uuid));
    }
    else {
        networkImageReplyDone(false, img, QString::fromLocal8Bit(uuid));
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
            QJsonObject childrenComment = comments[j].toObject();
            if (childrenComment["parent"].isBool()){
                continue;
            }
            if (childrenComment["parent"].toString() == comment["commentUuid"].toString()) {
                childrenComment["createDate"] = FormatTransformer::toDateTime(childrenComment["createDate"]);
                childrenComment["commentParent"] = childrenComment["parent"].toString();
                childrenComment.remove("parent");
                childrens.append(childrenComment);
            }
        }
        comment["childrens"] = childrens;
        rebuildArr.append(comment);
    }
    fileInfoReplyDone(true, status, netDiskPath);
    commentReplyDone(true, rebuildArr);
}

void ReplyParser::m3u8IndexProcess(QByteArray data)
{
    HlsIndex* index = new HlsIndex();
    index->setData(data.data());
    index->setUrl("http://192.168.10.110:8069/cloudmovie/interrogationRoom/V1/index.m3u8");
    QRegExp re("(#EXTINF:.*,\\w*\\.ts\n)");
    // 逗号后的换行符方便处理
    QString tmp = QString::fromLocal8Bit(data).replace(",\n", ",");
    re.indexIn(tmp);
    QStringList targetList = re.cap(1).split("\n", QString::SkipEmptyParts);
    double indexDuration = 0;
    for (int i = 0; i < targetList.size(); i++) {
        QString target = targetList[i];
        TsFile* file = new TsFile();
        QStringList info = target.split(",");
        file->setTsIndex(i);
        file->setDuration(info[0].remove("#EXTINF:").toDouble());
        file->setFileName(info[1].trimmed());
        file->setUrl(index->getUrl() + file->fileName());
        file->setDecoded(false);
        file->setFetched(false);
        connect(file, SIGNAL(fetchChanged(bool)),
                index, SLOT(onFileFetchChanged(bool)));
        connect(file, SIGNAL(decodeChanged(bool)),
                index, SLOT(onFileDecodeChanged(bool)));
        index->insertTs(file);
        indexDuration += file->duration();
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
    ts->setFetched(true);
}

void ReplyParser::mediaInfoProcess(QByteArray data)
{
    QString content = QString::fromLocal8Bit(data);
    QRegExp re("nb_frames=\\d*");
    re.indexIn(content);
    int nbFrames = re.cap(0).replace("nb_frames=", "").toInt();
    re.setPattern("avg_frame_rate=\\d*");
    QString avgFrameRate = re.cap(0).replace("avg_frame_rate=", "");
    QStringList tmp = avgFrameRate.split("/");
    int frameRate = tmp[0].toInt() / tmp[1].toInt();
    mediaInfoReplyDone(true, nbFrames, frameRate);
}


void ReplyParser::onReplied(ReplyMeta meta, QByteArray content)
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
        m3u8IndexProcess(content);
        break;
    case DataType::TS:
        tsFileProcess(meta, content);
        break;
    case DataType::MEDIA_INFO:
        mediaInfoProcess(content);
        break;
    }
}

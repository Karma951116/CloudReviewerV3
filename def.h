#ifndef DEF_H
#define DEF_H

#include <QString>
#include <QList>
#include <QJsonObject>
#include <QJsonArray>

enum DataType {
    M3U8,
    TS,
    MEDIA_INFO,
    LOGIN,
    PIN,
    NETWORK_IMAGE,
    USER_COMPANY,
    COMPANY_PROJECTS,
    PROJECT_FILES,
    REVIEW_STATUS,
    FILE_INFO,
    FILE_STAKEHOLDER,
    COMMENT_UPLOAD,
    REFRESH_COMMENT,
    FILE_VERSION
};
enum Protocol {
    HTTP,
    HTTPS
};

const QVector<QString> ImageSuffix = {".jpg", ".png", ".jpeg"};
const QVector<QString> VideoSuffix = {".mp4", ".mkv"};
const QVector<QString> AudioSuffix = {".wav", ".mp3", ".aac"};

struct ReplyMeta {
    DataType type;
    void* userData;
    QString userData2;
};

struct Company {
    int id;
    QString uuid;
    QString name;
};

struct FileEntity {
    bool isTask;
    QString auditContentUuid;
    int versionId;
    QString versionUuid;
    QString versionName;
    QString auditFileFolderUuid;
    QString status;
    QString uuid;
    QString uploadUuid;
    QString name;
    QString fileSize;
    int fileType;
    QString createDate;
    QString imageUrl;
    QString duration;
    QString fileSuffix;
    bool checked;
};

struct Project {
    int id;
    QString uuid;
    QString name;
    QString companyName;
    qulonglong createDate;
    qulonglong startTime;
    qulonglong endTime;
    QString note;
    QString state;
    QString type;
    QString imageUrl;
};

struct Comment {
    QString commentUuid;
    QString parentUuid;
    QJsonObject sender;
    QJsonObject receiver;
    QString details;
    QString createDate;
    int operationType;
    QString printScreen;
    QJsonArray stakeHolder;
    QString start;
    QString end;
    QJsonArray childrens;
};

struct Stakeholder {
    int id;
    QString name;
    QString uuid;
    QString imageUrl;
};

struct Version {
    QString auditContentUuid;
    int versionId;
    QString versionUuid;
    QString versionName;
    QString auditFileFolderUuid;
    QString uuid;
    QString fileSuffix;
    QString name;
    QString uploadUuid;
    QString netDiskPath;
    QString status;
    QString imageUrl;
};

#endif // DEF_H

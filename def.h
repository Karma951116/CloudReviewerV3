#ifndef DEF_H
#define DEF_H

#include <QString>
#include <QList>

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
    FILE_INFO
};
enum Protocol {
    HTTP,
    HTTPS
};

enum FileType {
    VIDEO,
    AUDIO,
    IMAGE,
    NONE
};


const QVector<QString> ImageSuffix = {"jpg", "png", "jpeg"};
const QVector<QString> VideoSuffix = {"mp4", "mkv"};
const QVector<QString> AudioSuffix = {"wav", "mp3"};

struct ReplyMeta {
    DataType type;
    void* userData;
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


#endif // DEF_H

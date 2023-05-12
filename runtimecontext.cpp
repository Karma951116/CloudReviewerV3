#include "runtimecontext.h"

RuntimeContext::RuntimeContext(QObject *parent) : QObject(parent)
{

}

QString RuntimeContext::getCompanyUuid() const
{
    return companyUuid_;
}

void RuntimeContext::setCompanyUuid(const QString &value)
{
    companyUuid_ = value;
}

QString RuntimeContext::getCompanyName() const
{
    return companyName_;
}

void RuntimeContext::setCompanyName(const QString &value)
{
    companyName_ = value;
}

QString RuntimeContext::getProjectUuid() const
{
    return projectUuid_;
}

void RuntimeContext::setProjectUuid(const QString &value)
{
    projectUuid_ = value;
}

QString RuntimeContext::getProjectName() const
{
    return projectName_;
}

void RuntimeContext::setProjectName(const QString &value)
{
    projectName_ = value;
    projectNameChanged();
}

QString RuntimeContext::getAuditContentUuid() const
{
    return auditContentUuid_;
}

void RuntimeContext::setAuditContentUuid(const QString &auditContentUuid)
{
    auditContentUuid_ = auditContentUuid;
}

QString RuntimeContext::getVersionUuid() const
{
    return versionUuid_;
}

void RuntimeContext::setVersionUuid(const QString &versionUuid)
{
    versionUuid_ = versionUuid;
}

QString RuntimeContext::getAuditFileFolderUuid() const
{
    return auditFileFolderUuid_;
}

void RuntimeContext::setAuditFileFolderUuid(const QString &auditFileFolderUuid)
{
    auditFileFolderUuid_ = auditFileFolderUuid;
}

QString RuntimeContext::getAttachmentUuid() const
{
    return attachmentUuid_;
}

void RuntimeContext::setAttachmentUuid(const QString &attachmentUuid)
{
    attachmentUuid_ = attachmentUuid;
}

QString RuntimeContext::getFileName() const
{
    return fileName_;
}

void RuntimeContext::setFileName(const QString &fileName)
{
    fileName_ = fileName;
    fileNameChanged();
}

FileType RuntimeContext::getFileType() const
{
    return fileType_;
}

void RuntimeContext::setFileType(const FileType &fileType)
{
    fileType_ = fileType;
}

void RuntimeContext::setFileType(QString fileSuffix)
{
    if (VideoSuffix.contains(fileSuffix)) {
        fileType_ = VIDEO;
    }
    else if (AudioSuffix.contains(fileSuffix)) {
        fileType_ = AUDIO;
    }
    else if (ImageSuffix.contains(fileSuffix)) {
        fileType_ = IMAGE;
    }
    else {
        fileType_ = NONE;
    }
}


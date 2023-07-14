#ifndef RUNTIMECONTEXT_H
#define RUNTIMECONTEXT_H

#include <QObject>
#include <def.h>

class RuntimeContext : public QObject
{
    Q_OBJECT
public:
    enum FileType {
        VIDEO,
        AUDIO,
        IMAGE,
        NONE
    };
    Q_ENUM(FileType);
    explicit RuntimeContext(QObject *parent = nullptr);

    Q_PROPERTY(QString projectName READ getProjectName NOTIFY projectNameChanged)
    Q_PROPERTY(QString fileName READ getFileName NOTIFY fileNameChanged)

    Q_INVOKABLE QString getCompanyUuid() const;
    Q_INVOKABLE void setCompanyUuid(const QString &value);

    Q_INVOKABLE QString getCompanyName() const;
    Q_INVOKABLE void setCompanyName(const QString &value);

    Q_INVOKABLE QString getProjectUuid() const;
    Q_INVOKABLE void setProjectUuid(const QString &value);

    Q_INVOKABLE QString getProjectName() const;
    Q_INVOKABLE void setProjectName(const QString &value);

    Q_INVOKABLE QString getAuditContentUuid() const;
    Q_INVOKABLE void setAuditContentUuid(const QString &auditContentUuid);

    Q_INVOKABLE QString getVersionUuid() const;
    Q_INVOKABLE void setVersionUuid(const QString &versionUuid);

    Q_INVOKABLE QString getAuditFileFolderUuid() const;
    Q_INVOKABLE void setAuditFileFolderUuid(const QString &auditFileFolderUuid);

    Q_INVOKABLE QString getAttachmentUuid() const;
    Q_INVOKABLE void setAttachmentUuid(const QString &attachmentUuid);

    Q_INVOKABLE QString getFileName() const;
    Q_INVOKABLE void setFileName(const QString &fileName);

    Q_INVOKABLE FileType getFileType() const;
    Q_INVOKABLE void setFileType(const FileType &fileType);
    Q_INVOKABLE void setFileType(QString fileSuffix);

    Q_INVOKABLE QString getEmojis();
    Q_INVOKABLE QString getVersionName() const;
    Q_INVOKABLE void setVersionName(const QString &versionName);

private:
    QString companyUuid_;
    QString companyName_;
    QString projectUuid_;
    QString projectName_;
    QString auditContentUuid_;
    QString versionUuid_;
    QString versionName_;
    QString auditFileFolderUuid_;
    QString attachmentUuid_;
    QString fileName_;
    FileType fileType_;

signals:
    void projectNameChanged();
    void fileNameChanged();
    void fileTypeChanged();
};

#endif // RUNTIMECONTEXT_H

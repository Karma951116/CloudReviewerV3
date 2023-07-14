#ifndef VersionListModel_H
#define VersionListModel_H

#include <QObject>
#include <QAbstractListModel>

#include "def.h"

class VersionListModel : public QAbstractItemModel
{
    Q_OBJECT
public:

    Q_PROPERTY(QString currentVersion READ getCurrentVersion NOTIFY currentVersionChanged)

    enum RoleType {
        AUDIT_CONTENT_UUID,
        VERSION_ID,
        VERSION_UUID,
        VERSION_NAME,
        AUDIT_FILE_FOLDER_UUID,
        STATUS,
        UUID,
        UPLOAD_UUID,
        NAME,
        FILE_IMAGE_URL,
        FILE_SUFFIX,
        NET_DISK_PATH
    };

    VersionListModel();
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;
    void insert(Version* version);
    void remove(int row, int col, const QModelIndex &parent = QModelIndex());
    void clear(const QModelIndex &parent = QModelIndex());

    Q_INVOKABLE QVariant getRoleValue(int row, QString role);

    Q_INVOKABLE QString getCurrentVersion() const;
    Q_INVOKABLE void setCurrentVersion(const QString &currentVersion);

private:
    QList<Version*> versions_;
    QString versionToSwitch_;
    QString currentVersion_;
public slots:
    void onVersionReplyDone(bool success, QJsonArray versionList);
signals:
    void currentVersionChanged();
    void versionListRefreshed();
};

#endif // VersionListModel_H

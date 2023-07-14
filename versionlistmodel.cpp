#include "versionlistmodel.h"

VersionListModel::VersionListModel()
{

}

QModelIndex VersionListModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return createIndex(row, column);
}

QModelIndex VersionListModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);
    return QModelIndex();
}

QVariant VersionListModel::data(const QModelIndex &index, int role) const
{
    if(index.row() < 0 || index.row() >= versions_.size()) {
        return QVariant();
    }
    Version* version = versions_[index.row()];
    switch (role) {
    case RoleType::AUDIT_CONTENT_UUID:
        return version->auditContentUuid;
        break;
    case RoleType::VERSION_ID:
        return version->versionId;
        break;
    case RoleType::VERSION_UUID:
        return version->versionUuid;
        break;
    case RoleType::VERSION_NAME:
        return version->versionName;
        break;
    case RoleType::AUDIT_FILE_FOLDER_UUID:
        return version->auditFileFolderUuid;
        break;
    case RoleType::STATUS:
        return version->status;
        break;
    case RoleType::UUID:
        return version->uuid;
        break;
    case RoleType::UPLOAD_UUID:
        return version->uploadUuid;
        break;
    case RoleType::NAME:
        return version->name;
        break;
    case RoleType::FILE_IMAGE_URL:
        return version->imageUrl;
        break;
    case RoleType::FILE_SUFFIX:
        return version->fileSuffix;
        break;
    case RoleType::NET_DISK_PATH:
        return version->netDiskPath;
        break;
    }
    return QVariant();
}

int VersionListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return versions_.size();
}

int VersionListModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QHash<int, QByteArray> VersionListModel::roleNames() const
{
    QHash<int, QByteArray> map;
    map[RoleType::AUDIT_CONTENT_UUID] = "auditContentUuid";
    map[RoleType::VERSION_ID] = "versionId";
    map[RoleType::VERSION_UUID] = "versionUuid";
    map[RoleType::VERSION_NAME] = "versionName";
    map[RoleType::AUDIT_FILE_FOLDER_UUID] = "auditFileFolderUuid";
    map[RoleType::STATUS] = "status";
    map[RoleType::UUID] = "uuid";
    map[RoleType::UPLOAD_UUID] = "uploadUuid";
    map[RoleType::NAME] = "name";
    map[RoleType::FILE_IMAGE_URL] = "imageUrl";
    map[RoleType::FILE_SUFFIX] = "fileSuffix";
    map[RoleType::NET_DISK_PATH] = "netDiskPath";
    return map;
}

void VersionListModel::insert(Version* version)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    versions_.append(version);
    endInsertRows();
}

void VersionListModel::remove(int row, int col, const QModelIndex &parent)
{
    QModelIndex index = (QModelIndex)this->index(row, col);
    Version* version = versions_[index.row()];
    beginRemoveRows(parent, row, 0);
    versions_.removeAt(row);
    endRemoveRows();
    delete version;
}

void VersionListModel::clear(const QModelIndex &parent)
{
    beginRemoveRows(parent, 0, versions_.size() - 1);
    for (Version* version: versions_) {
        delete version;
    }
    versions_.clear();
    endRemoveRows();
}

QVariant VersionListModel::getRoleValue(int row, QString role)
{
    if(row < 0 || row >= versions_.size()) {
        return "";
    }
    QModelIndex index = (QModelIndex)this->index(row, 0);
    const Version* version = versions_[index.row()];
    switch (roleNames().key(role.toUtf8())) {
    case RoleType::AUDIT_CONTENT_UUID:
        return version->auditContentUuid;
        break;
    case RoleType::VERSION_ID:
        return version->versionId;
        break;
    case RoleType::VERSION_UUID:
        return version->versionUuid;
        break;
    case RoleType::VERSION_NAME:
        return version->versionName;
        break;
    case RoleType::AUDIT_FILE_FOLDER_UUID:
        return version->auditFileFolderUuid;
        break;
    case RoleType::STATUS:
        return version->status;
        break;
    case RoleType::UUID:
        return version->uuid;
        break;
    case RoleType::UPLOAD_UUID:
        return version->uploadUuid;
        break;
    case RoleType::NAME:
        return version->name;
        break;
    case RoleType::FILE_IMAGE_URL:
        return version->imageUrl;
        break;
    case RoleType::FILE_SUFFIX:
        return version->fileSuffix;
        break;
    case RoleType::NET_DISK_PATH:
        return version->netDiskPath;
        break;
    }
    return QVariant();
}

QString VersionListModel::getCurrentVersion() const
{
    return currentVersion_;
}

void VersionListModel::setCurrentVersion(const QString &currentVersion)
{
    currentVersion_ = currentVersion;
    currentVersionChanged();
}

void VersionListModel::onVersionReplyDone(bool success, QJsonArray versionList)
{
    if (!success) {
        return;
    }
    clear();
    for(int i = 0; i < versionList.size(); i++) {
        QJsonObject obj = versionList[i].toObject();
        Version* version = new Version();
        version->auditContentUuid = obj.value("auditContentUuid").toString();
        version->versionName = obj.value("versionName").toString();
        version->versionId = obj.value("versionId").toInt();
        version->versionUuid = obj.value("versionUuid").toString();
        version->auditFileFolderUuid = obj.value("auditFileFolderUuid").toString();
        version->status = obj.value("status").toString();
        version->uuid = obj.value("uuid").toString();
        version->uploadUuid = obj.value("uploadUuid").toInt();
        version->name = obj.value("name").toString();
        version->imageUrl = obj.value("imageUrl").toString();
        version->fileSuffix = obj.value("fileSuffix").toString();
        version->netDiskPath = obj.value("netDiskPath").toString();
        insert(version);
    }
    versionListRefreshed();
}

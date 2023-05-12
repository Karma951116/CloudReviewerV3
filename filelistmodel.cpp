#include "filelistmodel.h"

#include <QJsonObject>


FileListModel::FileListModel()
{
    //rows_ = 0;
    //columns_ = 5;
}

QModelIndex FileListModel::index(int row, int column, const QModelIndex &parent) const
{
    return createIndex(row, column);
}

QModelIndex FileListModel::parent(const QModelIndex &child) const
{
    return QModelIndex();
}

QVariant FileListModel::data(const QModelIndex &index, int role) const
{
    if(index.row() < 0 || index.row() >= fileList_.size()) {
        return QVariant();
    }
    const FileEntity* file = fileList_[index.row()];

    switch (role) {
    case RoleType::IS_TASK:
        return file->isTask;
        break;
    case RoleType::AUDIT_CONTENT_UUID:
        return file->auditContentUuid;
        break;
    case RoleType::VERSION_ID:
        return file->versionId;
        break;
    case RoleType::VERSION_UUID:
        return file->versionUuid;
        break;
    case RoleType::VERSION_NAME:
        return file->versionName;
        break;
    case RoleType::AUDIT_FILE_FOLDER_UUID:
        return file->auditFileFolderUuid;
        break;
    case RoleType::STATUS:
        return file->status;
        break;
    case RoleType::UUID:
        return file->uuid;
        break;
    case RoleType::UPLOAD_UUID:
        return file->uploadUuid;
        break;
    case RoleType::NAME:
        return file->name;
        break;
    case RoleType::FILE_SIZE:
        return file->fileSize;
        break;
    case RoleType::FILE_TYPE:
        return file->fileType;
        break;
    case RoleType::CREATE_DATE:
        return file->createDate;
        break;
    case RoleType::FILE_IMAGE_URL:
        return file->imageUrl;
        break;
    case RoleType::DURATION:
        return file->duration;
        break;
    case RoleType::FILE_SUFFIX:
        return file->fileSuffix;
        break;
    }
    return QVariant();
}

int FileListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return fileList_.size();
}

int FileListModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QHash<int, QByteArray> FileListModel::roleNames() const
{
    QHash<int, QByteArray> map;
    map[RoleType::IS_TASK] = "isTask";
    map[RoleType::AUDIT_CONTENT_UUID] = "auditContentUuid";
    map[RoleType::VERSION_ID] = "versionId";
    map[RoleType::VERSION_UUID] = "versionUuid";
    map[RoleType::VERSION_NAME] = "versionName";
    map[RoleType::AUDIT_FILE_FOLDER_UUID] = "auditFileFolderUuid";
    map[RoleType::STATUS] = "status";
    map[RoleType::UUID] = "uuid";
    map[RoleType::UPLOAD_UUID] = "uploadUuid";
    map[RoleType::NAME] = "name";
    map[RoleType::FILE_SIZE] = "fileSize";
    map[RoleType::FILE_TYPE] = "fileType";
    map[RoleType::CREATE_DATE] = "createDate";
    map[RoleType::FILE_IMAGE_URL] = "fileImageUrl";
    map[RoleType::DURATION] = "duration";
    map[RoleType::FILE_SUFFIX] = "fileSuffix";
    return map;
}

FileEntity *FileListModel::getFile(int row)
{
    QModelIndex index = (QModelIndex)this->index(row, 0);
    return fileList_[index.row()];
}

//QVariant FileListModel::getRoleValue(int row, int col, QString role)
//{
//    if(row < 0 || row >= fileList_.size()) {
//        return "";
//    }
//    QModelIndex index = (QModelIndex)this->index(row, 0);

//    const FileEntity* file = fileList_[index.row()];
//    switch (roleNames().key(role.toUtf8())) {
//    case RoleType::IS_TASK:
//        return file->isTask;
//        break;
//    case RoleType::AUDIT_CONTENT_UUID:
//        return file->auditContentUuid;
//        break;
//    case RoleType::VERSION_ID:
//        return file->versionId;
//        break;
//    case RoleType::VERSION_UUID:
//        return file->versionUuid;
//        break;
//    case RoleType::VERSION_NAME:
//        return file->versionName;
//        break;
//    case RoleType::AUDIT_FILE_FOLDER_UUID:
//        return file->auditFileFolderUuid;
//        break;
//    case RoleType::STATUS:
//        return file->status;
//        break;
//    case RoleType::UUID:
//        return file->uuid;
//        break;
//    case RoleType::UPLOAD_UUID:
//        return file->uploadUuid;
//        break;
//    case RoleType::NAME:
//        return file->name;
//        break;
//    case RoleType::FILE_SIZE:
//        return file->fileSize;
//        break;
//    case RoleType::FILE_TYPE:
//        return file->fileType;
//        break;
//    case RoleType::CREATE_DATE:
//        return file->createDate;
//        break;
//    case RoleType::FILE_IMAGE_URL:
//        return file->imageUrl;
//        break;
//    case RoleType::DURATION:
//        return file->duration;
//        break;
//    case RoleType::FILE_SUFFIX:
//        return file->fileSuffix;
//        break;
//    }
//    return QVariant();
//}

//QModelIndex FileListModel::getModelIndex(int row, int col)
//{
//    return (QModelIndex)this->index(row, 0);
//}


void FileListModel::insert(FileEntity* file)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    fileList_.append(file);
    endInsertRows();
}

void FileListModel::remove(int row, int col, const QModelIndex &parent)
{
    QModelIndex index = (QModelIndex)this->index(row, col);
    FileEntity* file = fileList_[index.row()];
    beginRemoveRows(parent, row, 0);
    fileList_.removeAt(row);
    endRemoveRows();
    delete file;
}

void FileListModel::clear(const QModelIndex &parent)
{
    beginRemoveRows(parent, 0, fileList_.size() - 1);
    for (FileEntity* file: fileList_) {
        delete file;
    }
    fileList_.clear();
    endRemoveRows();
}

void FileListModel::updateList(Level level, QString taskUuid, QString folderUuid)
{
    clear();
    switch (level) {
    case Level::ROOT: {
        for (QString key: originArr_.keys()) {
            QJsonObject task = originArr_[key].toObject();
            FileEntity* file = new FileEntity();
            file->auditContentUuid = key;
            file->versionId = task.value("versionId").toInt();
            file->versionUuid = task.value("versionUuid").toString();
            file->versionName = task.value("versionName").toString();
            file->isTask = task.value("isTask").toBool();
            insert(file);
        }
        curLevel = Level::ROOT;
        break;
    }
    case Level::TASK: {
        QJsonObject targetTask = originArr_.value(taskUuid).toObject();
        QJsonObject files = targetTask.value("files").toObject();
        for (QString key: files.keys()) {
            QJsonObject fileObj= files[key].toObject();
            FileEntity* file = new FileEntity();
            file->auditContentUuid = taskUuid;
            file->versionId = fileObj.value("versionId").toInt();
            file->versionUuid = fileObj.value("versionUuid").toString();
            file->versionName = fileObj.value("versionName").toString();
            file->auditFileFolderUuid = key;
            file->status = fileObj.value("status").toString();
            file->uuid = fileObj.value("uuid").toString();
            file->uploadUuid = fileObj.value("uploadUuid").toString();
            file->name = fileObj.value("name").toString();
            file->fileSize = fileObj.value("fileSize").toString();
            file->fileType = fileObj.value("fileType").toInt();
            file->createDate = fileObj.value("createDate").toString();
            file->imageUrl = fileObj.value("fileImage").toString();
            file->duration = fileObj.value("duration").toString();
            file->fileSuffix = fileObj.value("fileSuffix").toString();
            file->isTask = fileObj.value("isTask").toBool();
            insert(file);
        }
        curLevel = Level::TASK;
        break;
    }
    case Level::FOLDER: {
        QJsonObject targetTask = originArr_.value(taskUuid).toObject();
        QJsonObject folderList = targetTask.value("files").toObject();
        QJsonObject targetFolder = folderList.value(folderUuid).toObject();
        if (targetFolder["attachment"].isNull()) {
            return;
        }
        QJsonObject attach = targetFolder["attachment"].toObject();
        FileEntity* file = new FileEntity();
        file->auditContentUuid = taskUuid;
        file->versionId = targetTask.value("versionId").toInt();
        file->versionUuid = targetTask.value("versionUuid").toString();
        file->versionName = targetTask.value("versionName").toString();
        file->auditFileFolderUuid = folderUuid;
        file->status = targetFolder.value("status").toString();
        file->uuid = attach.value("uuid").toString();
        file->uploadUuid = attach.value("uploadUuid").toString();
        file->name = attach.value("name").toString();
        file->fileSize = attach.value("fileSize").toString();
        file->fileType = attach.value("fileType").toInt();
        file->createDate = attach.value("createDate").toString();
        file->imageUrl = attach.value("fileImage").toString();
        file->duration = attach.value("duration").toString();
        file->fileSuffix = attach.value("fileSuffix").toString();
        file->isTask = attach.value("isTask").toBool();
        insert(file);
        curLevel = Level::FOLDER;
        break;
    }
    }
}

void FileListModel::insertCheckedList(int checkedRow)
{
    checkedList_.append(checkedRow);
    checkedListChanged();
}

void FileListModel::checkAll()
{
    for (int i = 0; i < fileList_.size(); i++) {
        if (checkedList_.contains(i)) {
            continue;
        }
        checkedList_.append(i);
    }
    checkedListChanged();
}

void FileListModel::uncheckAll()
{
    checkedList_.clear();
    checkedListChanged();
}

bool FileListModel::isCheckedAll()
{
    return checkedList_.size() != 0 && checkedList_.size() == fileList_.size();
}

bool FileListModel::hasChecked()
{
    return checkedList_.size() != 0;
}

void FileListModel::removeCheckedList(int checkedRow)
{
    int index = checkedList_.indexOf(checkedRow);
    checkedList_.removeAt(index);
    checkedListChanged();
}

void FileListModel::clearCheckedList()
{
    checkedList_.clear();
    checkedListChanged();
}

bool FileListModel::checkedListContains(int checkedRow)
{
    return checkedList_.indexOf(checkedRow) == -1 ? false : true;
}

QJsonObject FileListModel::groupChekedFile()
{
    QJsonObject ret;
    switch (curLevel) {
    case ROOT:
        break;
    case TASK: {
        QJsonObject ret;
        for (int row: checkedList_) {
            // 按ContentUuid和versionUuid分组
            FileEntity* file = fileList_.at(row);
            if (!ret.contains(file->auditContentUuid)) {
                QJsonArray folderArr;
                folderArr.append(file->auditFileFolderUuid);
                QJsonObject versions;
                versions[file->versionUuid] = folderArr;
                ret[file->auditContentUuid] = versions;
            }
            else {
                QJsonObject retVersions = ret.value(file->auditContentUuid).toObject();
                if (!retVersions.contains(file->versionUuid)) {
                    QJsonArray folderArr;
                    folderArr.append(file->auditFileFolderUuid);
                    retVersions.insert(file->versionUuid, folderArr);
                    ret[file->auditContentUuid] = retVersions;
                }
                else {
                    QJsonArray retFolderArr = retVersions.value(file->versionUuid).toArray();
                    retFolderArr.append(file->auditFileFolderUuid);
                    retVersions.insert(file->versionUuid, retFolderArr);
                    ret[file->auditContentUuid] = retVersions;
                }
            }
        }
        return ret;
        break;
    }
    case FOLDER: {
        QJsonObject tasks;
        QJsonObject versions;
        QJsonArray folders;
        for (int row: checkedList_) {
            FileEntity* file = fileList_.at(row);
            folders.append(file->auditFileFolderUuid);
            versions[file->versionUuid] = folders;
            tasks[file->auditContentUuid] = versions;
        }
        ret = tasks;
        break;
    }
    }
    return ret;
}

void FileListModel::onProjectFileReplyDone(bool success, QJsonObject taskList)
{
    if (!fileList_.isEmpty()) {
        clear();
    }
    if (!checkedList_.isEmpty()) {
        clearCheckedList();
    }
    if (!success) {
        return;
    }
    originArr_ = taskList;
    updateList(Level::ROOT);
}

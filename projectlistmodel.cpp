#include "projectlistmodel.h"
#include <QJsonObject>

ProjectListModel::ProjectListModel()
{

}

QVariant ProjectListModel::data(const QModelIndex &index, int role) const
{
    if(index.row() < 0 || index.row() >= projects.size()) {
        return QVariant();
    }
    const Project* proj = projects[index.row()];
    switch (role) {
    case RoleType::ID:
        return proj->id;
        break;
    case RoleType::UUID:
        return proj->uuid;
        break;
    case RoleType::NAME:
        return proj->name;
        break;
    case RoleType::COMPANY:
        return proj->companyName;
        break;
    case RoleType::CREATE_DATE:
        return proj->createDate;
        break;
    case RoleType::START_TIME:
        return proj->startTime;
        break;
    case RoleType::END_TIME:
        return proj->endTime;
        break;
    case RoleType::NOTE:
        return proj->note;
        break;
    case RoleType::STATE:
        return proj->state;
        break;
    case RoleType::TYPE:
        return proj->type;
        break;
    case RoleType::IMAGE_URL:
        return proj->imageUrl;
        break;
    }
    return QVariant();
}

int ProjectListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return projects.size();
}

QHash<int, QByteArray> ProjectListModel::roleNames() const
{
    QHash<int, QByteArray> map;
    map[RoleType::ID] = "id";
    map[RoleType::UUID] = "uuid";
    map[RoleType::NAME] = "name";
    map[RoleType::COMPANY] = "company";
    map[RoleType::CREATE_DATE] = "createDate";
    map[RoleType::START_TIME] = "startTime";
    map[RoleType::END_TIME] = "endTime";
    map[RoleType::NOTE] = "note";
    map[RoleType::STATE] = "state";
    map[RoleType::TYPE] = "type";
    map[RoleType::IMAGE_URL] = "imageUrl";
    return map;
}

Project *ProjectListModel::getProject(int row)
{
    QModelIndex index = (QModelIndex)this->index(row);
    return projects[index.row()];
}

QModelIndex ProjectListModel::getModelIndex(int row)
{
    return (QModelIndex)this->index(row);
}

QVariant ProjectListModel::getRoleValue(int row, QString role)
{
    if(row < 0 || row >= projects.size()) {
        return "";
    }
    QModelIndex index = (QModelIndex)this->index(row);

    const Project* proj = projects[index.row()];
    switch (roleNames().key(role.toUtf8())) {
    case RoleType::ID:
        return proj->id;
        break;
    case RoleType::UUID:
        return proj->uuid;
        break;
    case RoleType::NAME:
        return proj->name;
        break;
    case RoleType::COMPANY:
        return proj->companyName;
        break;
    case RoleType::CREATE_DATE:
        return proj->createDate;
        break;
    case RoleType::START_TIME:
        return proj->startTime;
        break;
    case RoleType::END_TIME:
        return proj->endTime;
        break;
    case RoleType::NOTE:
        return proj->note;
        break;
    case RoleType::STATE:
        return proj->state;
        break;
    case RoleType::TYPE:
        return proj->type;
        break;
    case RoleType::IMAGE_URL:
        return proj->imageUrl;
        break;
    }
    return QVariant();
}

void ProjectListModel::insert(Project *proj)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    projects.append(proj);
    endInsertRows();
}

void ProjectListModel::remove(int row, const QModelIndex &parent)
{
    QModelIndex index = (QModelIndex)this->index(row);
    Project* proj = projects[index.row()];
    beginRemoveRows(parent, row, row);
    projects.removeAt(row);
    endRemoveRows();
    delete proj;
}

void ProjectListModel::clear(const QModelIndex &parent)
{
    beginRemoveRows(parent, 0, projects.size() - 1);
    for (Project* proj: projects) {
        delete proj;
    }
    projects.clear();
    endRemoveRows();
}

void ProjectListModel::onProjectReplyDone(bool success, QJsonArray projectList)
{
    clear();
    if (!success) {
        return;
    }
    for (int i = 0; i < projectList.size(); i++) {
        QJsonObject obj = projectList[i].toObject();
        Project* proj = new Project();
        proj->id = obj.value("id").toInt();
        proj->uuid = obj.value("projectUuid").toString();
        proj->name = obj.value("projectName").toString();
        proj->createDate = obj.value("createDate").toVariant().toULongLong();
        proj->startTime = obj.value("startTimer").toVariant().toULongLong();
        proj->endTime = obj.value("endTimer").toVariant().toULongLong();
        proj->note = obj.value("projectNote").toString();
        proj->state = obj.value("projectState").toString();
        proj->type = obj.value("projectType").toString();
        proj->imageUrl = obj.value("projectImage").toString();
        insert(proj);
    }
}

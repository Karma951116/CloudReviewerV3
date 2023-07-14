#include "stakeholderlistmodel.h"

StakeholderListModel::StakeholderListModel()
{

}

QModelIndex StakeholderListModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return createIndex(row, column);
}

QModelIndex StakeholderListModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);
    return QModelIndex();
}

QVariant StakeholderListModel::data(const QModelIndex &index, int role) const
{
    if(index.row() < 0 || index.row() >= stakeholderList_.size()) {
        return QVariant();
    }
    const Stakeholder* stakeholder = stakeholderList_[index.row()];
    switch (role) {
    case RoleType::STAKEHOLDER_ID:
        return stakeholder->id;
        break;
    case RoleType::NAME:
        return stakeholder->name;
        break;
    case RoleType::UUID:
        return stakeholder->uuid;
        break;
    case RoleType::IMAGE_URL:
        return stakeholder->imageUrl;
        break;
    }
    return QVariant();
}

int StakeholderListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return stakeholderList_.size();
}

int StakeholderListModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QHash<int, QByteArray> StakeholderListModel::roleNames() const
{
    QHash<int, QByteArray> map;
    map[RoleType::STAKEHOLDER_ID] = "stakeholderId";
    map[RoleType::UUID] = "uuid";
    map[RoleType::NAME] = "name";
    map[RoleType::IMAGE_URL] = "imageUrl";
    return map;
}

void StakeholderListModel::insert(Stakeholder *stakeholder)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    stakeholderList_.append(stakeholder);
    endInsertRows();
}

void StakeholderListModel::remove(int row, int col, const QModelIndex &parent)
{
    QModelIndex index = (QModelIndex)this->index(row, col);
    Stakeholder* sk = stakeholderList_[index.row()];
    beginRemoveRows(parent, row, 0);
    stakeholderList_.removeAt(row);
    endRemoveRows();
    delete sk;
}

void StakeholderListModel::clear(const QModelIndex &parent)
{
    beginRemoveRows(parent, 0, stakeholderList_.size() - 1);
    for (Stakeholder* sk: stakeholderList_) {
        delete sk;
    }
    stakeholderList_.clear();
    endRemoveRows();
}

QVariant StakeholderListModel::getRoleValue(int row, QString role)
{
    if(row < 0 || row >= stakeholderList_.size()) {
        return "";
    }
    QModelIndex index = (QModelIndex)this->index(row, 0);
    const Stakeholder* stakeholder = stakeholderList_[index.row()];
    switch (roleNames().key(role.toUtf8())) {
    case RoleType::STAKEHOLDER_ID:
        return stakeholder->id;
        break;
    case RoleType::NAME:
        return stakeholder->name;
        break;
    case RoleType::UUID:
        return stakeholder->uuid;
        break;
    case RoleType::IMAGE_URL:
        return stakeholder->imageUrl;
        break;
    }
    return QVariant();
}

void StakeholderListModel::onStakeholderReplyDone(bool success, QJsonArray stakeholderList)
{
    if (!success) {
        return;
    }
    clear();
    for (int i = 0; i < stakeholderList.size(); i++) {
        QJsonObject obj = stakeholderList[i].toObject();
        Stakeholder* sk = new Stakeholder();
        sk->id = obj.value("id").toInt();
        sk->name = obj.value("name").toString();
        sk->uuid = obj.value("uuid").toString();
        sk->imageUrl = obj.value("image").toString();
        insert(sk);
    }
}

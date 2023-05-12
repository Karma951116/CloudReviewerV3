#include "companylistmodel.h"


#include <QJsonObject>

CompanyListModel::CompanyListModel()
{

}

QVariant CompanyListModel::data(const QModelIndex &index, int role) const
{
    if(index.row() < 0 || index.row() >= companies.size()) {
        return QVariant();
    }
    const Company* comp = companies[index.row()];
    switch (role) {
    case RoleType::ID:
        return comp->id;
        break;
    case RoleType::UUID:
        return comp->uuid;
        break;
    case RoleType::NAME:
        return comp->name;
        break;
    }
    return QVariant();
}

int CompanyListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return companies.size();
}

QHash<int, QByteArray> CompanyListModel::roleNames() const
{
    QHash<int, QByteArray> map;
    map[RoleType::ID] = "id";
    map[RoleType::UUID] = "uuid";
    map[RoleType::NAME] = "name";
    return map;
}

Company *CompanyListModel::getCompany(int row)
{
    QModelIndex index = (QModelIndex)this->index(row);
    return companies[index.row()];
}

QVariant CompanyListModel::getRoleValue(int row, QString role)
{
    if(row < 0 || row >= companies.size()) {
        return "";
    }
    QModelIndex index = (QModelIndex)this->index(row);

    const Company* comp = companies[index.row()];
    switch (roleNames().key(role.toUtf8())) {
    case RoleType::ID:
        return comp->id;
        break;
    case RoleType::UUID:
        return comp->uuid;
        break;
    case RoleType::NAME:
        return comp->name;
        break;
    }
    return QVariant();
}

QModelIndex CompanyListModel::getModelIndex(int row)
{
    return (QModelIndex)this->index(row);
}

void CompanyListModel::insert(Company* comp)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    companies.append(comp);
    endInsertRows();
}

void CompanyListModel::remove(int row, const QModelIndex &parent)
{
    QModelIndex index = (QModelIndex)this->index(row);
    Company* comp = companies[index.row()];
    beginRemoveRows(parent, row, row);
    companies.removeAt(row);
    endRemoveRows();
    delete comp;
}

void CompanyListModel::clear(const QModelIndex &parent)
{
    beginRemoveRows(parent, 0, companies.size() - 1);
    for (Company* comp: companies) {
        delete comp;
    }
    companies.clear();
    endRemoveRows();
}

void CompanyListModel::onCompanyReplyDone(bool success, QJsonArray companyList)
{
    if (!success) {
        return;
    }
    clear();
    for (int i = 0; i < companyList.size(); i++) {
        QJsonObject obj = companyList[i].toObject();
        Company* comp = new Company();
        comp->id = obj.value("id").toInt();
        comp->uuid = obj.value("uuid").toString();
        comp->name = obj.value("name").toString();
        insert(comp);
    }
}

#ifndef STAKEHOLDERLISTMODEL_H
#define STAKEHOLDERLISTMODEL_H

#include <QObject>
#include <QAbstractListModel>

#include "def.h"

class StakeholderListModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum RoleType {
        STAKEHOLDER_ID,
        NAME,
        UUID,
        IMAGE_URL
    };
    StakeholderListModel();

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

    void insert(Stakeholder* stakeholder);
    void remove(int row, int col, const QModelIndex &parent = QModelIndex());
    void clear(const QModelIndex &parent = QModelIndex());

    Q_INVOKABLE QVariant getRoleValue(int row, QString role);

private:
    QList<Stakeholder*> stakeholderList_;
public slots:
    void onStakeholderReplyDone(bool success, QJsonArray stakeholderList);
};

#endif //STAKEHOLDERLISTMODEL_H

#ifndef COMPANYLISTMODEL_H
#define COMPANYLISTMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QJsonArray>
#include "def.h"

class CompanyListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum RoleType {
        ID,
        UUID,
        NAME
    };

    CompanyListModel();
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QHash<int, QByteArray> roleNames() const;
    Company* getCompany(int row);
    void insert(Company* comp);
    void remove(int row, const QModelIndex &parent = QModelIndex());
    void clear(const QModelIndex &parent = QModelIndex());

    Q_INVOKABLE QVariant getRoleValue(int row, QString role);
    Q_INVOKABLE QModelIndex getModelIndex(int row);

private:
    QList<Company*> companies;

public slots:
    void onCompanyReplyDone(bool success, QJsonArray companyList);
};

#endif // COMPANYLISTMODEL_H

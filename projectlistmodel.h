#ifndef PROJECTLISTMODEL_H
#define PROJECTLISTMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include "def.h"
#include <QJsonArray>

class ProjectListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum RoleType {
        ID,
        UUID,
        NAME,
        COMPANY,
        CREATE_DATE,
        START_TIME,
        END_TIME,
        NOTE,
        STATE,
        TYPE,
        IMAGE_URL
    };
    ProjectListModel();
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QHash<int, QByteArray> roleNames() const;
    Project* getProject(int row);
    Q_INVOKABLE QModelIndex getModelIndex(int row);
    Q_INVOKABLE QVariant getRoleValue(int row, QString role);
    void insert(Project* proj);
    void remove(int row, const QModelIndex &parent = QModelIndex());
    void clear(const QModelIndex &parent = QModelIndex());

private:
    QList<Project*> projects;
public slots:
    void onProjectReplyDone(bool success, QJsonArray projectList);

};

#endif // PROJECTLISTMODEL_H

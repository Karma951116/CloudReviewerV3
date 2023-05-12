#ifndef FILELISTMODEL_H
#define FILELISTMODEL_H

#include <QObject>
#include <QAbstractItemModel>
#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include <def.h>

class FileListModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum RoleType {
        IS_TASK,
        AUDIT_CONTENT_UUID,
        VERSION_ID,
        VERSION_UUID,
        VERSION_NAME,
        AUDIT_FILE_FOLDER_UUID,
        STATUS,
        UUID,
        UPLOAD_UUID,
        NAME,
        FILE_SIZE,
        FILE_TYPE,
        CREATE_DATE,
        FILE_IMAGE_URL,
        DURATION,
        FILE_SUFFIX
    };
    enum Level{
        ROOT,
        TASK,
        FOLDER
    };
    Q_ENUMS(Level);
    FileListModel();

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;
    Q_INVOKABLE FileEntity* getFile(int row);
    //Q_INVOKABLE QVariant getRoleValue(int row, int col, QString role);
    //Q_INVOKABLE QModelIndex getModelIndex(int row, int col);
    void insert(FileEntity* files);
    void remove(int row, int col, const QModelIndex &parent = QModelIndex());
    void clear(const QModelIndex &parent = QModelIndex());
    Q_INVOKABLE void updateList(Level level, QString taskUuid = nullptr,
                                QString folderUuid = nullptr);
    Q_INVOKABLE void insertCheckedList(int checkedRow);
    Q_INVOKABLE void checkAll();
    Q_INVOKABLE void uncheckAll();
    Q_INVOKABLE bool isCheckedAll();
    Q_INVOKABLE bool hasChecked();
    Q_INVOKABLE void removeCheckedList(int checkedRow);
    Q_INVOKABLE void clearCheckedList();
    Q_INVOKABLE bool checkedListContains(int checkedRow);
    Q_INVOKABLE QJsonObject groupChekedFile();

signals:
    void checkedListChanged();

public slots:
    void onProjectFileReplyDone(bool success, QJsonObject taskList);

private:
    QList<FileEntity*> fileList_;
    QJsonObject originArr_;
    QList<int> checkedList_;
    Level curLevel;
};

#endif // FILELISTMODEL_H

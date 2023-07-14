#ifndef COMMENTLISTMODEL_H
#define COMMENTLISTMODEL_H

#include <QObject>
#include <QAbstractListModel>

#include "def.h"

class CommentListModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum RoleType {
        COMMENT_UUID,
        PARENT_UUID,
        SENDER,
        RECEIVER,
        DETAILS,
        CREATE_DATE,
        OPERATION_TYPE,
        PRINTSCREEN,
        STAKEHOLDER,
        START,
        END,
        CHILDRENS
    };

    CommentListModel();
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;
    void insert(Comment* comment);
    void remove(int row, int col, const QModelIndex &parent = QModelIndex());
    void clear(const QModelIndex &parent = QModelIndex());

    Q_INVOKABLE QString hoveredComment() const;
    Q_INVOKABLE void setHoveredComment(const QString &hoveredComment);
    Q_INVOKABLE int commentCount();
    Q_INVOKABLE QVariant getRoleValue(int row, QString role);

    Q_INVOKABLE QJsonArray getCommentsOringin() const;

private:
    QList<Comment*> comments_;
    QString hoveredComment_;
    QJsonArray commentsOringin_;

public slots:
    void onCommentReplyDone(bool success, QJsonArray commentList);

signals:
    void hoveredCommentChanged();
};

#endif // COMMENTLISTMODEL_H

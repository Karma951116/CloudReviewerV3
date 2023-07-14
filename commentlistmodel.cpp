#include "commentlistmodel.h"

CommentListModel::CommentListModel()
{

}

QModelIndex CommentListModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return createIndex(row, column);
}

QModelIndex CommentListModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);
    return QModelIndex();
}

QVariant CommentListModel::data(const QModelIndex &index, int role) const
{
    if(index.row() < 0 || index.row() >= comments_.size()) {
        return QVariant();
    }
    const Comment* comment = comments_[index.row()];
    switch (role) {
    case RoleType::COMMENT_UUID:
        return comment->commentUuid;
        break;
    case RoleType::PARENT_UUID:
        return comment->parentUuid;
        break;
    case RoleType::SENDER:
        return comment->sender;
        break;
    case RoleType::RECEIVER:
        return comment->receiver;
        break;
    case RoleType::DETAILS:
        return comment->details;
        break;
    case RoleType::CREATE_DATE:
        return comment->createDate;
        break;
    case RoleType::OPERATION_TYPE:
        return comment->operationType;
        break;
    case RoleType::PRINTSCREEN:
        return comment->printScreen;
        break;
    case RoleType::STAKEHOLDER:
        return comment->stakeHolder;
        break;
    case RoleType::START:
        return comment->start;
        break;
    case RoleType::END:
        return comment->end;
        break;
    case RoleType::CHILDRENS:
        return comment->childrens;
        break;
    }
    return QVariant();
}

int CommentListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return comments_.size();
}

int CommentListModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QHash<int, QByteArray> CommentListModel::roleNames() const
{
    QHash<int, QByteArray> map;
    map[RoleType::COMMENT_UUID] = "commentUuid";
    map[RoleType::PARENT_UUID] = "parentUuid";
    map[RoleType::SENDER] = "sender";
    map[RoleType::RECEIVER] = "receiver";
    map[RoleType::DETAILS] = "details";
    map[RoleType::CREATE_DATE] = "createDate";
    map[RoleType::OPERATION_TYPE] = "operationType";
    map[RoleType::PRINTSCREEN] = "printscreen";
    map[RoleType::STAKEHOLDER] = "stakeholder";
    map[RoleType::START] = "start";
    map[RoleType::END] = "end";
    map[RoleType::CHILDRENS] = "childrens";
    return map;
}

void CommentListModel::insert(Comment *comment)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    comments_.append(comment);
    endInsertRows();
}

void CommentListModel::remove(int row, int col,const QModelIndex &parent)
{
    QModelIndex index = (QModelIndex)this->index(row, col);
    Comment* comment = comments_[index.row()];
    beginRemoveRows(parent, row, 0);
    comments_.removeAt(row);
    endRemoveRows();
    delete comment;
}

void CommentListModel::clear(const QModelIndex &parent)
{
    beginRemoveRows(parent, 0, comments_.size() - 1);
    for (Comment* comment: comments_) {
        delete comment;
    }
    comments_.clear();
    endRemoveRows();
}

QString CommentListModel::hoveredComment() const
{
    return hoveredComment_;
}

void CommentListModel::setHoveredComment(const QString &hoveredComment)
{
    hoveredComment_ = hoveredComment;
    hoveredCommentChanged();
}

int CommentListModel::commentCount()
{
    return comments_.size();
}

QVariant CommentListModel::getRoleValue(int row, QString role)
{
    if(row < 0 || row >= comments_.size()) {
        return "";
    }
    QModelIndex index = (QModelIndex)this->index(row, 0);
    const Comment* comment = comments_[index.row()];
    switch (roleNames().key(role.toUtf8())) {
    case RoleType::COMMENT_UUID:
        return comment->commentUuid;
        break;
    case RoleType::PARENT_UUID:
        return comment->parentUuid;
        break;
    case RoleType::SENDER:
        return comment->sender;
        break;
    case RoleType::RECEIVER:
        return comment->receiver;
        break;
    case RoleType::DETAILS:
        return comment->details;
        break;
    case RoleType::CREATE_DATE:
        return comment->createDate;
        break;
    case RoleType::OPERATION_TYPE:
        return comment->operationType;
        break;
    case RoleType::PRINTSCREEN:
        return comment->printScreen;
        break;
    case RoleType::STAKEHOLDER:
        return comment->stakeHolder;
        break;
    case RoleType::START:
        return comment->start;
        break;
    case RoleType::END:
        return comment->end;
        break;
    case RoleType::CHILDRENS:
        return comment->childrens;
        break;
    }
    return QVariant();
}

QJsonArray CommentListModel::getCommentsOringin() const
{
    return commentsOringin_;
}

void CommentListModel::onCommentReplyDone(bool success, QJsonArray commentList)
{
    if (!success) {
        return;
    }
    clear();
    commentsOringin_ = commentList;
    for(int i = 0; i < commentList.size(); i++) {
        QJsonObject obj = commentList[i].toObject();
        Comment* comment = new Comment();
        comment->commentUuid = obj.value("commentUuid").toString();
        comment->parentUuid = obj.value("parentUuid").toString();
        comment->sender = obj.value("commentSendUser").toObject();
        comment->receiver = obj.value("commentTheReceivingUser").toObject();
        comment->details = obj.value("commentDetails").toString();
        comment->createDate = obj.value("createDate").toString();
        comment->operationType = obj.value("operationType").toInt();
        comment->printScreen = obj.value("printScreen").toString();
        comment->start = obj.value("start").toString();
        comment->end = obj.value("end").toString();
        comment->childrens = obj.value("childrens").toArray();
        insert(comment);
    }
}

#include "user.h"

User::User(QObject *parent) : QObject(parent)
{

}

int User::id() const
{
    return id_;
}

QString User::uuid() const
{
    return uuid_;
}

QString User::email() const
{
    return email_;
}

QString User::sex() const
{
    return sex_;
}

QString User::phone() const
{
    return phone_;
}

QString User::birthday() const
{
    return birthday_;
}

QString User::userName() const
{
    return userName_;
}

QString User::fullName() const
{
    return fullName_;
}

QString User::nickName() const
{
    return nickName_;
}

QString User::login() const
{
    return login_;
}

QString User::role() const
{
    return role_;
}

QJsonArray User::ability() const
{
    return ability_;
}

QString User::avatarUrl() const
{
    return avatarUrl_;
}

QJsonObject User::extras() const
{
    return extras_;
}

void User::onUpdateUser(QJsonObject userData)
{
    id_ = userData.value("id").toInt();
    uuid_ = userData.value("uuid").toString();
    email_ = userData.value("email").toString();
    sex_ = userData.value("sex").toString();
    phone_ = userData.value("phone").toString();
    birthday_ = userData.value("birth").toString();

    userName_ = userData.value("username").toString();
    fullName_ = userData.value("fullName").toString();
    nickName_ = userData.value("nickname").toString();
    login_ = userData.value("login").toString();
    role_ = userData.value("role").toString();
    ability_ = userData.value("ability").toArray();
    avatarUrl_ = userData.value("avatar").toString();
    avatarUrl_ = userData.value("avatar").toString();
    headImageUrl_ = userData.value("user_head_img").toString();
    extras_ = userData.value("extras_").toObject();

    userNameChanged();
    roleChanged();
}

void User::onUpdateUserImage(QImage headImage, QImage avatarImage)
{

}



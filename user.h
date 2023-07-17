#ifndef USER_H
#define USER_H

#include <QObject>
#include <QJsonArray>
#include <QJsonObject>
#include <QImage>
#include <httpfunctions.h>

class User : public QObject
{
    Q_OBJECT
public:
    explicit User(QObject* parent = nullptr);

    Q_PROPERTY(QString userName READ userName NOTIFY userNameChanged)
    Q_PROPERTY(QString role READ role NOTIFY roleChanged)
    Q_INVOKABLE int id() const;

    Q_INVOKABLE QString uuid() const;

    Q_INVOKABLE QString email() const;

    Q_INVOKABLE QString sex() const;

    Q_INVOKABLE QString phone() const;

    Q_INVOKABLE QString birthday() const;

    Q_INVOKABLE QString userName() const;

    Q_INVOKABLE QString fullName() const;

    Q_INVOKABLE QString nickName() const;

    Q_INVOKABLE QString login() const;

    Q_INVOKABLE QString role() const;

    Q_INVOKABLE QJsonArray ability() const;

    Q_INVOKABLE QString avatarUrl() const;

    Q_INVOKABLE QJsonObject extras() const;

private:
    int id_;
    QString uuid_;
    QString email_;
    QString sex_;
    QString phone_;
    QString birthday_;
    QString userName_;
    QString fullName_;
    QString nickName_;
    QString login_;
    QString role_;
    QJsonArray ability_;
    QString avatarUrl_;
    QString headImageUrl_;
    QJsonObject extras_;

    QImage headImage_;
    QImage avatarImage_;

signals:
    void userNameChanged();
    void roleChanged();
public slots:
    void onUpdateUser(QJsonObject userData);
    void onUpdateUserImage(QImage headImage, QImage avatarImage);
};

#endif // USER_H

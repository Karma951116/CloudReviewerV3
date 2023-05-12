#ifndef PROMPTPROVIDER_H
#define PROMPTPROVIDER_H

#include <QObject>
#include <QJsonObject>

class PromptProvider : public QObject
{
    Q_OBJECT
public:
    explicit PromptProvider(QObject *parent = nullptr);
    Q_INVOKABLE QString getPrompt(QString type, QString model, QString name);
signals:
private:
    QJsonObject obj;
};

#endif // PROMPTPROVIDER_H

#include "promptprovider.h"

#include <QTextCodec>
#include <QCoreApplication>

#include <QFile>
#include <QDebug>
#include <QJsonParseError>

PromptProvider::PromptProvider(QObject *parent) : QObject(parent)
{
    //QString fileName = QCoreApplication::applicationDirPath() + "/Prompt.json";
    QFile f(":/settings/Prompt");
    if (!f.open(QIODevice::ReadOnly)) {
        qDebug() << "提示语载入失败";
    }
    QByteArray data = f.readAll();
    f.close();
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError) {
        qDebug() << "提示语载入失败";
    }
    obj = doc.object();
}

QString PromptProvider::getPrompt(QString type, QString model, QString name)
{
    if (!obj.contains(type)) {
        return nullptr;
    }
    QJsonValue typeValue = obj[type];
    if (!typeValue.toObject().contains(model)) {
        return nullptr;
    }
    QJsonValue modelValue = typeValue.toObject()[model];
    if (!modelValue.toObject().contains(name)) {
        return nullptr;
    }
    QString value = modelValue.toObject()[name].toString();
    return value;
}

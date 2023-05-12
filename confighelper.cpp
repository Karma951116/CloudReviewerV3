#include <QTextCodec>
#include <QCoreApplication>
#include <QDir>

#include "confighelper.h"

ConfigHelper::ConfigHelper(QString configName, QObject *parent) : QObject(parent)
{
    QString fileName = QCoreApplication::applicationDirPath() + "/" + configName;
    setting_ = new QSettings(fileName, QSettings::IniFormat);
    setting_->setIniCodec(QTextCodec::codecForName("UTF-8"));
    if (setting_->status() != QSettings::NoError &&
        setting_ != nullptr) {
        setting_ = nullptr;
    }
//    QDir dir;
//    if (!dir.exists(fileName)) {
//        dir.mkdir(fileName);
//        setConfig("Login", "KeepPwd", false);
//        setConfig("Network", "DebugIp", "127.0.0.1");
//        setConfig("Network", "ProdIp", "");
//    }
}

void ConfigHelper::setConfig(QString nodeName, QString keyName, QVariant value)
{
    setting_->beginGroup(nodeName);
    setting_->setValue(keyName, value.toString());
    setting_->endGroup();
    setting_->sync();
}

bool ConfigHelper::getBoolConfig(QString nodeName, QString keyName)
{
    return getConfig(nodeName, keyName).toBool();
}

QString ConfigHelper::getStrConfig(QString nodeName, QString keyName)
{
    return getConfig(nodeName, keyName).toString();
}

QVariant ConfigHelper::getConfig(QString nodeName, QString keyName)
{
    setting_->beginGroup(nodeName);
    QVariant qVar = setting_->value(keyName);
    setting_->endGroup();
    return qVar;
}

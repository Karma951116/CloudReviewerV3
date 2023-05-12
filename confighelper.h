#ifndef CONFIGHELPER_H
#define CONFIGHELPER_H

#include <QObject>
#include <QSettings>

class ConfigHelper : public QObject
{
    Q_OBJECT
public:
    explicit ConfigHelper(QString configName, QObject *parent = nullptr);
    Q_INVOKABLE void setConfig(QString nodeName, QString keyName, QVariant value);
    Q_INVOKABLE bool getBoolConfig(QString nodeName, QString keyName);
    Q_INVOKABLE QString getStrConfig(QString nodeName, QString keyName);

private:
    QSettings* setting_;
    /// Inner function for basic QVariant parameter.
    /// Use public function to get other type value.
    /// \brief getConfig
    /// \param nodeName
    /// \param keyName
    /// \return QVariant value
    ///
    QVariant getConfig(QString nodeName, QString keyName);
};

#endif // CONFIGHELPER_H

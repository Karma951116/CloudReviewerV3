#ifndef FORMATCHECKER_H
#define FORMATCHECKER_H

#include <QObject>

class FormatChecker : public QObject
{
    Q_OBJECT
public:
    explicit FormatChecker(QObject *parent = nullptr);

    Q_INVOKABLE bool accountCheck(QString accountStr);
    Q_INVOKABLE bool pwdCheck(QString pwdStr);
    Q_INVOKABLE bool phoneCheck(QString phoneStr);
    Q_INVOKABLE bool pinCheck(QString pinStr);

signals:

};

#endif // FORMATCHECKER_H

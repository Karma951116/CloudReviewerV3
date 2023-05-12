#include "formatchecker.h"

#include <QRegularExpression>

FormatChecker::FormatChecker(QObject *parent) : QObject(parent)
{

}

bool FormatChecker::accountCheck(QString accountStr)
{
    if (accountStr.length() < 8 || accountStr.length() > 14) {
        return false;
    }
    int types = 0;
    QRegularExpression re;
    re.setPattern("[0-9]");
    if (re.match(accountStr).hasMatch()) {
        types++;
    }
    re.setPattern("[A-Z]");
    if (re.match(accountStr).hasMatch()) {
        types++;
    }
    re.setPattern("[a-z]");
    if (re.match(accountStr).hasMatch()) {
        types++;
    }
    re.setPattern("_");
    if (re.match(accountStr).hasMatch()) {
        types++;
    }
    return types >= 3 ? true : false;
}

bool FormatChecker::pwdCheck(QString pwdStr)
{
    if (pwdStr.length() < 8 || pwdStr.length() > 18) {
        return false;
    }
    int types = 0;
    QRegularExpression re;
    re.setPattern("[0-9]");
    if (re.match(pwdStr).hasMatch()) {
        types++;
    }
    re.setPattern("[A-Z]");
    if (re.match(pwdStr).hasMatch()) {
        types++;
    }
    re.setPattern("[a-z]");
    if (re.match(pwdStr).hasMatch()) {
        types++;
    }
    re.setPattern("\\W");
    if (re.match(pwdStr).hasMatch()) {
        types++;
    }
    return types >= 3 ? true : false;
}

bool FormatChecker::phoneCheck(QString phoneStr)
{
    if (phoneStr.length() != 11) {
        return false;
    }
    QRegularExpression re;
    re.setPattern("^1[3-9]\\d{9}$");
    if (re.match(phoneStr).hasMatch()) {
        return true;
    }
    else return false;
}

bool FormatChecker::pinCheck(QString pinStr)
{
    if (pinStr.length() != 6) {
        return false;
    }
    QRegularExpression re;
    re.setPattern("^\\d{6}$");
    if (re.match(pinStr).hasMatch()) {
        return true;
    }
    else return false;
}



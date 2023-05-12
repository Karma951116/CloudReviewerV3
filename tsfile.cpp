#include "tsfile.h"

TsFile::TsFile(QObject *parent) : QObject(parent)
{

}

QString TsFile::fileName() const
{
    return fileName_;
}

void TsFile::setFileName(const QString &fileName)
{
    fileName_ = fileName;
}

double TsFile::duration() const
{
    return duration_;
}

void TsFile::setDuration(double duration)
{
    duration_ = duration;
}


QString TsFile::url() const
{
    return url_;
}

void TsFile::setUrl(const QString &url)
{
    url_ = url;
}

unsigned char *TsFile::data()
{
    return data_;
}

void TsFile::setData(uint8_t *data)
{
    data_ = data;
}

int TsFile::dataLength() const
{
    return dataLength_;
}

void TsFile::setDataLength(int dataLength)
{
    dataLength_ = (size_t)dataLength;
}

bool TsFile::decoded() const
{
    return decoded_;
}

void TsFile::setDecoded(bool decoded)
{
    QWriteLocker locker(&lock_);
    decoded_ = decoded;
    decodeChanged(decoded_);
}

bool TsFile::fetched() const
{
    return fetched_;
}

void TsFile::setFetched(bool fetched)
{
    QWriteLocker locker(&lock_);
    fetched_ = fetched;
    fetchChanged(fetched_);
}

int TsFile::tsIndex() const
{
    return tsIndex_;
}

void TsFile::setTsIndex(int tsIndex)
{
    tsIndex_ = tsIndex;
}


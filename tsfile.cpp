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

int TsFile::tsIndex() const
{
    return tsIndex_;
}

void TsFile::setTsIndex(int tsIndex)
{
    tsIndex_ = tsIndex;
}

TsFile::TsState TsFile::fetchState() const
{
    return fetchState_;
}

void TsFile::setFetchState(const TsState &fetchState)
{
    QWriteLocker locker(&lock_);
    fetchState_ = fetchState;
    fetchChanged(fetchState);
}

TsFile::DecodeState TsFile::decodeState() const
{
    return decodeState_;
}

void TsFile::setDecodeState(const DecodeState &decodeState)
{
    QWriteLocker locker(&lock_);
    decodeState_ = decodeState;
}


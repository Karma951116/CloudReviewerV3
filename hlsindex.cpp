#include "hlsindex.h"

#include <QWriteLocker>

//QReadWriteLock lock;

HlsIndex::HlsIndex(QObject *parent)
    : QObject(parent),
      fetchedCount_(0),
      decodedCount_(0),
      fetchedAll_(false)
{

}

HlsIndex::~HlsIndex()
{

}

int HlsIndex::getSliceCount() const
{
    return files_.size();
}



void HlsIndex::insertTs(TsFile *file)
{
    //QWriteLocker locker(&lock);
    files_.append(file);
}

char *HlsIndex::getData() const
{
    return data_;
}

void HlsIndex::setData(char *value)
{
    data_ = value;
}

QList<TsFile *> HlsIndex::getFiles()
{
    return files_;
}

QString HlsIndex::getUrl() const
{
    return url_;
}

void HlsIndex::setUrl(const QString &value)
{
    url_ = value;
}

int HlsIndex::getFetchedCount() const
{
    return fetchedCount_;
}

//void HlsIndex::setFetchedCount(int value)
//{
//    fetchedCount = value;
//}

int HlsIndex::getDecodedCount() const
{
    return decodedCount_;
}

bool HlsIndex::getFetchedAll() const
{
    return fetchedAll_;
}


bool HlsIndex::getDecodedAll() const
{
    return decodedAll_;
}

double HlsIndex::getDuration() const
{
    return duration_;
}

void HlsIndex::setDuration(double duration)
{
    duration_ = duration;
}

int HlsIndex::getNbFrames() const
{
    return nbFrames_;
}

void HlsIndex::setNbFrames(int nbFrames)
{
    nbFrames_ = nbFrames;
}

int HlsIndex::getFrameRate() const
{
    return frameRate_;
}

void HlsIndex::setFrameRate(int frameRate)
{
    frameRate_ = frameRate;
}

//void HlsIndex::setDecodedCount(int value)
//{
//    decodedCount = value;
//}

void HlsIndex::onFileFetchChanged(bool fetched)
{
    fetchedCount_ += 1;
    if (fetchedCount_ == files_.size()) {
        fetchedAll_ = true;
    }
}

void HlsIndex::onFileDecodeChanged(bool decode)
{
    decodedCount_ += 1;
    if (decodedCount_ == files_.size()) {
        fetchedAll_ = true;
    }
}

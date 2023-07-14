#include "hlsindex.h"

#include <QWriteLocker>

//QReadWriteLock lock;

HlsIndex::HlsIndex(QObject *parent)
    : QObject(parent),
      fetchedCount_(0),
      decodedCount_(0),
      fetchedAll_(false),
      timeTable(new QList<double>())
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

int HlsIndex::getSampleRate() const
{
    return sampleRate_;
}

void HlsIndex::setSampleRate(int sampleRate)
{
    sampleRate_ = sampleRate;
}

int HlsIndex::getChennels() const
{
    return chennels_;
}

void HlsIndex::setChennels(int chennels)
{
    chennels_ = chennels;
}

QList<double>* HlsIndex::getTimeTable() const
{
    return timeTable;
}

QString HlsIndex::getAuditFileFolderUuid() const
{
    return auditFileFolderUuid_;
}

void HlsIndex::setAuditFileFolderUuid(const QString &auditFileFolderUuid)
{
    auditFileFolderUuid_ = auditFileFolderUuid;
}

//void HlsIndex::setDecodedCount(int value)
//{
//    decodedCount = value;
//}

void HlsIndex::onFileFetchChanged(TsFile::TsState fetched)
{
    if (fetched == TsFile::TsState::FETCHED) {
        fetchedCount_ += 1;
    }
    if (fetchedCount_ == files_.size()) {
        fetchedAll_ = true;
    }
}


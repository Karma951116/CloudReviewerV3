﻿#ifndef HLSINDEX_H
#define HLSINDEX_H

#include <QObject>
#include <QMap>
#include <tsfile.h>

class HlsIndex : public QObject
{
    Q_OBJECT
public:
    explicit HlsIndex(QObject *parent = nullptr);
    ~HlsIndex();

    int getSliceCount() const;

    void insertTs(TsFile* file);

    char *getData() const;
    void setData(char *value);

    Q_INVOKABLE QList<TsFile *> getFiles();

    QString getUrl() const;
    void setUrl(const QString &value);

    int getFetchedCount() const;
    //void setFetchedCount(int value);

    int getDecodedCount() const;
    //void setDecodedCount(int value);

    bool getFetchedAll() const;
    bool getDecodedAll() const;

    double getDuration() const;
    void setDuration(double duration);

    int getNbFrames() const;
    void setNbFrames(int nbFrames);

    int getFrameRate() const;
    void setFrameRate(int frameRate);

    int getSampleRate() const;
    void setSampleRate(int sampleRate);

    int getChennels() const;
    void setChennels(int chennels);

    QList<double>* getTimeTable() const;

    QString getAuditFileFolderUuid() const;
    void setAuditFileFolderUuid(const QString &auditFileFolderUuid);

private:
    QString auditFileFolderUuid_;
    QString url_;
    char* data_;
    int fetchedCount_;
    int decodedCount_;
    QList<TsFile*> files_;
    bool fetchedAll_;
    bool decodedAll_;
    double duration_;
    int nbFrames_;
    int frameRate_;
    int sampleRate_;
    int chennels_;
    QList<double>* timeTable;

public slots:
    void onFileFetchChanged(TsFile::TsState fetched);

};

#endif // HLSINDEX_H

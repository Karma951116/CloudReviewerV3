#ifndef TSFILE_H
#define TSFILE_H

#include <QObject>
#include <QReadWriteLock>

class TsFile : public QObject
{
    Q_OBJECT
public:
    enum TsState {
        NOT_FETCHED,
        FETCHING,
        FETCHED
    };
    enum DecodeState {
        NOT_DECODE,
        DECODING,
        DECODED,
    };

    explicit TsFile(QObject *parent = nullptr);


    QString fileName() const;
    void setFileName(const QString &fileName);

    double duration() const;
    void setDuration(double duration);


    QString url() const;
    void setUrl(const QString &url);

    unsigned char *data();
    void setData(unsigned char *data);

    int dataLength() const;
    void setDataLength(int dataLength);

    int tsIndex() const;
    void setTsIndex(int tsIndex);


    TsFile::TsState fetchState() const;
    void setFetchState(const TsFile::TsState &fetchState);

    DecodeState decodeState() const;
    void setDecodeState(const DecodeState &decodeState);

private:
    int tsIndex_;
    QString fileName_;
    QString url_;
    double duration_;
    // 指针类型待定
    uint8_t* data_;
    size_t dataLength_;
    TsState fetchState_;
    DecodeState decodeState_;
    QReadWriteLock lock_;

signals:
    void fetchChanged(TsFile::TsState fetchState);
};

#endif // TSFILE_H

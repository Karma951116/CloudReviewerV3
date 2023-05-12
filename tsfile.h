#ifndef TSFILE_H
#define TSFILE_H

#include <QObject>
#include <QReadWriteLock>

class TsFile : public QObject
{
    Q_OBJECT
public:
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

    bool decoded() const;
    void setDecoded(bool decoded);

    bool fetched() const;
    void setFetched(bool fetched);

    int tsIndex() const;
    void setTsIndex(int tsIndex);

private:
    int tsIndex_;
    QString fileName_;
    QString url_;
    double duration_;
    // 指针类型待定
    uint8_t* data_;
    size_t dataLength_;
    bool decoded_;
    bool fetched_;
    QReadWriteLock lock_;

signals:
    void fetchChanged(bool fetched);
    void decodeChanged(bool decoded);
};

#endif // TSFILE_H

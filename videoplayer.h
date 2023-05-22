#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QObject>
#include <QQuickPaintedItem>
#include <QPainter>
#include <QReadWriteLock>
#include <QThreadPool>
#include <QTimer>

#include "hlsindex.h"
//#include "framequeue.h"
#include "replyparser.h"
#include "httpfunctions.h"
#include "tsroll.h"
#include "SDL.h"
#include "decodeworker.h"
#include "sonic.h"

extern "C"
{
#include <libavformat/avformat.h>
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
#include "libavutil/pixfmt.h"
#include "libavutil/opt.h"
#include "libavcodec/avfft.h"
#include "libavutil/imgutils.h"
#include "libswresample/swresample.h"
}

class VideoPlayer : public QQuickPaintedItem
{
    Q_OBJECT
public:
    enum PlayState {
        PLAY,
        PAUSE,
        STOP
    };


    struct SyncMeta {
        double videoClock;
        double audioClock;

    };

    Q_ENUM(PlayState);
    VideoPlayer();

    Q_INVOKABLE void play();
    Q_INVOKABLE void pause();
    HlsIndex *getHlsIndex() const;
    void setHlsIndex(HlsIndex *index);

    int getFetchIndex() const;
    void setFetchIndex(int fetchIndex);

    Q_INVOKABLE PlayState getState() const;
    Q_INVOKABLE void setState(const PlayState &state);

    int getDecodeIndex() const;
    void setDecodeIndex(int decodeIndex);
//    static int read_packet(void *opaque, uint8_t *buf, int buf_size);
    //FrameQueue* videoQueue_;
    //FrameQueue* audioQueue_;
    TsRoll* vRoll;
    TsRoll* aRoll;

    SyncMeta* syncMeta;

    bool getHasVideo() const;
    void setHasVideo(bool value);

    bool getHasAudio() const;
    void setHasAudio(bool value);

    Q_INVOKABLE void setReplyParser(ReplyParser *value);
    Q_INVOKABLE void setHttpFunctions(HttpFunctions *value);

protected:
    virtual void paint(QPainter* painter);

private:
    QImage image_;
    PlayState state_;

    HlsIndex* index_;
    int maxDecoderCount_;
    int tsDecoderCount_;
    QReadWriteLock fetchLock_;
    QReadWriteLock decodeLock_;

    SwsContext* swsContext_;
    unsigned char* swsOutBuffer_;
    bool hasVideo;
    bool hasAudio;
    VideoMeta* vMeta_;
    AudioMeta* aMeta_;

    SDL_Thread* videoThread_;

    //QThreadPool fetchPool_;
    //QThreadPool decodePool_;
    int fetchIndex_;
    int fetcherCount_;
    static int tsFetch(void *arg);
    static int tsFetchWorker(void *arg);

    static int videoPlay(void *arg);

    int openAudio();
    static void audioCallBack(void* userdata, quint8* stream, int SDL_BufferSize);
    int audioResample();
    SDL_AudioSpec desiredSpec_, hardwareSpec_;
    uint8_t* audioBuffer_;
    int audioBufferSize_;
    uint8_t* audioBufferPtr_;
    qint32 audioBufferRest_;
    int audioBytePerSec_;
    int audioDeviceId_;
    sonicStream sonicHandler_;
    int volume_;
    float speed_;
    SwrContext* swrContext_;

    ReplyParser* replyParser_;
    HttpFunctions* httpFunctions_;

    QTimer* checkTimer_;
signals:
    void updateImage(QImage image);
    void requestTsFile(QString tsName, TsFile* TS, QString url = nullptr);
    void firstTsDecoded();
public slots:
    void onM3u8ReplyDone(bool success, HlsIndex* index);
    void onMediaInfoReplyDone(bool success, QJsonObject ret);
//    void onTsFetched(TsFile* ts);
private slots:
    void onUpdateImage(QImage image);
    void onFirstTsDecoded();
    void onDecodeCheck();
    void onFetchCheck();
    void onDecodeFinished(QVector<AVFrame*>* vFrameVector, QVector<AVFrame*>* aFrameVector,
                          int blockIndex, QVariant vMeta, QVariant aMeta);
};

#endif // VIDEOPLAYER_H

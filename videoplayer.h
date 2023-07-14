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
        STOP,
        WAIT
    };
    Q_ENUM(PlayState);
    enum PlayMode {
        VIDEO,
        AUDIO
    };
    Q_ENUM(PlayMode)


    Q_PROPERTY(double durationTime READ getIndexDurationTime NOTIFY indexDurationTimeChanged)
    Q_PROPERTY(double currentTime READ getIndexCurrentTime NOTIFY indexCurrentTimeChanged)
    Q_PROPERTY(double durationFrame READ getIndexDurationFrame NOTIFY indexDurationFrameChanged)
    Q_PROPERTY(double currentFrame READ getIndexCurrentFrame NOTIFY indexCurrentFrameChanged)
    Q_PROPERTY(double state READ getState NOTIFY stateChanged)
    Q_PROPERTY(double mode READ getMode NOTIFY playModeChanged)

    VideoPlayer();

    Q_INVOKABLE void play();
    Q_INVOKABLE void pause();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void replay();
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
    bool getHasVideo() const;
    void setHasVideo(bool value);

    bool getHasAudio() const;
    void setHasAudio(bool value);

    Q_INVOKABLE void setReplyParser(ReplyParser *value);
    Q_INVOKABLE void setHttpFunctions(HttpFunctions *value);

    Q_INVOKABLE double getIndexDurationTime();
    Q_INVOKABLE double getIndexCurrentTime();
    Q_INVOKABLE int getIndexDurationFrame();
    Q_INVOKABLE int getIndexCurrentFrame();

    Q_INVOKABLE void seekTime(double seekPoint);
    Q_INVOKABLE void seekFrame(int seekPoint);
    Q_INVOKABLE void seekPreFrame();
    Q_INVOKABLE void seekNextFrame();

    Q_INVOKABLE QImage getCurrentImage();
    Q_INVOKABLE void setCurrentImage(QString imagePath);

    bool audioEnd;
    bool videoEnd;

    Q_INVOKABLE void setSpeed(float speed);

    Q_INVOKABLE void setVolume(int volume);

    Q_INVOKABLE PlayMode getMode() const;
    Q_INVOKABLE void setMode(const PlayMode &mode);

protected:
    virtual void paint(QPainter* painter);

private:
    QImage image_;
    PlayState state_;
    PlayMode mode_;

    HlsIndex* index_;
    int maxDecoderCount_;
    int tsDecoderCount_;
    QReadWriteLock fetchLock_;
    QReadWriteLock decodeLock_;
    QReadWriteLock playLock_;

    SwsContext* swsContext_;
    unsigned char* swsOutBuffer_;
    bool hasVideo;
    bool hasAudio;
    VideoMeta* vMeta_;
    AudioMeta* aMeta_;

    SDL_Thread* videoThread_;

    int decodeWorkerCount_;

    double videoClock_;
    double vFirstClock_;
    double audioClock_;
    double aFirstClock_;
    double diff_;
    int videoCurFrame;
    int videoStartPts;
    int videoPtsInterval;
    int audioPtsInterval;

    static int videoPlay(void *arg);
    void updateCurrentFrame();

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
    double correctAudioClock();

    static int durationFrameCalc(void *arg);

#ifdef DEBUG
    double tsVolume;
    double FrameVolume;
#endif
signals:
    void updateImage(QImage image);
    void requestTsFile(QString tsName, TsFile* TS, QString url = nullptr);
    void requestMediaInfo();
    void firstTsDecoded();
    //void waitNextFrameReady();
    void indexDurationTimeChanged();
    void indexCurrentTimeChanged();
    void indexDurationFrameChanged();
    void indexCurrentFrameChanged();
    void videoFinished();
    void stateChanged();
    void durationFrameCalcEnd();
    void durationTimeReady();
    void playModeChanged();

public slots:
    void onM3u8ReplyDone(bool success, HlsIndex* index);
    void onMediaInfoReplyDone(bool success, QJsonObject ret);
    void onBlockCleared(int blockIdx);
#ifdef DEBUG
    void onTsFetched(TsFile* ts);
    void onThreadFinished();
#endif

private slots:
    void onUpdateImage(QImage image);
    void onFirstTsDecoded();
    void onDecodeCheck();
    void onFetchCheck();
    void onDecodeFinished(QVector<AVFrame*>* vFrameVector, QVector<AVFrame*>* aFrameVector,
                          int blockIndex, QVariant vMeta, QVariant aMeta);
    void onVideoFinished();
    void onDurationFrameCalcEnd();
};

#endif // VIDEOPLAYER_H

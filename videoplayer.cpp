#include "videoplayer.h"

#define SDL_AUDIO_MIN_BUFFER_SIZE 512
#define SDL_AUDIO_MAX_CALLBACKS_PER_SEC 30

VideoPlayer::VideoPlayer() :
    audioEnd(false),
    videoEnd(false),
    state_(STOP),
    index_(nullptr),
    swsContext_(nullptr),
    vMeta_(nullptr),
    aMeta_(nullptr),
    videoThread_(nullptr),
    videoClock_(0),
    vFirstClock_(NULL),
    audioClock_(0),
    aFirstClock_(NULL),
    volume_(50),
    speed_(1.0f),
    swrContext_(nullptr),
    checkTimer_(new QTimer())
{
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
        qDebug() << "SDL init failed";
    }
    connect(this, SIGNAL(updateImage(QImage)),
            this, SLOT(onUpdateImage(QImage)));
    connect(this, SIGNAL(firstTsDecoded()),
            this, SLOT(onFirstTsDecoded()));
    connect(this, SIGNAL(videoFinished()),
            this, SLOT(onVideoFinished()));
    connect(checkTimer_, SIGNAL(timeout()),
            this, SLOT(onDecodeCheck()));
    connect(checkTimer_, SIGNAL(timeout()),
            this, SLOT(onFetchCheck()));

#ifdef DEBUG

    tsVolume = 0;

#endif

}

void VideoPlayer::play()
{
    setState(PLAY);
    stateChanged();
    if (videoThread_ != nullptr) {
        SDL_PauseAudio(0);
    }
//    else {
//        if (!openAudio()) {
//        }
//        videoThread_ = SDL_CreateThread(videoPlay, "videoPlay", this);
//    }
    qDebug() << "Player State: PLAY";
}

void VideoPlayer::pause()
{
    setState(PAUSE);
    SDL_PauseAudio(1);
    stateChanged();
    qDebug() << "Player State: PAUSE";
}

void VideoPlayer::stop()
{
    setState(STOP);
    SDL_PauseAudio(1);
    SDL_CloseAudio();
    checkTimer_->stop();
    vRoll->empty();
    aRoll->empty();
    vRoll->deleteLater();
    aRoll->deleteLater();
    delete vMeta_;
    delete aMeta_;
    videoClock_ = 0;
    audioClock_ = 0;
    indexCurrentTimeChanged();
    emit updateImage(QImage(vMeta_->width,
                            vMeta_->height,
                            QImage::Format_RGBA8888).copy());
    stateChanged();
}

void VideoPlayer::replay()
{
    pause();
    vRoll->seek(0, 0);
    aRoll->seek(0, 0);   
    checkTimer_->start();
    audioEnd = false;
    videoClock_ = 0;
    audioClock_ = 0;
    emit updateImage(QImage(vMeta_->width,
                            vMeta_->height,
                            QImage::Format_RGBA8888).copy());
    indexCurrentTimeChanged();
    play();
}

HlsIndex *VideoPlayer::getHlsIndex() const
{
    return index_;
}

void VideoPlayer::setHlsIndex(HlsIndex *index)
{
    index_ = index;
    vRoll = new TsRoll(index->getSliceCount());
    aRoll = new TsRoll(index->getSliceCount());
    connect(vRoll, SIGNAL(blockCleared(int)),
            this, SLOT(onBlockCleared(int)));
    connect(aRoll, SIGNAL(blockCleared(int)),
            this, SLOT(onBlockCleared(int)));
    vMeta_ = new VideoMeta();
    aMeta_ = new AudioMeta();

    indexDurationTimeChanged();
    indexCurrentTimeChanged();
    SDL_CreateThread(durationFrameCalc, "durationFrameCalc", this);
}

VideoPlayer::PlayState VideoPlayer::getState() const
{
    return state_;
}

void VideoPlayer::setState(const PlayState &state)
{
    state_ = state;
}

bool VideoPlayer::getHasVideo() const
{
    return hasVideo;
}

void VideoPlayer::setHasVideo(bool value)
{
    hasVideo = value;
}

bool VideoPlayer::getHasAudio() const
{
    return hasAudio;
}

void VideoPlayer::setHasAudio(bool value)
{
    hasAudio = value;
}

void VideoPlayer::paint(QPainter *painter)
{
    painter->setRenderHint(QPainter::Antialiasing, true);

    int width = this->width();
    int height = this->height();

    painter->setBrush(Qt::black);
    painter->drawRect(0, 0, width, height);

    if (!image_.isNull()) {
        QImage img = image_.scaled(QSize(width, height), Qt::KeepAspectRatio);
        /* calculate display position */
        int x = (this->width() - img.width()) / 2;
        int y = (this->height() - img.height()) / 2;
        painter->drawImage(QPoint(x, y), img);
    }
}

int VideoPlayer::videoPlay(void *arg)
{
    VideoPlayer* player = (VideoPlayer*) arg;
    AVFrame* frame;
    AVFrame* frame_rgb = av_frame_alloc();
    bool flag = false;
    while (player->state_ != STOP) {
        while (player->state_ == PAUSE) {
            SDL_Delay(5);
            continue;
        }
        if (!player->vRoll->canRead()) {
            continue;
        }
        frame = player->vRoll->read();
        if (player->swsContext_ == nullptr) {
            player->swsContext_ = sws_alloc_context();
            player->swsContext_ = sws_getContext(frame->width,
                                                 frame->height,
                                                 AVPixelFormat::AV_PIX_FMT_YUV420P,
                                                 frame->width,
                                                 frame->height,
                                                 AV_PIX_FMT_RGBA, SWS_BICUBIC, nullptr,
                                                 nullptr, nullptr);
            player->swsOutBuffer_ = (unsigned char*)av_malloc(
                        av_image_get_buffer_size(AV_PIX_FMT_RGBA,
                                                 frame->width,
                                                 frame->height,
                                                 1));
        }
        if (player->swsContext_ != nullptr) {
            av_image_fill_arrays(frame_rgb->data,
                                 frame_rgb->linesize,
                                 player->swsOutBuffer_, AV_PIX_FMT_RGBA,
                                 frame->width,
                                 frame->height,
                                 1);
            sws_scale(player->swsContext_, (const unsigned char* const*)frame->data,
                      frame->linesize, 0, frame->height,
                      frame_rgb->data, frame_rgb->linesize);
        }
        player->videoClock_ = player->vMeta_->vTimeBase * frame->pts;
        player->indexCurrentTimeChanged();
        //qDebug() << "Video:" <<frame->pts << player->videoClock_;
        while (player->state_ != STOP) {
            // 同步阈值根据Rec.ITU-R BT 1359-1中可察觉与可接受阈值相关定义
            if (player->audioEnd) {
                break;
            }
            player->diff_ = (player->videoClock_ - player->correctAudioClock()) * 1000;
            // 音频超前
            if (player->diff_ < 0) {
                break;
            }
            SDL_Delay(player->diff_ > 5 ? 5 : player->diff_);
            flag = true;
        }
        QImage img = (QImage(frame_rgb->data[0],
                      frame->width,
                      frame->height,
                      QImage::Format_RGBA8888
                      ).copy());
        emit player->updateImage(img);
        if (player->vRoll->isLastFrame() && player->audioEnd) {
            player->videoFinished();
            continue;
        }
        player->vRoll->nextFrame();
        if (!flag) {
            SDL_Delay(1 / player->vMeta_->frameRate);
        }
    }
    av_frame_free(&frame_rgb);
    return 0;
}

int VideoPlayer::openAudio()
{
    desiredSpec_.channels = av_get_channel_layout_nb_channels(
                             aMeta_->channelLayout);
    desiredSpec_.freq = aMeta_->sampleRate;
    // format需要从浮点转为整型
    desiredSpec_.format = AUDIO_S16SYS;
            //media_params_.sample_format;
    // av_log2()算法见intmath.h和log2_tab.c 寻找输入值附近的合适样本数
    desiredSpec_.samples = FFMAX(SDL_AUDIO_MIN_BUFFER_SIZE,
                                 2 << av_log2( desiredSpec_.freq /
                                               SDL_AUDIO_MAX_CALLBACKS_PER_SEC));
    desiredSpec_.silence = 0;
    desiredSpec_.callback = audioCallBack;
    desiredSpec_.userdata = this;

//    int ret = SDL_GetNumAudioDevices(false);
//    QString deviceName;
//    for (int i = 0; i < ret; i++) {
//        QString deviceName = QString::fromUtf8(SDL_GetAudioDeviceName(i, false)).toStdString().c_str();
//        qDebug() << deviceName.toUtf8();
//    }

    if((audioDeviceId_ = SDL_OpenAudio(&desiredSpec_, &hardwareSpec_)) < 0) {
        qDebug() << "can't open audio.\n";
        return -1;
    }
    qDebug() << SDL_GetAudioDeviceName(audioDeviceId_, false);

    //TODO:改写为转换函数
    switch (hardwareSpec_.format) {
    case AUDIO_U8:
        hardwareSpec_.format = AV_SAMPLE_FMT_U8;
        break;

    case AUDIO_S16SYS:
        hardwareSpec_.format = AV_SAMPLE_FMT_S16;
        break;

    case AUDIO_S32SYS:
        hardwareSpec_.format = AV_SAMPLE_FMT_S32;
        break;

    case AUDIO_F32SYS:
        hardwareSpec_.format = AV_SAMPLE_FMT_FLT;
        break;

    default:
        hardwareSpec_.format = AV_SAMPLE_FMT_S16;
        break;
    }
    audioBuffer_ = (uint8_t *)av_malloc(hardwareSpec_.size);
    audioBufferRest_ = 0;
    audioBufferSize_ = hardwareSpec_.size;
    audioBytePerSec_ = aMeta_->channels * aMeta_->sampleRate *
                         av_get_bytes_per_sample((AVSampleFormat)hardwareSpec_.format);
    if(sonicHandler_ == nullptr) {
        sonicHandler_ = sonicCreateStream(hardwareSpec_.freq,
                                          hardwareSpec_.channels);
    }
    return hardwareSpec_.size;
}

void VideoPlayer::audioCallBack(void *userdata, quint8 *stream, int SDL_BufferSize)
{
    VideoPlayer* player = (VideoPlayer*) userdata;
    int data_size = 0;
    SDL_memset(stream, 0, SDL_BufferSize);

    while (SDL_BufferSize > 0) {
        if (player->audioBufferRest_ <= 0) {
            data_size = player->audioResample();

            if (data_size < 0 ) {
                //TODO: 返回值处理
                break;
            }
        }
        int write_len = SDL_BufferSize > player->audioBufferRest_ ?
                        player->audioBufferRest_ : SDL_BufferSize;
        if (player->audioBuffer_) {
//            if (player->step_){
//                SDL_MixAudio(stream, player->audioBufferPtr_, write_len, 0);
//            }
//            else {

//            }
            SDL_MixAudio(stream, player->audioBufferPtr_, write_len, SDL_MIX_MAXVOLUME);
            SDL_BufferSize -= write_len;
            stream += write_len;
            player->audioBufferRest_ -= write_len;
            player->audioBufferPtr_ += write_len;
        }
    }
}

int VideoPlayer::audioResample()
{
    AVFrame* audio_frame;
    int data_count_byte = 0, samples_count = 0, out_count = 0;
    if (!aRoll->canRead()) {
        //qDebug() << aRoll->tsBlockIndex() << "," << aRoll->tsFrameIndex() << "," << aRoll->isLastFrame();
        //aRoll->nextFrame();
        //waitNextFrameReady();
        return -1;
    }
    audio_frame = aRoll->read();

//    if (seek_ && media_type_ == AUDIO_ONLY) {
//        //按帧播放seek使用BACKWARD，因此需要丢弃seek结果帧与目标帧之间的帧
//        if (step_) {
//            if ((audio_frame->pts / audio_frame->pkt_duration) !=
//                 seek_target_frame_) {
//                //av_frame_unref(audio_frame);
//                av_frame_free(&audio_frame);
//                return -1;
//            }
//        }
//        else {
//            if (audio_frame->pts < seek_pts_) {
//                //av_frame_unref(audio_frame);
//                av_frame_free(&audio_frame);
//                return -1;
//            }
//        }
//    }
    if (((AVSampleFormat)audio_frame->format != hardwareSpec_.format ||
        audio_frame->channels != hardwareSpec_.channels ||
        audio_frame->sample_rate != hardwareSpec_.freq) && !swrContext_) {
        swrContext_ = swr_alloc_set_opts(NULL, av_get_default_channel_layout(hardwareSpec_.channels),
                           (AVSampleFormat)hardwareSpec_.format, hardwareSpec_.freq,
                           av_get_default_channel_layout(audio_frame->channels),
                           (AVSampleFormat)audio_frame->format, audio_frame->sample_rate, 0, NULL);
        if (!swrContext_) {
            qDebug() << "swr_context alloc failed\n" ;
            av_frame_free(&audio_frame);
            return -1;
        }
        switch (swr_init(swrContext_)) {
        case AVERROR(EINVAL):
            qDebug() << "swr_context init failed\n" ;
        }
    }

    if (swrContext_ != NULL) {
        out_count = audioBufferSize_ / hardwareSpec_.channels /
                av_get_bytes_per_sample((AVSampleFormat)hardwareSpec_.format);
        // 样本数计算为该帧所有声道总样本数
        samples_count = swr_convert(swrContext_, &audioBuffer_, out_count,
                                    (const uint8_t **)audio_frame->data,
                                    audio_frame->nb_samples);
        data_count_byte = samples_count *
                          av_get_bytes_per_sample((AVSampleFormat)hardwareSpec_.format) *
                          hardwareSpec_.channels;
    }
    else {
        audioBuffer_ = audio_frame->data[0];
        samples_count = audio_frame->nb_samples;
        data_count_byte =
                av_samples_get_buffer_size(
                    NULL, audio_frame->channels,
                    audio_frame->nb_samples,
                    (AVSampleFormat)audio_frame->format, 1);
    }
//     sonic变速
//     TODO: 4K变速会产生声画不同步现象，可能原因是解码速度慢
    if (speed_ != 1.0f) {
        sonicSetSpeed(sonicHandler_, speed_);
        sonicSetPitch(sonicHandler_, 1.0f);
        sonicSetRate(sonicHandler_, 1.0f);
        sonicSetVolume(sonicHandler_, 1.0f);
        sonicSetChordPitch(sonicHandler_, 0);
        sonicSetQuality(sonicHandler_, 1);
        sonicWriteFloatToStream(sonicHandler_, (float *)audioBuffer_, samples_count);
        samples_count = sonicReadFloatFromStream(sonicHandler_, (float *)audioBuffer_, out_count);
        data_count_byte = samples_count *
                          av_get_bytes_per_sample((AVSampleFormat)hardwareSpec_.format) *
                          hardwareSpec_.channels;
    }

    audioBufferPtr_ = audioBuffer_;
    audioBufferRest_ = data_count_byte;
//    if (!audio_frame_duration_) {
//        audio_frame_duration_ = audio_frame->pkt_duration;
//    }
//    if (audio_frame->pkt_duration) {
//        audio_frame_num_ = (audio_frame->pts / audio_frame->pkt_duration);
//    }
//    audio_frame_num_ = audio_frame->sample_rate / audio_frame->nb_samples;
    if (audio_frame->pts > 0) {
        audioClock_ = aMeta_->aTimeBase * audio_frame->pts;
        //qDebug() << "Audio:" << audio_frame->pts << audioClock_;
    }

    // 优先使用视频帧与时间，在仅有音频时使用音频数据
//    if (media_type_ == AUDIO_ONLY) {
//        emit currentFrameChanged();
//        emit currentTimeChanged();
//        if (seek_) {
//            if (step_) {
//                pause();
//                step_ = false;
//                //setLockState(false);
//            }
//            seek_ = false;
//            //setLockState(false);
//        }
//    }
    //av_frame_free(&audio_frame);
    if (aRoll->isLastFrame()) {
        audioEnd = true;
    }
    aRoll->nextFrame();
    return data_count_byte;
}

double VideoPlayer::correctAudioClock()
{
    if (audioBytePerSec_ <= 0) {
        return audioClock_;
    }
    audioClock_ -= static_cast<double>(audioBufferRest_) /
                    audioBytePerSec_;
    return audioClock_;
}

int VideoPlayer::durationFrameCalc(void *arg)
{
    VideoPlayer* player = (VideoPlayer*) arg;
    QList<TsFile*> files = player->index_->getFiles();
    int blockIdx = files.length() - 1;
    player->requestTsFile(files[0]->fileName(), files[0], nullptr);
    qDebug() << "Request Ts: " << files[0]->fileName();
    player->requestTsFile(files[blockIdx]->fileName(), files[blockIdx], nullptr);
    qDebug() << "Request Ts: " << files[blockIdx]->fileName();
    while(files[0]->fetchState() != TsFile::TsState::FETCHED ||
          files[blockIdx]->fetchState() != TsFile::TsState::FETCHED) {
        SDL_Delay(100);
    }
    DecodeWorker* worker1 = new DecodeWorker(player->index_, blockIdx);
    connect(worker1, &DecodeWorker::finished, worker1, &QObject::deleteLater);
    DecodeWorker* worker2 = new DecodeWorker(player->index_, 0);
    connect(worker2, &DecodeWorker::finished, worker2, &QObject::deleteLater);
    while(files[0]->decodeState() != TsFile::DecodeState::DECODED ||
          files[blockIdx]->decodeState() != TsFile::DecodeState::DECODED) {
        SDL_Delay(100);
    }
    // calc video frames
    AVFrame* first = player->vRoll->read(0, 0);
    AVFrame* second = player->vRoll->read(0, 1);
    AVFrame* last = player->vRoll->read(blockIdx, player->vRoll->blockSize(blockIdx) - 1);
    int ptsFirst = first->pts;
    int ptsSecond = second->pts;
    int ptsLast = last->pts;
    int nbFrames = (ptsLast - ptsFirst) / (ptsSecond - ptsFirst) + 1;
    // calc audio frames
    first = player->aRoll->read(0, 0);
    second = player->aRoll->read(0, 1);
    last = player->aRoll->read(blockIdx, player->vRoll->blockSize(blockIdx) - 1);
    ptsFirst = first->pts;
    ptsSecond = second->pts;
    ptsLast = last->pts;
    nbFrames = (ptsLast - ptsFirst) / (ptsSecond - ptsFirst) + 1;
}


void VideoPlayer::setHttpFunctions(HttpFunctions *value)
{
    httpFunctions_ = value;
    connect(this, SIGNAL(requestTsFile(QString, TsFile*, QString)),
            httpFunctions_, SLOT(onRequestTsFile(QString, TsFile*, QString)));

}

double VideoPlayer::getIndexDurationTime()
{
    if (index_ == nullptr) {
        return NULL;
    }
    //qDebug() << "Duration:" << index_->getDuration();
    return index_->getDuration();
}

double VideoPlayer::getIndexCurrentTime()
{
    //qDebug() << "Current:" << videoClock_;
    return videoClock_;
}

void VideoPlayer::seekTime(int seekPoint)
{
    pause();
    QList<double>* timeTable = index_->getTimeTable();
    int left = 0;
    int right = timeTable->length() - 1;
    if (seekPoint > timeTable->value(right)) {
        return;
    }
    if (seekPoint < timeTable->value(left)) {
        return;
    }

    while ((right - left) > 1) {
        int mid = floor((right + left) / 2);
        if (seekPoint <= timeTable->value(mid)) {
            right = mid;
        }
        else if (seekPoint >= timeTable->value(mid)) {
            left = mid;
        }
    }
    // seek return block index is right
    int blockIndex = right;
    double segSeekDuration = seekPoint - timeTable->value(left);
    int vFrameIndex = segSeekDuration * vMeta_->frameRate;
    int aFrameIndex = segSeekDuration * (aMeta_->sampleRate / aMeta_->samplePerFrame);
    vRoll->empty();
    aRoll->empty();
    SDL_Delay(100);
    bool seekFlag = vRoll->seek(blockIndex, vFrameIndex);
    seekFlag &= aRoll->seek(blockIndex, aFrameIndex);
    audioEnd = false;
    videoClock_ = 0;
    audioClock_ = 0;
    play();
}

void VideoPlayer::seekFrame(int seekPoint)
{

}

void VideoPlayer::setReplyParser(ReplyParser *value)
{
    replyParser_ = value;
    connect(replyParser_, SIGNAL(m3u8ReplyDone(bool, HlsIndex*)),
            this, SLOT(onM3u8ReplyDone(bool, HlsIndex*)));
    connect(replyParser_, SIGNAL(mediaInfoReplyDone(bool, QJsonObject)),
            this, SLOT(onMediaInfoReplyDone(bool, QJsonObject)));
    connect(replyParser_, SIGNAL(tsFetched(TsFile*)),
            this, SLOT(onTsFetched(TsFile*)));
}

void VideoPlayer::onM3u8ReplyDone(bool success, HlsIndex *index)
{
    if (success) {
        setHlsIndex(index);
    }
    else {
        index_ = nullptr;
    }

}

void VideoPlayer::onMediaInfoReplyDone(bool success, QJsonObject ret)
{
    if (success && index_ != nullptr) {
        QJsonObject video = ret.value("video").toObject();
        QJsonObject audio = ret.value("audio").toObject();
        index_->setNbFrames(video.value("nbFrames").toInt());
        index_->setFrameRate(video.value("frameRate").toInt());
        index_->setSampleRate(audio.value("sampleRate").toInt());
        index_->setChennels(audio.value("chennels").toInt());
    }
}

void VideoPlayer::onBlockCleared(int blockIdx)
{
    index_->getFiles().at(blockIdx)->setDecodeState(TsFile::NOT_DECODE);
}

#ifdef DEBUG
void VideoPlayer::onTsFetched(TsFile *ts)
{
    double curBytes = ts->dataLength();
    tsVolume += curBytes / 1024 / 1024;
}

void VideoPlayer::onThreadFinished()
{
    decodeWorkerCount_ -= 1;
}
#endif
void VideoPlayer::onUpdateImage(QImage image)
{
    image_ = image;
    update();
}

void VideoPlayer::onFirstTsDecoded()
{
    pause();
    if (!openAudio()) {

    }
    videoThread_ = SDL_CreateThread(videoPlay, "videoPlay", this);
    play();
}

void VideoPlayer::onDecodeCheck()
{
    QList<TsFile*> files = index_->getFiles();
    int blockIdx = vRoll->tsBlockIndex();
    for (int i = 0; i < 5; i++) {
        if (blockIdx + i >= files.length()) {
            break;
        }
        if(!vRoll->isBlockEmpty(blockIdx + i) ||
                files[blockIdx + i]->decodeState() != TsFile::NOT_DECODE) {
            continue;
        }
        DecodeWorker* worker = new DecodeWorker(index_, blockIdx + i);
        connect(worker, SIGNAL(decodeFinished(QVector<AVFrame*>*, QVector<AVFrame*>*,int, QVariant, QVariant)),
                this, SLOT(onDecodeFinished(QVector<AVFrame*>*, QVector<AVFrame*>*,int, QVariant, QVariant)));
        connect(worker, &DecodeWorker::finished, worker, &QObject::deleteLater);
#ifdef DEBUG
        connect(worker, &DecodeWorker::finished, this, &VideoPlayer::onThreadFinished);
#endif
        worker->start();
        QWriteLocker locker(&decodeLock_);
        decodeWorkerCount_ += 1;
    }
}

void VideoPlayer::onFetchCheck()
{
    QList<TsFile*> files = index_->getFiles();
    int blockIdx = vRoll->tsBlockIndex();
    for (int i = 0; i < 5; i++) {
        if (blockIdx + i >= files.length()) {
            break;
        }
        if (files[blockIdx + i]->fetchState() != TsFile::TsState::FETCHED) {
            requestTsFile(files[blockIdx + i]->fileName(), files[blockIdx + i], nullptr);
            qDebug() << "Request Ts: " << files[blockIdx + i]->fileName();
        }
    }
    if (index_->getFetchedAll()) {
        checkTimer_->stop();
    }
    //qDebug() << QString("Current Ts Data Size: %1 MB").arg(tsVolume);
}

void VideoPlayer::onDecodeFinished(QVector<AVFrame*>* vFrameVector, QVector<AVFrame*>* aFrameVector,
                                   int blockIndex, QVariant vMeta, QVariant aMeta)
{
//    if (getState() == STOP || videoThread_ != nullptr) {
//        return;
//    }
    if (blockIndex == 0) {
        VideoMeta v = vMeta.value<VideoMeta>();
        vMeta_->width = v.width;
        vMeta_->height = v.height;
        vMeta_->pixelFormat = v.pixelFormat;
        vMeta_->vTimeBase = v.vTimeBase;
        vMeta_->vTimeBaseRational = v.vTimeBaseRational;
        vMeta_->frameRate = v.frameRate;
        AudioMeta a = aMeta.value<AudioMeta>();
        aMeta_->channelLayout = a.channelLayout;
        aMeta_->sampleRate = a.sampleRate;
        aMeta_->sampleFormat = a.sampleFormat;
        aMeta_->channels = a.channels;
        aMeta_->aTimeBase = a.aTimeBase;
        aMeta_->aTimeBaseRational = a.aTimeBaseRational;
        aMeta_->samplePerFrame = a.samplePerFrame;
        firstTsDecoded();
    }
    vRoll->setTsBlock(blockIndex, vFrameVector);
    aRoll->setTsBlock(blockIndex, aFrameVector);
#ifdef DEBUG
    FrameVolume += vFrameVector->size() * 1.32;
    FrameVolume += aFrameVector->size() * 0.0039;
    //qDebug() << QString("Current Frames Data Size: %1 MB").arg(FrameVolume);
#endif
}

void VideoPlayer::onVideoFinished()
{
    pause();
//    emit updateImage(QImage(vMeta_->width,
//                            vMeta_->height,
//                            QImage::Format_RGBA8888).copy());
    checkTimer_->stop();
}

void VideoPlayer::onDurationFrameCalcEnd()
{
    checkTimer_->setInterval(2000);
    checkTimer_->start();
}


#include "videoplayer.h"

#define SDL_AUDIO_MIN_BUFFER_SIZE 512
#define SDL_AUDIO_MAX_CALLBACKS_PER_SEC 30

//struct buffer_data {
//    uint8_t* data;
//    int size;
//    int offset;
//};

VideoPlayer::VideoPlayer() :
    syncMeta(new SyncMeta()),
    state_(STOP),
    swsContext_(nullptr),
    vMeta_(nullptr),
    aMeta_(nullptr),
    videoThread_(nullptr),
    fetchIndex_(0),
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
    connect(checkTimer_, SIGNAL(timeout()),
            this, SLOT(onDecodeCheck()));
    connect(checkTimer_, SIGNAL(timeout()),
            this, SLOT(onFetchCheck()));
}

void VideoPlayer::play()
{
    setState(PLAY);
    SDL_PauseAudio(0);
}

void VideoPlayer::pause()
{
    setState(PAUSE);
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
    vMeta_ = new VideoMeta();
    aMeta_ = new AudioMeta();
}

int VideoPlayer::getFetchIndex() const
{
    return fetchIndex_;
}

void VideoPlayer::setFetchIndex(int fetchIndex)
{
    QWriteLocker locker(&fetchLock_);
    fetchIndex_ = fetchIndex;
}

VideoPlayer::PlayState VideoPlayer::getState() const
{
    return state_;
}

void VideoPlayer::setState(const PlayState &state)
{
    state_ = state;
}

//int VideoPlayer::tsFetch(void *arg)
//{
//    VideoPlayer* player = (VideoPlayer*) arg;
//    player->fetcherCount_ = 0;
//    while (!player->index_->getFetchedAll()) {
//        while (player->fetcherCount_ < 3) {
//            SDL_Delay(200);
//            SDL_CreateThread(tsFetchWorker, "", player);
//            QWriteLocker locker(&player->fetchLock_);
//            player->fetcherCount_ += 1;
//        }
//        SDL_Delay(2000);
//    }
//    return 0;
//}

//int VideoPlayer::tsFetchWorker(void *arg)
//{
//    VideoPlayer* player = (VideoPlayer*) arg;
//    QList<TsFile*> files = player->index_->getFiles();
//    int fetchIndex = player->getFetchIndex();
//    if (!files[fetchIndex]->fetched()) {
//        player->requestTsFile(files[fetchIndex]->fileName(), files[fetchIndex], nullptr);
//        qDebug() << "Request Ts: " << files[fetchIndex]->fileName() <<
//                    "; CurrentWorkers: " << player->fetcherCount_;
//    }
//    fetchIndex += 1;
//    if (fetchIndex >= files.size()) {
//        fetchIndex = 0;
//    }
//    player->setFetchIndex(fetchIndex);
//    return 0;
//}



//int VideoPlayer::getDecodeIndex() const
//{
//    return decodeIndex_;
//}

//void VideoPlayer::setDecodeIndex(int decodeIndex)
//{
//    QWriteLocker locker(&decodeLock_);
//    decodeIndex_ = decodeIndex;
//}

//int VideoPlayer::read_packet(void *opaque, uint8_t *buf, int buf_size)
//{
//    buffer_data *bd = (buffer_data *) opaque;
//    int copySize = bd->offset + buf_size < bd->size ? buf_size :
//                                                      (bd->size - bd->offset);
//    buf_size = FFMIN(buf_size, (int)bd->size);
//    if (bd->offset < bd->size) {
//        memcpy(buf, bd->data + bd->offset, copySize);
//        bd->offset += copySize;
//        return copySize;
//    }
//    else {
//        return -1;
//    }
//}


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
        player->syncMeta->videoClock = player->vMeta_->vTimeBase * frame->pts;

        while (player->state_ != STOP) {
            // 同步阈值根据Rec.ITU-R BT 1359-1中可察觉与可接受阈值相关定义
            double diff = (player->syncMeta->videoClock - player->syncMeta->audioClock) * 1000;
            // 音频超前
            if (diff < 0) {
                break;
            }
            SDL_Delay(diff > 5 ? 5 : diff);
        }

        QImage img = (QImage(frame_rgb->data[0],
                      frame->width,
                      frame->height,
                      QImage::Format_RGBA8888
                      ).copy());
        emit player->updateImage(img);
        player->vRoll->nextFrame();
        SDL_Delay(42);
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
        aRoll->nextFrame();
        return -1;
    }
    audio_frame = aRoll->read();
    aRoll->nextFrame();

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
    if (audio_frame->pts > 0) {
        syncMeta->audioClock =  aMeta_->aTimeBase * audio_frame->pts;
    }
//    if (!audio_frame_duration_) {
//        audio_frame_duration_ = audio_frame->pkt_duration;
//    }
//    if (audio_frame->pkt_duration) {
//        audio_frame_num_ = (audio_frame->pts / audio_frame->pkt_duration);
//    }
//    audio_frame_num_ = audio_frame->sample_rate / audio_frame->nb_samples;
//    audio_clock_ = media_params_.audio_time_base *
//                   audio_frame->pts;
    //qDebug() << audio_frame->pts << " " << audio_frame_num_;
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
    return data_count_byte;
}


void VideoPlayer::setHttpFunctions(HttpFunctions *value)
{
    httpFunctions_ = value;
    connect(this, SIGNAL(requestTsFile(QString, TsFile*, QString)),
            httpFunctions_, SLOT(onRequestTsFile(QString, TsFile*, QString)));

}

void VideoPlayer::setReplyParser(ReplyParser *value)
{
    replyParser_ = value;
    connect(replyParser_, SIGNAL(m3u8ReplyDone(bool, HlsIndex*)),
            this, SLOT(onM3u8ReplyDone(bool, HlsIndex*)));
    connect(replyParser_, SIGNAL(mediaInfoReplyDone(bool, int, int)),
            this, SLOT(onMediaInfoReplyDone(bool, int, int)));
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
    //tsFetcher_ = SDL_CreateThread(tsFetch, "tsFetch", this);
    checkTimer_->setInterval(2000);
    checkTimer_->start();
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

//void VideoPlayer::onTsFetched(TsFile *ts)
//{
//    if (!ts->fetched()) {
//        return;
//    }
//    AVFormatContext *fmt_ctx = nullptr;
//    AVIOContext *avio_ctx = nullptr;
//    AVCodec* video_codec_  = nullptr;
//    AVCodecContext* video_codec_context_  = nullptr;
//    AVStream* video_stream_  = nullptr;
//    AVCodec* audio_codec_  = nullptr;
//    AVCodecContext* audio_codec_context_  = nullptr;
//    AVStream* audio_stream_  = nullptr;
//    // io缓冲初始化
//    uint8_t *avio_ctx_buffer = nullptr;
//    size_t avio_ctx_buffer_size = 65536;

//    int video_index_ = -1;
//    int audio_index_ = -1;
//    // ts文件参数初始化
//    buffer_data bd = {(uint8_t*)ts->data(), ts->dataLength(), 0};

//    fmt_ctx = avformat_alloc_context();
//    avio_ctx_buffer = (uint8_t *)av_malloc(avio_ctx_buffer_size);
//    avio_ctx = avio_alloc_context(avio_ctx_buffer, avio_ctx_buffer_size,
//                                  0, &bd, read_packet, NULL, NULL);

////    AVInputFormat* inputFmt = NULL;
////    if (av_probe_input_buffer(avio_ctx, &inputFmt, "", NULL, 0, 0) < 0)
////    {
////        qDebug() << ("probe format failed\n");
////    }
////    else
////    {
////        qDebug() << ("format:%s[%s]\n", inputFmt->name, inputFmt->long_name);
////    }
//    fmt_ctx->pb = avio_ctx;
//    fmt_ctx->flags = AVFMT_FLAG_CUSTOM_IO;
//    if (avformat_open_input(&fmt_ctx, "", NULL, NULL) < 0)
//    {
//        fprintf(stderr, "Could not open input\n");
//        return;
//    }

//    for (unsigned int i = 0; i < fmt_ctx->nb_streams; i++) {
//        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
//            video_index_ = i;
//            //qDebug() << "Find video stream.videoIndex: " << video_index_;
//            continue;
//        }
//        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
//            audio_index_ = i;
//            //qDebug() << "Find video stream.audio_index_: " << audio_index_;
//            continue;
//        }
//    }
//    if (video_index_ >= 0) {
//        video_stream_ = fmt_ctx->streams[video_index_];
//        // 配置packet舍弃策略
//        fmt_ctx->streams[video_index_]->discard = AVDISCARD_DEFAULT;
//        // 根据视频流创建视频解码器上下文
//        video_codec_context_ = avcodec_alloc_context3(NULL);
//        if (avcodec_parameters_to_context(
//                    video_codec_context_,
//                    fmt_ctx->streams[video_index_]->codecpar) < 0) {
//            qDebug() << "Fill video decoder context failed.";
//            return;
//        }
//        // 寻找视频解码器
//        video_codec_ = avcodec_find_decoder(video_codec_context_->codec_id);
//        if (video_codec_ == NULL) {
//            qDebug() << "Video decoder not found.";
//            return;
//        }
//        // 打开视频解码器待用
//        if (avcodec_open2(video_codec_context_, video_codec_, NULL) < 0) {
//            qDebug() << "Open video decoder failed.";
//            return;
//        }
//    }
//    if (audio_index_ >= 0) {
//        audio_stream_ = fmt_ctx->streams[audio_index_];
//        fmt_ctx->streams[audio_index_]->discard = AVDISCARD_DEFAULT;
//        audio_codec_context_ = avcodec_alloc_context3(NULL);
//        if (avcodec_parameters_to_context(
//                    audio_codec_context_,
//                    fmt_ctx->streams[audio_index_]->codecpar) < 0) {
//            qDebug() << "Fill audio decoder failed.";
//            return;
//        }
//        audio_codec_ = avcodec_find_decoder(audio_codec_context_->codec_id);
//        if (audio_codec_ == NULL) {
//            qDebug() << "Audio Decoder not found.";
//            return;
//        }
//        if (avcodec_open2(audio_codec_context_, audio_codec_, NULL) < 0) {
//            qDebug() << "Open audio decoder failed.";
//            return;
//        }
//    }

//    AVPacket pkt;
//    QVector<AVFrame*>* vFrameVector = new QVector<AVFrame*>();
//    QVector<AVFrame*>* aFrameVector = new QVector<AVFrame*>();
//    int ret = 0;
//    while (1) {
//        ret = 0;
//        ret = av_read_frame(fmt_ctx, &pkt);
//        if (pkt.stream_index != video_index_ &&
//                pkt.stream_index != audio_index_) {
//            av_packet_unref(&pkt);
//            continue;
//        }
//        AVFrame* frame = av_frame_alloc();
//        ret = avcodec_send_packet(
//                    pkt.stream_index == video_index_ ? video_codec_context_ : audio_codec_context_,
//                    &pkt);
//        if (ret == AVERROR_EOF) {
//            break;
//        }
//        else if (ret == AVERROR(EAGAIN)) {
//            continue;
//        }
//        ret = avcodec_receive_frame(
//                    pkt.stream_index == video_index_ ? video_codec_context_ : audio_codec_context_,
//                    frame);
//        if (ret == AVERROR_EOF) {
//            break;
//        }
//        else if (ret == AVERROR(EAGAIN)) {
//            continue;
//        }
//        if (pkt.stream_index == video_index_) {

//            vFrameVector->append(frame);
//        }
//        else if (pkt.stream_index == audio_index_){
//            aFrameVector->append(frame);
//        }
//        if (vMeta == nullptr) {
//            vMeta = new VideoMeta();
//            vMeta->width = video_codec_context_->width;
//            vMeta->height = video_codec_context_->height;
//            vMeta->pixelFormat = video_codec_context_->pix_fmt;
//            vMeta->vTimeBase = av_q2d(video_stream_->time_base);
//            vMeta->vTimeBaseRational = video_stream_->time_base;
//        }
//        if (aMeta == nullptr) {
//            aMeta = new AudioMeta();
//            aMeta->channelLayout = audio_codec_context_->channel_layout == 0 ?
//                                                         frame->channel_layout : audio_codec_context_->channel_layout;
//            aMeta->sampleRate = audio_codec_context_->sample_rate == 0 ?
//                                                        frame->sample_rate : audio_codec_context_->sample_rate;
//            aMeta->sampleFormat = audio_codec_context_->sample_fmt;
//            aMeta->channels = audio_codec_context_->channels == 0 ?
//                                                        frame->channels : audio_codec_context_->channels;
//            aMeta->aTimeBase = av_q2d(audio_stream_->time_base);
//            vMeta->vTimeBaseRational = audio_stream_->time_base;
//        }
//        av_packet_unref(&pkt);
//    }
//    vRoll->setTsBlock(ts->tsIndex(), vFrameVector);
//    aRoll->setTsBlock(ts->tsIndex(), aFrameVector);
//    avformat_free_context(fmt_ctx);
//    QWriteLocker locker(&fetchLock_);
//    fetcherCount_ -= 1;
//    qDebug() << "Decode Ts: " << ts->fileName() <<
//                "; CurrentWorkers: " << fetcherCount_;
//    if (videoThread_ == nullptr) {
//        firstTsDecoded();
//    }
//}

void VideoPlayer::onUpdateImage(QImage image)
{
    image_ = image;
    update();
}

void VideoPlayer::onFirstTsDecoded()
{
    setState(PAUSE);
    videoThread_ = SDL_CreateThread(videoPlay, "videoPlay", this);
    if (!openAudio()) {

    }
}

void VideoPlayer::onDecodeCheck()
{
    int blockIdx = vRoll->tsBlockIndex();
    for (int i = 0; i < 5; i++) {
        if(!vRoll->isBlockEmpty(blockIdx + i)) {
            continue;
        }
        DecodeWorker* worker = new DecodeWorker(index_, blockIdx + i);
        connect(worker, SIGNAL(decodeFinished(QVector<AVFrame*>*, QVector<AVFrame*>*,int, QVariant, QVariant)),
                this, SLOT(onDecodeFinished(QVector<AVFrame*>*, QVector<AVFrame*>*,int, QVariant, QVariant)));
        worker->start();
    }
}

void VideoPlayer::onFetchCheck()
{
    QList<TsFile*> files = index_->getFiles();
    int blockIdx = vRoll->tsBlockIndex();
    for (int i = 0; i < 10; i++) {
        if (!files[blockIdx + i]->fetched()) {
            requestTsFile(files[blockIdx + i]->fileName(), files[blockIdx + i], nullptr);
            qDebug() << "Request Ts: " << files[blockIdx + i]->fileName();
        }
    }
}

void VideoPlayer::onDecodeFinished(QVector<AVFrame*>* vFrameVector, QVector<AVFrame*>* aFrameVector,
                                   int blockIndex, QVariant vMeta, QVariant aMeta)
{
    if (blockIndex == 0) {
        VideoMeta v = vMeta.value<VideoMeta>();
        vMeta_->width = v.width;
        vMeta_->height = v.height;
        vMeta_->pixelFormat = v.pixelFormat;
        vMeta_->vTimeBase = v.vTimeBase;
        vMeta_->vTimeBaseRational = v.vTimeBaseRational;
        AudioMeta a = aMeta.value<AudioMeta>();
        aMeta_->channelLayout = a.channelLayout;
        aMeta_->sampleRate = a.sampleRate;
        aMeta_->sampleFormat = a.sampleFormat;
        aMeta_->channels = a.channels;
        aMeta_->aTimeBase = a.aTimeBase;
        aMeta_->aTimeBaseRational = a.aTimeBaseRational;
        firstTsDecoded();
    }
    vRoll->setTsBlock(blockIndex, vFrameVector);
    aRoll->setTsBlock(blockIndex, aFrameVector);
}

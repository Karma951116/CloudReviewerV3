QT += quick

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += DEBUG
# DEFINES += PROD

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        commentlistmodel.cpp \
        companylistmodel.cpp \
        confighelper.cpp \
        decodeworker.cpp \
        filelistmodel.cpp \
        formatchecker.cpp \
        formattransformer.cpp \
        framequeue.cpp \
        hlsindex.cpp \
        httpfunctions.cpp \
        main.cpp \
        paintcanvas.cpp \
        projectlistmodel.cpp \
        promptprovider.cpp \
        replyparser.cpp \
        replytimeout.cpp \
        runtimecontext.cpp \
        sonic.c \
        stakeholderlistmodel.cpp \
        tsfile.cpp \
        tsroll.cpp \
        user.cpp \
        versionlistmodel.cpp \
        videoplayer.cpp

RESOURCES += \
    Controls.qrc \
    Statics.qrc

include($$PWD/sdl.pri)
include($$PWD/ffmpeg.pri)
include ($$PWD/sortfliter/SortFilterProxyModel.pri)

TRANSLATIONS += \
    CloudReviewerV3_zh_CN.ts

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    commentlistmodel.h \
    companylistmodel.h \
    confighelper.h \
    decodeworker.h \
    def.h \
    emojis.h \
    filelistmodel.h \
    formatchecker.h \
    formattransformer.h \
    framequeue.h \
    hlsindex.h \
    httpfunctions.h \
    paintcanvas.h \
    projectlistmodel.h \
    promptprovider.h \
    replyparser.h \
    replytimeout.h \
    runtimecontext.h \
    sonic.h \
    stakeholderlistmodel.h \
    tsfile.h \
    tsroll.h \
    user.h \
    versionlistmodel.h \
    videoplayer.h

DISTFILES +=

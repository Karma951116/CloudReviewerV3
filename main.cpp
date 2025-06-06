﻿#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QModelIndex>

#include <confighelper.h>
#include <httpfunctions.h>
#include <formatchecker.h>
#include <formattransformer.h>
#include <promptprovider.h>
#include <replyparser.h>
#include <user.h>
#include <companylistmodel.h>
#include <projectlistmodel.h>
#include <commentlistmodel.h>
#include <filelistmodel.h>
#include <stakeholderlistmodel.h>
#include <versionlistmodel.h>
#include <runtimecontext.h>
#include <videoplayer.h>
#include <paintcanvas.h>

#undef main
int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    qmlRegisterType<RuntimeContext>("RuntimeContext",1,0,"RuntimeContext");
    qmlRegisterType<ReplyParser>("FileListModel",1,0,"FileListModel");
    qmlRegisterType<VideoPlayer>("VideoPlayer",1,0,"VideoPlayer");
    qmlRegisterType<PaintCanvas>("PaintCanvas",1,0,"PaintCanvas");

    ConfigHelper* config = new ConfigHelper("Config.ini");
    HttpFunctions* httpFunctions = new HttpFunctions();
    FormatChecker* formatChecker = new FormatChecker();
    FormatTransformer* formatTransformer = new FormatTransformer();
    PromptProvider* promptProvider = new PromptProvider();
    ReplyParser* replyParser = new ReplyParser();
    User* clientUser = new User();
    RuntimeContext* runCtx = new RuntimeContext();

    // 数据模型初始化
    CompanyListModel* companyModel = new CompanyListModel();
    ProjectListModel* projectModel = new ProjectListModel();
    FileListModel* fileModel = new FileListModel();
    CommentListModel* commentModel = new CommentListModel();
    StakeholderListModel* stakeholderModel = new StakeholderListModel();
    VersionListModel* versionModel = new VersionListModel();


    QObject::connect(httpFunctions, &HttpFunctions::replied,
                     replyParser, &ReplyParser::onReplied);
    QObject::connect(replyParser, &ReplyParser::updateTokens,
                     httpFunctions, &HttpFunctions::onUpdateTokens);
    QObject::connect(replyParser, &ReplyParser::updateUser,
                     clientUser, &User::onUpdateUser);
    QObject::connect(replyParser, &ReplyParser::companyReplyDone,
                     companyModel, &CompanyListModel::onCompanyReplyDone);
    QObject::connect(replyParser, &ReplyParser::projectReplyDone,
                     projectModel, &ProjectListModel::onProjectReplyDone);
    QObject::connect(replyParser, &ReplyParser::projectFileReplyDone,
                     fileModel, &FileListModel::onProjectFileReplyDone);
    QObject::connect(replyParser, &ReplyParser::commentReplyDone,
                     commentModel, &CommentListModel::onCommentReplyDone);
    QObject::connect(replyParser, &ReplyParser::stakeholderReplyDone,
                     stakeholderModel, &StakeholderListModel::onStakeholderReplyDone);
    QObject::connect(replyParser, &ReplyParser::versionReplyDone,
                     versionModel, &VersionListModel::onVersionReplyDone);



    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("config", config);
    engine.rootContext()->setContextProperty("httpFunctions", httpFunctions);
    engine.rootContext()->setContextProperty("formatChecker", formatChecker);
    engine.rootContext()->setContextProperty("formatTransformer", formatTransformer);
    engine.rootContext()->setContextProperty("promptProvider", promptProvider);
    engine.rootContext()->setContextProperty("replyParser", replyParser);
    engine.rootContext()->setContextProperty("clientUser", clientUser);
    engine.rootContext()->setContextProperty("companyModel", companyModel);
    engine.rootContext()->setContextProperty("projectModel", projectModel);
    engine.rootContext()->setContextProperty("runtimeCtx", runCtx);
    engine.rootContext()->setContextProperty("fileModel", fileModel);
    engine.rootContext()->setContextProperty("commentModel", commentModel);
    engine.rootContext()->setContextProperty("stakeholderModel", stakeholderModel);
    engine.rootContext()->setContextProperty("versionModel", versionModel);




    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}

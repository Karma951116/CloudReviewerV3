#include <QGuiApplication>
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
#include <runtimecontext.h>
#include <filelistmodel.h>
#include <videoplayer.h>

#undef main
int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    //qmlRegisterType<FileEntity>("FileEntity",1,0,"FileEntity");
    qmlRegisterType<ReplyParser>("FileListModel",1,0,"FileListModel");
    qmlRegisterType<VideoPlayer>("VideoPlayer",1,0,"VideoPlayer");

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



    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}

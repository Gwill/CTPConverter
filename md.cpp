#include <CMdHandler.h>
#include <CMdWrapper.h>
#include <unistd.h>
#include <json/json.h>



static MdConfigure config;
static char buffer[1024*10];

int main(int argc,char * argv[]){
    // 导入配置信息
    CommandOption commandOption(argc,argv);
    if( commandOption.exists("--env") ) {
        // 使用环境变量
        config.loadFromEnvironment();
    } else {
        // 使用命令行参数
        config.loadFromCommandLine(commandOption);
    }

    // 忠诚选项判断
    // 即当父进程退出,本进程也退出
    bool loyalty;
    if( commandOption.exists("--loyalty") ) {
        loyalty = true;
    } else {
        loyalty = false;
    }
    pid_t originalPpid = getppid();

    // 初始化zmq环境
    zmq::context_t context(1);
    zmq::socket_t request  (context, ZMQ_REP);
    //zmq::socket_t response (context, ZMQ_ROUTER);
    zmq::socket_t pushback  (context, ZMQ_PULL);
    zmq::socket_t publish  (context, ZMQ_PUB);

    // 连接对应通讯管道
    request.bind(config.requestPipe);
    //response.bind(config.responsePipe);
    pushback.connect(config.pushbackPipe);
    publish.bind(config.publishPipe);

    std::cout << "main():行情广播地址为:" << config.publishPipe << std::endl;
    sleep(1);    // 给一定时间让订阅者连接上来，以免错过第1条消息


    // 初始化api接口实例
    CMdWrapper api(&config);
    api.init();

    // 定义消息变量
    RequestMessage requestMessage;
    RequestIDMessage requestIDMessage;
    PushbackMessage pushbackMessage;
    PublishMessage publishMessage;

    long timeout = 1;
    long lastTime = s_clock();
    long thisTime = 0;
    long timeDelta = 0;
    long loopTimes = 0;
    long mdCount = 0;

    //std::cout << "main():转发市场报价信息..." << std::endl;
    //api.SubscribeMarketData(config.instrumentIDArray,config.instrumentCount);


    while(1) {
        // 尝试读取通讯管道
        zmq::pollitem_t pullItems [] = {
            { request, 0, ZMQ_POLLIN, 0 },
            { pushback, 0, ZMQ_POLLIN, 0 }
        };
        zmq::poll (pullItems, 1, timeout);

        // 接收到来自客户端的请求
        if (pullItems[0].revents & ZMQ_POLLIN) {
            // 调用对应的api函数
            requestMessage.recv(request);
            std::cout << "apiName" << requestMessage.apiName << std::endl;

        }


        if ( pullItems[1].revents & ZMQ_POLLIN) {

            // 从pushback管道读取消息
            pushbackMessage.recv(pushback);
            publishMessage.header = "PUBLISH";
            publishMessage.apiName = pushbackMessage.apiName;
            publishMessage.respInfo = pushbackMessage.respInfo;
            publishMessage.send(publish);

            // 如果消息是行情数据,将其返回客户端
            if (pushbackMessage.apiName == "OnRtnDepthMarketData") {
                mdCount++;
            }

        }

        // 打印提示信息
        thisTime = s_clock();
        timeDelta = thisTime - lastTime;
        lastTime = thisTime;
        loopTimes += timeDelta;
        if ( loopTimes >= 1000 ) {
            loopTimes = 0;
            std::cout << "main():" << "接收到行情数据:" << mdCount << std::endl;
            mdCount = 0;

            // 忠诚选项的处理
            if (loyalty) {
                // 检查父进程是否还在运行
                pid_t ppid = getppid();
                if ( ppid != originalPpid ) {
                    // 如果父进程不在运行程序退出
                    break;
                }
            }

        }
    }
}

#include <CMdWrapper.h>
#include <json/json.h>
#include <comhelper.h>


/// 构造函数
CMdWrapper::CMdWrapper(MdConfigure * pConfigure) {

    // 读取配置数据信息
    this->pConfigure = pConfigure;

    // 创建CTP API工作对象
    pMdApi = CThostFtdcMdApi::CreateFtdcMdApi();

    // 创建SPI工作对象并让其和API关联
    pMdHandler = new CMdHandler(pConfigure);
    pMdApi->RegisterSpi(pMdHandler);

    // 初始化RequestID序列
    RequestID = 0;

    // 初始化上次出错代码和出错信息
    lastErrorID = 0;
    lastErrorMsg = "";

    ////////////// API方法的wrapper ///////////////

    ///登出请求
    int ReqUserLogout(std::string jsonString);

    ///用户登录请求
    int ReqUserLogin(std::string jsonString);


    std::map<std::string,int (CMdWrapper::*) (std::string)> apiMap;

    int callApiByName(std::string apiName,std::string jsonString);

}

// 将所有api函数映射到名称
void CMdWrapper::initApiMap() {

}


/// 启动CTP连接
void CMdWrapper::init() {

}



/// 获取下一个RequestID序列
int CMdWrapper::getNextRequestID() {
    return 	++this->RequestID;
}

/// 获取当前RequestID序列
int CMdWrapper::getCurrentRequestID() {
    return 	this->RequestID;
}

/// 获取上次出错代码
int CMdWrapper::getLastErrorID() {
    return lastErrorID;
}

/// 获取上次错误信息
std::string CMdWrapper::getLastErrorMsg() {
    return lastErrorMsg;
}




///登出请求
/// 调用成功返回RequestID,失败返回-1
/// 通过查看lastErrorID和lastErrorMsg查看出错的原因
int CMdWrapper::ReqUserLogout(std::string jsonString)
{
    printf("ReqUserLogout():被执行...\n");


    CThostFtdcUserLogoutField data;
    int nRequestID;

    // 解析json格式数据
    try {

        Json::Reader jsonReader;
        Json::Value jsonData;

        if (!jsonReader.parse(jsonString, jsonData)) {
            throw std::exception();
        }

        Json::Value Parameters = jsonData["Parameters"];
        Assert<std::exception>(!Parameters.empty());
        Json::Value Data = Parameters["Data"];
        //Assert<std::exception>(!Data.empty());


        // TODO:这里将pJsonData转化为对应的结构参数

        ///经纪公司代码 char BrokerID[11];
        if (!Data["BrokerID"].empty()) {
            data.BrokerID[sizeof(data.BrokerID)-1] = 0;
            strncpy(data.BrokerID,Data["BrokerID"].asCString(),sizeof(data.BrokerID)-1);
        } else {
            strcpy(data.BrokerID,"");
        }

        ///用户代码 char UserID[16];
        if (!Data["UserID"].empty()) {
            data.UserID[sizeof(data.UserID)-1] = 0;
            strncpy(data.UserID,Data["UserID"].asCString(),sizeof(data.UserID)-1);
        } else {
            strcpy(data.UserID,"");
        }

    } catch (...) {
        lastErrorID = -1001;
        lastErrorMsg = "json数据格式错误";
        return -1;
    }

    // 获取RequestID
    nRequestID = getNextRequestID();

    // 调用对应的CTP API函数
    int result =
        pMdApi->ReqUserLogout(&data, nRequestID);

    // TODO:检查API调用是否失败,并设置LastError信息
    if ( result != 0 ) {
        lastErrorID = result;
        switch(result) {
        case -1 :
            lastErrorMsg = "网络连接失败";
            break;
        case -2 :
            lastErrorMsg = "未处理请求超过许可数";
            break;
        case -3 :
            lastErrorMsg = "每秒发送请求超过许可数";
            break;
        default :
            lastErrorMsg = "未知错误";
        }

        //return result;
        return -1;
    }

    // 如果执行成功重置最近错误信息，并将RequestID返回调用程序
    lastErrorID = 0;
    lastErrorMsg = "";
    return nRequestID;
}


///用户登录请求
/// 调用成功返回RequestID,失败返回-1
/// 通过查看lastErrorID和lastErrorMsg查看出错的原因
int CMdWrapper::ReqUserLogin(std::string jsonString)
{
    printf("ReqUserLogin():被执行...\n");


    CThostFtdcReqUserLoginField data;
    int nRequestID;

    // 解析json格式数据
    try {

        Json::Reader jsonReader;
        Json::Value jsonData;

        if (!jsonReader.parse(jsonString, jsonData)) {
            throw std::exception();
        }

        Json::Value Parameters = jsonData["Parameters"];
        Assert<std::exception>(!Parameters.empty());
        Json::Value Data = Parameters["Data"];
        //Assert<std::exception>(!Data.empty());


        // TODO:这里将pJsonData转化为对应的结构参数

        ///交易日 char TradingDay[9];
        if (!Data["TradingDay"].empty()) {
            data.TradingDay[sizeof(data.TradingDay)-1] = 0;
            strncpy(data.TradingDay,Data["TradingDay"].asCString(),sizeof(data.TradingDay)-1);
        } else {
            strcpy(data.TradingDay,"");
        }

        ///经纪公司代码 char BrokerID[11];
        if (!Data["BrokerID"].empty()) {
            data.BrokerID[sizeof(data.BrokerID)-1] = 0;
            strncpy(data.BrokerID,Data["BrokerID"].asCString(),sizeof(data.BrokerID)-1);
        } else {
            strcpy(data.BrokerID,"");
        }

        ///用户代码 char UserID[16];
        if (!Data["UserID"].empty()) {
            data.UserID[sizeof(data.UserID)-1] = 0;
            strncpy(data.UserID,Data["UserID"].asCString(),sizeof(data.UserID)-1);
        } else {
            strcpy(data.UserID,"");
        }

        ///密码 char Password[41];
        if (!Data["Password"].empty()) {
            data.Password[sizeof(data.Password)-1] = 0;
            strncpy(data.Password,Data["Password"].asCString(),sizeof(data.Password)-1);
        } else {
            strcpy(data.Password,"");
        }

        ///用户端产品信息 char UserProductInfo[11];
        if (!Data["UserProductInfo"].empty()) {
            data.UserProductInfo[sizeof(data.UserProductInfo)-1] = 0;
            strncpy(data.UserProductInfo,Data["UserProductInfo"].asCString(),sizeof(data.UserProductInfo)-1);
        } else {
            strcpy(data.UserProductInfo,"");
        }

        ///接口端产品信息 char InterfaceProductInfo[11];
        if (!Data["InterfaceProductInfo"].empty()) {
            data.InterfaceProductInfo[sizeof(data.InterfaceProductInfo)-1] = 0;
            strncpy(data.InterfaceProductInfo,Data["InterfaceProductInfo"].asCString(),sizeof(data.InterfaceProductInfo)-1);
        } else {
            strcpy(data.InterfaceProductInfo,"");
        }

        ///协议信息 char ProtocolInfo[11];
        if (!Data["ProtocolInfo"].empty()) {
            data.ProtocolInfo[sizeof(data.ProtocolInfo)-1] = 0;
            strncpy(data.ProtocolInfo,Data["ProtocolInfo"].asCString(),sizeof(data.ProtocolInfo)-1);
        } else {
            strcpy(data.ProtocolInfo,"");
        }

        ///Mac地址 char MacAddress[21];
        if (!Data["MacAddress"].empty()) {
            data.MacAddress[sizeof(data.MacAddress)-1] = 0;
            strncpy(data.MacAddress,Data["MacAddress"].asCString(),sizeof(data.MacAddress)-1);
        } else {
            strcpy(data.MacAddress,"");
        }

        ///动态密码 char OneTimePassword[41];
        if (!Data["OneTimePassword"].empty()) {
            data.OneTimePassword[sizeof(data.OneTimePassword)-1] = 0;
            strncpy(data.OneTimePassword,Data["OneTimePassword"].asCString(),sizeof(data.OneTimePassword)-1);
        } else {
            strcpy(data.OneTimePassword,"");
        }

        ///终端IP地址 char ClientIPAddress[16];
        if (!Data["ClientIPAddress"].empty()) {
            data.ClientIPAddress[sizeof(data.ClientIPAddress)-1] = 0;
            strncpy(data.ClientIPAddress,Data["ClientIPAddress"].asCString(),sizeof(data.ClientIPAddress)-1);
        } else {
            strcpy(data.ClientIPAddress,"");
        }

    } catch (...) {
        lastErrorID = -1001;
        lastErrorMsg = "json数据格式错误";
        return -1;
    }

    // 获取RequestID
    nRequestID = getNextRequestID();

    // 调用对应的CTP API函数
    int result =
        pMdApi->ReqUserLogin(&data, nRequestID);

    // TODO:检查API调用是否失败,并设置LastError信息
    if ( result != 0 ) {
        lastErrorID = result;
        switch(result) {
        case -1 :
            lastErrorMsg = "网络连接失败";
            break;
        case -2 :
            lastErrorMsg = "未处理请求超过许可数";
            break;
        case -3 :
            lastErrorMsg = "每秒发送请求超过许可数";
            break;
        default :
            lastErrorMsg = "未知错误";
        }

        //return result;
        return -1;
    }

    // 如果执行成功重置最近错误信息，并将RequestID返回调用程序
    lastErrorID = 0;
    lastErrorMsg = "";
    return nRequestID;
}




// 通过名称调用api
int CMdWrapper::callApiByName(std::string apiName,std::string jsonString) {

    if ( apiMap.find(apiName) != apiMap.end() ) {
        return (this->*apiMap[apiName])(jsonString);
    } else {
        lastErrorID = -1000;
        lastErrorMsg = "没有这个接口函数";
        return -1;
    }
    return 0;
}
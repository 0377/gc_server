#pragma once

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include <string>

#include "stdarg.h" 
#define endStr "JudgeParamEnd"
#define judgeJsonMember(ABC,...)  judgeJsonMemberT(ABC,__VA_ARGS__,endStr)

void webRequestGameServerInfo(std::string& out);

void webRequestGmCommand(rapidjson::Document& document, std::string& out);

void webRequestCashFalse(rapidjson::Document& document, std::string& out);

void webRequestRcharge(rapidjson::Document& document, std::string& out);

void webChangeTax(rapidjson::Document& document, std::string& out);

void webChangeGameCfg(rapidjson::Document& document, std::string& out);

void webGmCommandChangeMoney(rapidjson::Document& document, std::string& out);

void webBroadcastClientUpdate(rapidjson::Document& document, std::string& out);

// lua��������ң����ؽ��
void webLuaCmdPlayerResult(rapidjson::Document& document, std::string& out);
//lua����,��ͬ����ά������֪ͨ��������Ӧ
void webLuaCmdQueryMaintain(rapidjson::Document& document, std::string& out);
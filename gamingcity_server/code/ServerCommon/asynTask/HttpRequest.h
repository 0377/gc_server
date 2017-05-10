#pragma once

#include <string>
#include "boost/asio.hpp"

bool AsioHttpGet(boost::asio::io_service& io_service,std::string url_, std::string pms, std::string & code_ret, std::string & _code_err);
bool AsioHttpGet_AllMsg(boost::asio::io_service& io_service, std::string url_, std::string & code_ret, std::string & _code_err);

bool AsioHttpPost(boost::asio::io_service& io_service, std::string url_, std::string s_message, std::string & code_ret, std::string & _code_err);
bool AsioHttpPost_AllMsg(boost::asio::io_service& io_service, std::string url_, std::string sMsg, std::string & code_ret, std::string & _code_err, std::string & split);


/*
do
{
std::string url = "http://www.cip.cc/171.212.7.164";
std::string code_ret;
std::string code_err;
if (AsioHttpGet_AllMsg(GateSessionManager::instance()->getNetworkServer()->get_io_server_pool().get_io_service(), url, code_ret, code_err))
{
int pos = code_ret.find("<pre>IP");
if (std::string::npos != pos)
{
int pos01 = code_ret.find("</pre>", pos);

if (std::string::npos != pos01)
{
std::string detail = code_ret.substr(pos, pos01 - pos);
int pos02 = detail.find("\t:");
int pos03 = detail.find("\t:", pos02 + 1);
int pos04 = detail.find("\n", pos03 + 1);
if (std::string::npos != pos02 && std::string::npos != pos03 && std::string::npos != pos04)
{
std::string addr = detail.substr(pos03 + 2, pos04 - pos03 - 2);

auto sss =
[](const char src[]){
std::string ans;
if (!src)  //���UTF8�ַ���ΪNULL������˳�
return ans;

wchar_t * lpUnicodeStr = NULL;
int nRetLen = 0;

nRetLen = ::MultiByteToWideChar(CP_UTF8, 0, (char *)src, -1, NULL, NULL);  //��ȡת����Unicode���������Ҫ���ַ��ռ䳤��
lpUnicodeStr = new WCHAR[nRetLen + 1];  //ΪUnicode�ַ����ռ�
nRetLen = ::MultiByteToWideChar(CP_UTF8, 0, (char *)src, -1, lpUnicodeStr, nRetLen);  //ת����Unicode����
if (!nRetLen)  //ת��ʧ��������˳�
{
delete[] lpUnicodeStr;
return ans;
}

nRetLen = ::WideCharToMultiByte(CP_ACP, 0, lpUnicodeStr, -1, NULL, NULL, NULL, NULL);  //��ȡת����GBK���������Ҫ���ַ��ռ䳤��
char* p = new char[nRetLen + 1];
nRetLen = ::WideCharToMultiByte(CP_ACP, 0, lpUnicodeStr, -1, (char *)p, nRetLen, NULL, NULL);  //ת����GBK����
ans.assign(p);

delete[] p;
delete[]lpUnicodeStr;

return ans;
};
std::string anssss = sss(addr.c_str());
LOG_ERR(anssss.c_str());
					}

				}
			}

		}
	} while (0);
*/


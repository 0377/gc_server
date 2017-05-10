// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include "perinclude.h"

#define ASSERT(x) assert(x)
using std::min;
using std::max;

#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

// ��Ϸ����
#define GAME_PLAYER			CGameConfig::GetInstance()->nPlayerCount

#define MAX_PROBABILITY		1000.0f
//#define GAME_FPS			60// del lee 2016.03.07
#define GAME_FPS			30// �޸ĳ�30֡ add lee 2016.03.07
#define MAX_TABLE_CHAIR	4// ÿ���������Ӹ���

#define SCENE_CHANAGE_NONE	 -1

#define	SWITCH_SCENE_END	8


#define SAFE_DELETE(x) { if (NULL != (x)) { delete (x); (x) = NULL; } }

//////////////////////////////////////////////////////////////////////////
inline void DebugString(LPCTSTR lpszFormat, ...)
{
	va_list   args;
	int       nBuf;
	TCHAR     szBuffer[1024];

	va_start(args, lpszFormat);

#if _MSC_VER>1400
	nBuf = _vsnwprintf_s(szBuffer, _TRUNCATE, lpszFormat, args);
#else
	nBuf = _vsnwprintf(szBuffer, CountArray(szBuffer), lpszFormat, args);
#endif

	OutputDebugString(szBuffer);

	va_end(args);
}



static unsigned int g_seed = 0;
static void RandSeed(int seed)
{
	if (!seed) g_seed = GetTickCount();
	else g_seed = seed;
}

static int RandInt(int min, int max)
{
	if (min == max) return min;

	g_seed = 214013 * g_seed + 2531011;

	return min + (g_seed ^ g_seed >> 15) % (max - min);
}

static float RandFloat(float min, float max)
{
	if (min == max) return min;

	g_seed = 214013 * g_seed + 2531011;

	return min + (g_seed >> 16) * (1.0f / 65535.0f) * (max - min);
}


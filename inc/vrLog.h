#pragma once
#include "pch_vrLog.h"
#include "articulated_body.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct _VRMSG
{
	int id_tracker;
	int evt;
	_TRANSFORM tm;
} VRMSG;

VRLOGLIB(HLOG, create_vrlog)();
VRLOGLIB(HLOG, load_vrlog_w)(const wchar_t* logPath);
VRLOGLIB(bool, save_vrlog_w)(HLOG hLog, const wchar_t* path);
VRLOGLIB(void, release_vrlog)(HLOG log);
VRLOGLIB(unsigned int, num_framelogs)(HLOG log);
VRLOGLIB(HLOG_i, getlog_i)(HLOG hLog, unsigned int frame_id);
VRLOGLIB(void, appenditem)(HLOG_i hLog_i, const VRMSG* item);
VRLOGLIB(unsigned int, num_items)(HLOG_i hLog_i);
VRLOGLIB(const VRMSG*, get_item)(HLOG_i hLog_i, unsigned int i_item);

#ifdef __cplusplus
}
#endif
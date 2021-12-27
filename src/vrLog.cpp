#include "pch_vrLog.h"
#include <string>
#include <sstream>
#include <Windows.h>
#include <fstream>      // std::ofstream
#include <vector>
#include <boost/serialization/vector.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include "vrLog.h"

template <typename F, typename T>
T* CAST_2P(F h)
{
#ifdef _DEBUG
	return reinterpret_cast<T*>(h.p);
#else
	return reinterpret_cast<T*>(h);
#endif
}

template <typename F, typename T>
T CAST_2H(F* p)
{
#ifdef _DEBUG
	return {p};
#else
	return p;
#endif
}

#define CAST_2PVRLOG(hVRLog)\
	CAST_2P<HLOG, CVRLog>(hVRLog)

#define CAST_2HVRLOG(pVRLog)\
	CAST_2H<CVRLog, HLOG>(pVRLog)

#define CAST_2PVRLOG_i(hVRLog_i)\
	CAST_2P<HLOG_i, CVRLog_i>(hVRLog_i)

#define CAST_2HVRLOG_i(pVRLog_i)\
	CAST_2H<CVRLog_i, HLOG_i>(pVRLog_i)

class CVRMsg : public VRMSG
{
public:
	CVRMsg(const VRMSG& src)
	{
		id_tracker = src.id_tracker;
		evt = src.evt;
		tm = src.tm;
	}

	CVRMsg()
	{
		id_tracker = -1;
		evt = -1;
		tm = {
				{(Real)1, (Real)1, (Real)1},
				{(Real)1, (Real)0, (Real)0, (Real)0},
				{(Real)0, (Real)0, (Real)0}
			};
	}

	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & id_tracker;
		ar & evt;
		ar & tm.s.x; ar & tm.s.y; ar & tm.s.z;
		ar & tm.r.w; ar & tm.r.x; ar & tm.r.y; ar & tm.r.z;
		ar & tm.tt.x; ar & tm.tt.y; ar & tm.tt.z;
	}
private:
};

class CVRLog_i
{
public:
	CVRLog_i()
	{
	}
	~CVRLog_i()
	{
		for (auto msg : m_msgs)
			delete msg;
	}
	
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & m_msgs;
	}

	void AppendItem(const VRMSG* item)
	{
		CVRMsg* msg = new CVRMsg(*item);
		m_msgs.push_back(msg);
	}

	unsigned int N_Items() const
	{
		return (unsigned int)m_msgs.size();
	}

	const CVRMsg* GetItem(int i_item) const
	{
		return m_msgs[i_item];
	}
private:
	std::vector<CVRMsg*> m_msgs;
};

class CVRLog
{
public:
	CVRLog()
	{
	}

	~CVRLog()
	{
		for (auto log_i : m_frameLog)
			delete log_i;
	}

	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & m_frameLog;
	}

	CVRLog_i* GetLog_i(unsigned int frame_id)
	{
		int n_frames = (int)m_frameLog.size();
		int n_trails = frame_id+1-n_frames;
		if (n_trails > 0)
		{
			m_frameLog.resize(frame_id + 1, NULL);
			for (int i_trail = 0; i_trail < n_trails; i_trail++)
			{
				m_frameLog[i_trail + n_frames] = new CVRLog_i();
			}
		}
		return m_frameLog[frame_id];
	}

	unsigned int N_FrameLogs() const
	{
		return (unsigned int)m_frameLog.size();
	}

private:
	std::vector<CVRLog_i*> m_frameLog;
};


HLOG create_vrlog()
{
	return CAST_2HVRLOG(new CVRLog());
}

HLOG load_vrlog_w(const wchar_t* path)
{
	CVRLog* log = new CVRLog();
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::string path_c = converter.to_bytes(path);
	// create and open a character archive for output
	std::ifstream ifs(path_c.c_str(), std::ifstream::binary);
	if (std::ios_base::failbit == ifs.rdstate())
		return H_INVALID;
	boost::archive::binary_iarchive ia(ifs);
	ia >> *log;
	return CAST_2HVRLOG(log);
}

bool save_vrlog_w(HLOG hLog, const wchar_t* path)
{
	CVRLog* log = CAST_2PVRLOG(hLog);
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::string path_c = converter.to_bytes(path);
	// create and open a character archive for output
	std::ofstream ofs(path_c.c_str(), std::ifstream::binary);
	if (std::ios_base::failbit == ofs.rdstate())
		return false;
	boost::archive::binary_oarchive oa(ofs);
	oa << *log;
	return true;
}

void release_vrlog(HLOG hLog)
{
	CVRLog* pLog = CAST_2PVRLOG(hLog);
	delete pLog;
}

HLOG_i getlog_i(HLOG hLog, unsigned int frame_id)
{
	CVRLog* pLog = CAST_2PVRLOG(hLog);
	if (NULL != pLog)
		return CAST_2HVRLOG_i(pLog->GetLog_i(frame_id));
	else
		return H_INVALID;
}

unsigned int num_framelogs(HLOG hLog)
{
	CVRLog* pLog = CAST_2PVRLOG(hLog);
	if (NULL != pLog)
		return pLog->N_FrameLogs();
	else
		return 0;	
}


void appenditem(HLOG_i hLog_i, const VRMSG* item)
{
	CVRLog_i* log_i = CAST_2PVRLOG_i(hLog_i);
	if (NULL != log_i)
		log_i->AppendItem(item);
}

unsigned int num_items(HLOG_i hLog_i)
{
	CVRLog_i* log_i = CAST_2PVRLOG_i(hLog_i);
	if (NULL != log_i)
		return log_i->N_Items();
	else
		return 0;
}

const VRMSG* get_item(HLOG_i hLog_i, unsigned int i_item)
{
	CVRLog_i* log_i = CAST_2PVRLOG_i(hLog_i);
	if (NULL != log_i && i_item < log_i->N_Items())
		return log_i->GetItem(i_item);
	else
		return NULL;
}
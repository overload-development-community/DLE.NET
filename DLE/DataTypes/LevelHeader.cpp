#include "stdafx.h"

int CLevelHeader::Read(IFileManager* fp)
{
	if (fp->Read(m_name, 1, sizeof(m_name)) != sizeof(m_name))
		return 0;
	m_name[sizeof(m_name) - 1] = '\0';
	_strlwr_s(m_name);
	if (fp->Read(&m_size, sizeof(m_size), 1) != 1)
		return 1;
	if ((m_bExtended = m_size < 0)) {
		if (fp->Read(m_longName, 1, sizeof(m_longName)) != sizeof(m_longName))
			return 0;
		m_longName[sizeof(m_longName) - 1] = '\0';
		_strlwr_s(m_longName);
	}
	else
		m_longName[0] = '\0';
	return 1;
}

int CLevelHeader::Write(IFileManager* fp, int bExtended)
{
	if (fp->Write(m_name, 1, sizeof(m_name)) != sizeof(m_name))
		return 0;
	m_bExtended = (bExtended < 0) ? (g_data.LevelVersion() >= 9) : bExtended;
	m_size = m_bExtended ? -abs(m_size) : abs(m_size);
	if (fp->Write(&m_size, sizeof(m_size), 1) != 1)
		return 1;
	if (m_bExtended) {
		if (fp->Write(m_longName, 1, sizeof(m_longName)) != sizeof(m_longName))
			return 0;
	}
	return 1;
}

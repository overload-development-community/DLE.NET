/*
THE COMPUTER CODE CONTAINED HEREIN IS THE SOLE PROPERTY OF PARALLAX
SOFTWARE CORPORATION ("PARALLAX").  PARALLAX, IN DISTRIBUTING THE CODE TO
END-USERS, AND SUBJECT TO ALL OF THE TERMS AND CONDITIONS HEREIN, GRANTS A
ROYALTY-FREE, PERPETUAL LICENSE TO SUCH END-USERS FOR USE BY SUCH END-USERS
IN USING, DISPLAYING,  AND CREATING DERIVATIVE WORKS THEREOF, SO LONG AS
SUCH USE, DISPLAY OR CREATION IS FOR NON-COMMERCIAL, ROYALTY OR REVENUE
FREE PURPOSES.  IN NO EVENT SHALL THE END-USER USE THE COMPUTER CODE
CONTAINED HEREIN FOR REVENUE-BEARING PURPOSES.  THE END-USER UNDERSTANDS
AND AGREES TO THE TERMS HEREIN AND ACCEPTS THE SAME BY USE OF THIS FILE.
COPYRIGHT 1993-1999 PARALLAX SOFTWARE CORPORATION.  ALL RIGHTS RESERVED.
*/

#ifdef HAVE_CONFIG_H
#include <conf.h>
#endif

#include <stdio.h>
#include <string.h>
#if defined (_WIN32_WCE) || defined (_WIN32)
#	include <sys/stat.h>
#else
#	include <sys/stat.h>
#	include <errno.h>
#endif

#include "MemoryFile.h"
//#include "findfile.h"

// ----------------------------------------------------------------------------

bool CMemoryFile::Create (size_t size)
{
m_buffer = new ubyte [size];
if (m_buffer == null)
	return false;
m_bLocalBuffer = true;
m_info.size = (long) size;
m_info.position = 0;
return true;
}

// ----------------------------------------------------------------------------

int CMemoryFile::Open (const char *filename, const char *mode) 
{
if (!CFileManager::Open (filename, mode))
	return 0;

if (!Create (m_info.size)) {
	Close ();
	return 0;
	}

if (CFileManager::Read (m_buffer, 1, m_info.size) != m_info.size)
	Close ();

CFileManager::Close (false);
m_info.position = 0;
return (m_buffer != null);
}

// ----------------------------------------------------------------------------

int CMemoryFile::Open (ubyte* buffer, long bufSize) 
{
m_buffer = buffer;
m_info.size = bufSize;
m_info.position = 0;
m_bLocalBuffer = false;
return (m_buffer != null);
}

// ----------------------------------------------------------------------------
// Write () writes to the file
//
// returns:   number of full elements actually written
//
//
int CMemoryFile::Write (const void *buffer, int nElemSize, int nElemCount)
{
return 0;
}

// ----------------------------------------------------------------------------
// CMemoryFile::PutC () writes a character to a file
//
// returns:   success ==> returns character written
//            error   ==> EOF
//
int CMemoryFile::PutC (int c)
{
return 0;
}

// ----------------------------------------------------------------------------

int CMemoryFile::GetC (void) 
{
if (m_info.position >= m_info.size) 
	return EOF;
return (int) m_buffer [m_info.position++];
}

// ----------------------------------------------------------------------------
// CMemoryFile::PutS () writes a string to a file
//
// returns:   success ==> non-negative value
//            error   ==> EOF
//
int CMemoryFile::PutS (const char *str)
{
return 0;
}

// ----------------------------------------------------------------------------

char * CMemoryFile::GetS (char * buffer, size_t n) 
{
	char * t = buffer;
	size_t i;
	int c;

for (i = 0; i < n - 1; i++) {
	do {
		if (m_info.position >= m_info.size) {
			*buffer = 0;
			return (buffer == t) ? null : t;
			}
		c = GetC ();
		if (c == 0 || c == 10)       // Unix line ending
			break;
		if (c == 13) {      // Mac or DOS line ending
			int c1 = GetC ();
			Seek (-1, SEEK_CUR);
			if (c1 == 10) // DOS line ending
				continue;
			else            // Mac line ending
				break;
			}
		} while (c == 13);
 	if (c == 0 || c == 10 || c == 13)  // because cr-lf is a bad thing on the mac
 		break;   // and anyway -- 0xod is CR on mac, not 0x0a
	*buffer++ = c;
	if (c == '\n')
		break;
	}
*buffer++ = 0;
return  t;
}

// ----------------------------------------------------------------------------

size_t CMemoryFile::Read (void *buffer, size_t elSize, size_t nElems) 
{
uint size = (int) (elSize * nElems);
if (size == 0)
	return 0;
if (!m_buffer || (m_info.size < 1))
	return 0;
uint available = m_info.size - m_info.position;
if (size > available)
	size = uint ((available / elSize) * elSize);
if (size == 0)
	return 0;
memcpy (buffer, m_buffer + m_info.position, size);
m_info.position += size;
return size / elSize;
}

// ----------------------------------------------------------------------------

int CMemoryFile::Tell (void) 
{
return m_info.position;
}

// ----------------------------------------------------------------------------

int CMemoryFile::Seek (size_t offset, int whence) 
{
if (m_buffer == null)
	return -1;
if (whence == SEEK_SET)
	m_info.position = (long) offset;
else if (whence == SEEK_CUR)
	m_info.position += (long) offset;
else if (whence == SEEK_END)
	m_info.position -= (long) offset;
if (m_info.position < 0)
	m_info.position = 0;
else if (m_info.position >= m_info.size)
	m_info.position = m_info.size - 1;
return m_info.position;
}

// ----------------------------------------------------------------------------

int CMemoryFile::Close (bool bReset)
{
if (m_buffer != null) {
	if (m_bLocalBuffer)
		delete m_buffer;
	m_buffer = null;
	}
m_info.size = 0;
m_info.position = -1;
return 1;
}

// ----------------------------------------------------------------------------

int CMemoryFile::Load (CFileManager* fp, size_t size)
{
if (!Create (size))
	return 0;
if (fp->Read (m_buffer, 1, size) != size) {
	Close ();
	return 0;
	}
m_info.name = fp->Name ();
return 1;
}

// ----------------------------------------------------------------------------

int CMemoryFile::Load (const ubyte* buffer, size_t size)
{
if ((buffer == null) || (size == 0) || !Create (size))
	return 0;
memcpy (m_buffer, buffer, size);
return 1;
}

// ----------------------------------------------------------------------------

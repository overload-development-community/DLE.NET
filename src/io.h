#ifndef __io_h
#define __io_h

#if 0

double ReadDouble (CFileManager* fp);
int ReadInt32 (CFileManager* fp);
short ReadInt16 (CFileManager* fp);
char ReadInt8 (CFileManager* fp);
uint ReadUInt32 (CFileManager* fp);
ushort ReadUInt16 (CFileManager* fp);
ubyte ReadUInt8 (CFileManager* fp);
void* ReadBytes (void* buffer, uint length, CFileManager* fp);

#define ReadInt32(_fp)		(int) ReadInt32 (_fp)
#define ReadFixAng(_fp)	(short) ReadInt16 (_fp)

int WriteInt32 (int value, CFileManager* fp);
short WriteInt16 (short value, CFileManager* fp);
char WriteInt8 (char value, CFileManager* fp);
uint WriteUInt32 (uint value, CFileManager* fp);
ushort WriteUInt16 (ushort value, CFileManager* fp);
ubyte WriteUInt8 (ubyte value, CFileManager* fp);
void* WriteBytes (void* buffer, uint length, CFileManager* fp);

#define WriteInt32(_value, _fp)		WriteInt32 ((int) (_value), _fp)
#define WriteInt16(_value, _fp)	WriteInt32 ((short) (_value), _fp)

double WriteDouble (double value, CFileManager* fp);

LPSTR CFileManager::SplitPath (LPSTR fullName, LPSTR pathName, LPSTR fileName, LPSTR extName);

#endif

char *TimeStr (char *pszTime, int nDestSize);
char *DateStr (char *pszTime, int nDestSize, bool bMonthNames = false);
char *DateTimeStr (char *pszTime, int nDestSize, bool bMonthNames = false);

#endif //__io_h
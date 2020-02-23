#ifndef __timedate_h
#define __timedate_h

char *TimeStr (char *pszTime, int nDestSize);

char *DateStr (char *pszTime, int nDestSize, bool bMonthNames = false);

char *DateTimeStr (char *pszTime, int nDestSize, bool bMonthNames = false);

#endif //__timedate_h
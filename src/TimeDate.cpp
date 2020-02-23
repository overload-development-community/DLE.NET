
//------------------------------------------------------------------------------

#include <stdio.h>
#include <time.h>

static char *szMonths [] = {"jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec"};

struct tm *GetTimeDate (struct tm *td)
{
   time_t t;
   static struct tm h;

time (&t);
localtime_s (&h, &t);
h.tm_mon++;
if (!td)
   return &h;
*td = h;
td->tm_year += (td->tm_year < 80 ? 2000 : 1900);
return td;
}

//------------------------------------------------------------------------------

char *TimeStr (char *pszTime, int nDestSize)
{
	struct tm td;

GetTimeDate (&td);
sprintf_s (pszTime, nDestSize, "%d:%02d.%02d", td.tm_hour, td.tm_min, td.tm_sec);
return pszTime;
} 

//------------------------------------------------------------------------------

char *DateStr (char *pszTime, int nDestSize, bool bMonthNames)
{
	struct tm td;

GetTimeDate (&td);
if (bMonthNames)
	sprintf_s (pszTime, nDestSize, "%d %s %d", td.tm_mday, szMonths [td.tm_mon - 1], td.tm_year);
else
	sprintf_s (pszTime, nDestSize, "%d/%d/%d", td.tm_mon, td.tm_mday, td.tm_year);
return pszTime;
} 

//------------------------------------------------------------------------------

char *TimeDateStr (char *pszTime, int nDestSize, bool bMonthNames)
{
	struct tm td;

GetTimeDate (&td);
if (bMonthNames)
	sprintf_s (pszTime, nDestSize, "%d %s %d %d:%02d",
		       td.tm_mday, szMonths [td.tm_mon - 1], td.tm_year,
			    td.tm_hour, td.tm_min);
else
	sprintf_s (pszTime, nDestSize, "%d/%d/%d %d:%02d",
		       td.tm_mon, td.tm_mday, td.tm_year,
			    td.tm_hour, td.tm_min);
return pszTime;
} 

//------------------------------------------------------------------------------


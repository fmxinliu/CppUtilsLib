#include "StdAfx.h"
#include "DateTime.h"
#include <time.h>

namespace UtilTools
{
    String DateTime::toString()
    {
        // 基于当前系统的当前日期/时间
        time_t now = time(0);
        //printf("1970 到目前经过秒数:%ld\n", now);

        struct tm *ltm;
        TCHAR buffer[80];
        ltm = localtime(&now);

        _tcsftime(buffer, 80, _T("%Y-%m-%d %H:%M:%S"), ltm);
        //_tprintf(_T("格式化的日期 & 时间 : |%s|\n"), buffer);

        String dateTimeString(buffer);
        return dateTimeString;
    }

    String DateTime::toString(String format)
    {
        time_t now;
        struct tm *ltm;
        TCHAR buffer[80];

        time(&now);
        ltm = localtime(&now);

        if (_T("yyyy-MM-dd") == format) {
            _tcsftime(buffer, 80, _T("%Y-%m-%d"), ltm);
        } else if (_T("yyyy/MM/dd") == format) {
            _tcsftime(buffer, 80, _T("%Y/%m/%d"), ltm);
        } else if (_T("hh:mm:ss") == format) {
            _tcsftime(buffer, 80, _T("%H:%M:%S"), ltm);
        } else if (_T("yyyy年MM月dd日 hh时mm分ss秒")) {
            _tcsftime(buffer, 80, _T("%Y年%m月%d日 %H时%M分%S秒"), ltm);
        } else {
            _tcsftime(buffer, 80, _T("%Y-%m-%d %H:%M:%S"), ltm);
        }

        String dateTimeString(buffer);
        return dateTimeString;
    }
}

#include "StdAfx.h"
#include "DateTime.h"
#include <time.h>

namespace UtilTools
{
    String DateTime::toString()
    {
        // ���ڵ�ǰϵͳ�ĵ�ǰ����/ʱ��
        time_t now = time(0);
        //printf("1970 ��Ŀǰ��������:%ld\n", now);

        struct tm *ltm;
        TCHAR buffer[80];
        ltm = localtime(&now);

        _tcsftime(buffer, 80, _T("%Y-%m-%d %H:%M:%S"), ltm);
        //_tprintf(_T("��ʽ�������� & ʱ�� : |%s|\n"), buffer);

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
        } else if (_T("yyyy��MM��dd�� hhʱmm��ss��")) {
            _tcsftime(buffer, 80, _T("%Y��%m��%d�� %Hʱ%M��%S��"), ltm);
        } else {
            _tcsftime(buffer, 80, _T("%Y-%m-%d %H:%M:%S"), ltm);
        }

        String dateTimeString(buffer);
        return dateTimeString;
    }
}

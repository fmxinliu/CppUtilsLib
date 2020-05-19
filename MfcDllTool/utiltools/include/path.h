#pragma once
#include "dllexport.h"

namespace UtilTools
{
    class DLL_API Path
    {
    public:
        // �жϴ��ڡ��ɶ�����д����ִ��
        STATIC bool isExist(const String &filename);
        STATIC bool isReadable(const String &filename);
        STATIC bool isWriteable(const String &filename);
        STATIC bool isExecutable(const String &filename);
        STATIC bool isReadwrite(const String &filename);

        // �ж��ļ� or Ŀ¼
        STATIC bool isFile(const String &pathname);
        STATIC bool isFolder(const String &pathname);

        // ���� && ɾ��Ŀ¼
        STATIC bool mkdir(const String &pathname);
        STATIC bool rmdir(const String &pathname);

        // ɾ���ļ�
        STATIC bool rmove(const String &pathname);

        // ������ or �ƶ�(Ŀ��Ŀ¼�������)
        STATIC bool rename(const String &oldname, const String &newname, bool overwrite);
    };
}

#pragma once
#include "dllexport.h"

namespace UtilTools
{
    class DLL_API Path
    {
    public:
        // 判断存在、可读、可写、可执行
        STATIC bool isExist(const String &filename);
        STATIC bool isReadable(const String &filename);
        STATIC bool isWriteable(const String &filename);
        STATIC bool isExecutable(const String &filename);
        STATIC bool isReadwrite(const String &filename);

        // 判断文件 or 目录
        STATIC bool isFile(const String &pathname);
        STATIC bool isFolder(const String &pathname);

        // 创建 && 删除目录
        STATIC bool mkdir(const String &pathname);
        STATIC bool rmdir(const String &pathname);

        // 删除文件
        STATIC bool rmove(const String &pathname);

        // 重命名 or 移动(目标目录必须存在)
        STATIC bool rename(const String &oldname, const String &newname, bool overwrite);
    };
}

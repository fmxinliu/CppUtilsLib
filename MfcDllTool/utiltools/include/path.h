#pragma once
#include "dllexport.h"

namespace UtilTools
{
    enum FileTimeOptions { CreateTime, LastWriteTime, LastAccessTime };
    enum FileHiddenOptions { UnHidden, Hidden };
    enum FileReadOnlyOptions { Writable, ReadOnly };
    enum FolderSelfSizeOptions { Exclude, Include };

    class DLL_API Path
    {
    public:
        // 判断存在、可读、可写、可执行
        STATIC bool isExist(const String &path);
        STATIC bool isReadable(const String &path);
        STATIC bool isWriteable(const String &path);
        STATIC bool isExecutable(const String &path);
        STATIC bool isReadwrite(const String &path);

        // 属性
        STATIC long getSelfSizeAttr(const String &path); // 文件/目录自身大小（不包括目录中文件大小）
        STATIC long getSizeAttr(const String &path, FolderSelfSizeOptions options);
        STATIC String getTimeAttr(const String &path, FileTimeOptions options);
        STATIC bool setHiddenAttr(const String &path, FileHiddenOptions options);
        STATIC bool setReadOnlyAttr(const String &path, FileReadOnlyOptions options);
        STATIC bool setReadOnlyAttr(const String &path, FileReadOnlyOptions options, bool recursionFiles);
        STATIC bool isHidden(const String &path);
        STATIC bool isReadOnly(const String &path);

        // 判断文件 or 目录
        STATIC bool isFile(const String &path);
        STATIC bool isFolder(const String &path);

        // 创建 && 删除目录
        STATIC bool mkdir(const String &path);
        STATIC bool rmdir(const String &path);

        // 删除文件
        STATIC bool remove(const String &path);

        // 重命名 or 移动(目标目录必须存在)
        STATIC bool rename(const String &oldname, const String &newname, bool overwrite);

        // 路径
        STATIC String getAppPath();
        STATIC String getCurWorkDir();
        STATIC String getDirName(const String &path);
        STATIC String getExtName(const String &path);
        STATIC String getFileName(const String &path);
        STATIC String getFileNameWithoutExtension(const String &path);
        STATIC bool hasExtension(const String &path);
    };
}

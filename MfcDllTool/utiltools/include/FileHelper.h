#pragma once
#include "dllexport.h"
#include <vector>

namespace UtilTools
{
    enum LineEndOptions { IgnoreNone, IgnoreCR };
    enum OverWriteOptions { SkipIfExist, OverWriteIfExist };
    enum BlankLineOptions { RetainBlankLine, RemoveBlankLine };
    enum LineCountOptions { UnCountBlankLine, CountBlankLine };
    enum FileAppendOptions { OverWriteFile, AppendFile };

    class DLL_API FileHelper
    {
    public:
        // 文件基本操作
        STATIC bool create(const String &filename);
        STATIC bool exists(const String &filename);
        STATIC bool remove(const String &filename);
        STATIC bool copy(const String &sourceFileName, const String &destFileName, BOOL skipIfExist = SkipIfExist);
        STATIC bool move(const String &sourceFileName, const String &destFileName, BOOL skipIfExist = SkipIfExist);

        // 文件属性
        STATIC long size(const String &filename);
        STATIC long length(const String &filename, BOOL ignoreLineEndCR = IgnoreNone);
        STATIC long lineCount(const String &filename, BOOL countBlankLine = CountBlankLine);
        STATIC String createTime(const String &filename);
        STATIC String lastWriteTime(const String &filename);
        STATIC String lastAccessTime(const String &filename);
        STATIC bool empty(const String &filename);
        STATIC bool setHidden(const String &filename);
        STATIC bool setUnHidden(const String &filename);
        STATIC bool setReadOnly(const String &filename);
        STATIC bool setWritable(const String &filename);

        // 写文件
        STATIC bool write(const String &path, const String &contents, BOOL append = OverWriteFile);
        STATIC bool write(const String &path, const unsigned char *contents, size_t length, BOOL append = OverWriteFile); // 按字节写入
        STATIC bool write(const String &path, const std::vector<String> &contents, const String &separator = _T("\n"), BOOL append = OverWriteFile);

        // 读文件
        STATIC bool read(const String &path, String &contents);
        STATIC bool read(const String &path, std::vector<String> &contents, BOOL removeBlankLine = RetainBlankLine);
        STATIC bool read(const String &path, std::vector<std::vector<String>> &contents, const TCHAR &separator, BOOL removeBlankLine = RetainBlankLine);

        // 安全写文件
        STATIC bool safeSave(const String &path, const String &contents, BOOL append = OverWriteFile);
        STATIC bool safeSave(const String &path, const std::vector<String> &contents, BOOL append = OverWriteFile);
    };
}

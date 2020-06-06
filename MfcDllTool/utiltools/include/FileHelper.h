#pragma once
#include "dllexport.h"
#include <vector>

namespace UtilTools
{
    enum LineEndOptions { IgnoreNone, IgnoreCR };

    class DLL_API FileHelper
    {
    public:
        // 文件基本操作
        STATIC bool create(const String &filename);
        STATIC bool exists(const String &filename);
        STATIC bool remove(const String &filename);
        STATIC bool copy(const String &sourceFileName, const String &destFileName, bool overwrite = false);
        STATIC bool move(const String &sourceFileName, const String &destFileName, bool overwrite = false);

        // 文件属性
        STATIC bool empty(const String &filename);
        STATIC INT64 size(const String &filename);
        STATIC INT64 length(const String &filename, LineEndOptions options = IgnoreNone);
        STATIC String createTime(const String &filename);
        STATIC String lastWriteTime(const String &filename);
        STATIC String lastAccessTime(const String &filename);

        // 写文件：覆盖方式
        STATIC bool write(const String &path, const String &contents);
        STATIC bool write(const String &path, const std::vector<String> &contents, const String &separator = _T(""));

        // 写文件：追加方式
        STATIC bool append(const String &path, const String &contents);
        STATIC bool append(const String &path, const std::vector<String> &contents, const String &separator = _T(""));

        // 写文件：追加方式，写行
        STATIC bool appendLine(const String &path, const String &contents);
        STATIC bool appendLine(const String &path, const std::vector<String> &contents, const String &separator = _T(""));

        // 读文件：所有文本、按行读、分割行
        STATIC bool read(const String &path, String &contents);
        STATIC bool readLines(const String &path, std::vector<String> &contents, bool removeBlankLine = false);
        STATIC bool readLines(const String &path, std::vector<std::vector<String>> &contents, const TCHAR &separator, bool removeBlankLine = false);

        // 读大文件
        STATIC bool readLinesEx(const String &path, std::vector<String> &contents, bool removeBlankLine = false);
        STATIC bool readLinesEx(const String &path, std::vector<std::vector<String>> &contents, const TCHAR &separator, bool removeBlankLine = false);

        // 写大文件
        STATIC bool writeEx(const String &path, const std::vector<String> &contents, const String &separator = _T(""));
        STATIC bool appendEx(const String &path, const std::vector<String> &contents, const String &separator = _T(""));
        STATIC bool appendLineEx(const String &path, const std::vector<String> &contents, const String &separator = _T(""));
    };
}

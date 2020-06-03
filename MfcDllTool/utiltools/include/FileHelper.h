#pragma once
#include "dllexport.h"
#include <vector>

namespace UtilTools
{
    class DLL_API FileHelper
    {
    public:
        // 文件基本操作
        STATIC bool create(const String &filename);
        STATIC bool exists(const String &filename);
        STATIC bool remove(const String &filename);
        STATIC bool copy(const String &sourceFileName, const String &destFileName, bool overwrite = false);
        STATIC bool move(const String &sourceFileName, const String &destFileName, bool overwrite = false);

        // 写文件：覆盖方式
        STATIC bool write(const String &path, const String &contents);
        STATIC bool write(const String &path, const std::vector<String> &contents);
        STATIC bool write(const String &path, const std::vector<String> &contents, const String &separator);

        // 写文件：追加方式
        STATIC bool append(const String &path, const String &contents);
        STATIC bool append(const String &path, const std::vector<String> &contents);
        STATIC bool append(const String &path, const std::vector<String> &contents, const String &separator);

        // 写文件：追加方式，写行
        STATIC bool appendLine(const String &path, const String &contents);
        STATIC bool appendLine(const String &path, const std::vector<String> &contents);
        STATIC bool appendLine(const String &path, const std::vector<String> &contents, const String &separator);

        // 读文件
        STATIC bool read(const String &path, String &contents);
        STATIC bool read(const String &path, std::vector<String> &contents, bool removeBlankLine);
        STATIC bool readLines(const String &path, std::vector<String> &contents);
        STATIC bool readLines(const String &path, std::vector<String> &contents, bool removeBlankLine);
    };
}

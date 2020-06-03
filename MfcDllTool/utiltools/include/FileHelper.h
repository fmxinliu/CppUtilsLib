#pragma once
#include "dllexport.h"
#include <vector>

namespace UtilTools
{
    class DLL_API FileHelper
    {
    public:
        // �ļ���������
        STATIC bool create(const String &filename);
        STATIC bool exists(const String &filename);
        STATIC bool remove(const String &filename);
        STATIC bool copy(const String &sourceFileName, const String &destFileName, bool overwrite = false);
        STATIC bool move(const String &sourceFileName, const String &destFileName, bool overwrite = false);

        // д�ļ������Ƿ�ʽ
        STATIC bool write(const String &path, const String &contents);
        STATIC bool write(const String &path, const std::vector<String> &contents);
        STATIC bool write(const String &path, const std::vector<String> &contents, const String &separator);

        // д�ļ���׷�ӷ�ʽ
        STATIC bool append(const String &path, const String &contents);
        STATIC bool append(const String &path, const std::vector<String> &contents);
        STATIC bool append(const String &path, const std::vector<String> &contents, const String &separator);

        // д�ļ���׷�ӷ�ʽ��д��
        STATIC bool appendLine(const String &path, const String &contents);
        STATIC bool appendLine(const String &path, const std::vector<String> &contents);
        STATIC bool appendLine(const String &path, const std::vector<String> &contents, const String &separator);

        // ���ļ�
        STATIC bool read(const String &path, String &contents);
        STATIC bool read(const String &path, std::vector<String> &contents, bool removeBlankLine);
        STATIC bool readLines(const String &path, std::vector<String> &contents);
        STATIC bool readLines(const String &path, std::vector<String> &contents, bool removeBlankLine);
    };
}

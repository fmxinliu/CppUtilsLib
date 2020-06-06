#pragma once
#include "dllexport.h"
#include <vector>

namespace UtilTools
{
    enum LineEndOptions { IgnoreNone, IgnoreCR };

    class DLL_API FileHelper
    {
    public:
        // �ļ���������
        STATIC bool create(const String &filename);
        STATIC bool exists(const String &filename);
        STATIC bool remove(const String &filename);
        STATIC bool copy(const String &sourceFileName, const String &destFileName, bool overwrite = false);
        STATIC bool move(const String &sourceFileName, const String &destFileName, bool overwrite = false);

        // �ļ�����
        STATIC bool empty(const String &filename);
        STATIC INT64 size(const String &filename);
        STATIC INT64 length(const String &filename, LineEndOptions options = IgnoreNone);
        STATIC String createTime(const String &filename);
        STATIC String lastWriteTime(const String &filename);
        STATIC String lastAccessTime(const String &filename);

        // д�ļ������Ƿ�ʽ
        STATIC bool write(const String &path, const String &contents);
        STATIC bool write(const String &path, const std::vector<String> &contents, const String &separator = _T(""));

        // д�ļ���׷�ӷ�ʽ
        STATIC bool append(const String &path, const String &contents);
        STATIC bool append(const String &path, const std::vector<String> &contents, const String &separator = _T(""));

        // д�ļ���׷�ӷ�ʽ��д��
        STATIC bool appendLine(const String &path, const String &contents);
        STATIC bool appendLine(const String &path, const std::vector<String> &contents, const String &separator = _T(""));

        // ���ļ��������ı������ж����ָ���
        STATIC bool read(const String &path, String &contents);
        STATIC bool readLines(const String &path, std::vector<String> &contents, bool removeBlankLine = false);
        STATIC bool readLines(const String &path, std::vector<std::vector<String>> &contents, const TCHAR &separator, bool removeBlankLine = false);

        // �����ļ�
        STATIC bool readLinesEx(const String &path, std::vector<String> &contents, bool removeBlankLine = false);
        STATIC bool readLinesEx(const String &path, std::vector<std::vector<String>> &contents, const TCHAR &separator, bool removeBlankLine = false);

        // д���ļ�
        STATIC bool writeEx(const String &path, const std::vector<String> &contents, const String &separator = _T(""));
        STATIC bool appendEx(const String &path, const std::vector<String> &contents, const String &separator = _T(""));
        STATIC bool appendLineEx(const String &path, const std::vector<String> &contents, const String &separator = _T(""));
    };
}

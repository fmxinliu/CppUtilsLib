#pragma once
#include "dllexport.h"
#include <vector>

namespace UtilTools
{
    enum LineEndOptions { IgnoreNone, IgnoreCR };
    enum OverWriteOptions { SkipIfExist, OverWriteIfExist };
    enum BlankLineOptions { RetainBlankLine, RemoveBlankLine };
    enum LineCountOptions { UnCountBlankLine, CountBlankLine };

    class DLL_API FileHelper
    {
    public:
        // �ļ���������
        STATIC bool create(const String &filename);
        STATIC bool exists(const String &filename);
        STATIC bool remove(const String &filename);
        STATIC bool copy(const String &sourceFileName, const String &destFileName, OverWriteOptions options = SkipIfExist);
        STATIC bool move(const String &sourceFileName, const String &destFileName, OverWriteOptions options = SkipIfExist);

        // �ļ�����
        STATIC INT64 size(const String &filename);
        STATIC INT64 length(const String &filename, LineEndOptions options = IgnoreNone);
        STATIC INT64 lineCount(const String &filename, LineCountOptions options = CountBlankLine);
        STATIC String createTime(const String &filename);
        STATIC String lastWriteTime(const String &filename);
        STATIC String lastAccessTime(const String &filename);
        STATIC bool empty(const String &filename);
        STATIC bool setHidden(const String &filename);
        STATIC bool setUnHidden(const String &filename);
        STATIC bool setReadOnly(const String &filename);
        STATIC bool setWritable(const String &filename);

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
        STATIC bool readLines(const String &path, std::vector<String> &contents, BlankLineOptions options = RetainBlankLine);
        STATIC bool readLines(const String &path, std::vector<std::vector<String>> &contents, const TCHAR &separator, BlankLineOptions options = RetainBlankLine);

        // �����ļ�
        STATIC bool readLinesEx(const String &path, std::vector<String> &contents, BlankLineOptions options = RetainBlankLine);
        STATIC bool readLinesEx(const String &path, std::vector<std::vector<String>> &contents, const TCHAR &separator, BlankLineOptions options = RetainBlankLine);

        // д���ļ�
        STATIC bool writeEx(const String &path, const std::vector<String> &contents, const String &separator = _T(""));
        STATIC bool appendEx(const String &path, const std::vector<String> &contents, const String &separator = _T(""));
        STATIC bool appendLineEx(const String &path, const std::vector<String> &contents, const String &separator = _T(""));

        // ��ȫд�ļ�
        STATIC bool safeSave(const String &path, const String &contents, bool append);
        STATIC bool safeSave(const String &path, const std::vector<String> &contents, bool append);
    };
}

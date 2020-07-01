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
        // �жϴ��ڡ��ɶ�����д����ִ��
        STATIC bool isExist(const String &path);
        STATIC bool isReadable(const String &path);
        STATIC bool isWriteable(const String &path);
        STATIC bool isExecutable(const String &path);
        STATIC bool isReadwrite(const String &path);

        // ����
        STATIC long getSelfSizeAttr(const String &path); // �ļ�/Ŀ¼�����С��������Ŀ¼���ļ���С��
        STATIC long getSizeAttr(const String &path, FolderSelfSizeOptions options);
        STATIC String getTimeAttr(const String &path, FileTimeOptions options);
        STATIC bool setHiddenAttr(const String &path, FileHiddenOptions options);
        STATIC bool setReadOnlyAttr(const String &path, FileReadOnlyOptions options);
        STATIC bool setReadOnlyAttr(const String &path, FileReadOnlyOptions options, bool recursionFiles);
        STATIC bool isHidden(const String &path);
        STATIC bool isReadOnly(const String &path);

        // �ж��ļ� or Ŀ¼
        STATIC bool isFile(const String &path);
        STATIC bool isFolder(const String &path);

        // ���� && ɾ��Ŀ¼
        STATIC bool mkdir(const String &path);
        STATIC bool rmdir(const String &path);

        // ɾ���ļ�
        STATIC bool remove(const String &path);

        // ������ or �ƶ�(Ŀ��Ŀ¼�������)
        STATIC bool rename(const String &oldname, const String &newname, bool overwrite);

        // ·��
        STATIC String getAppPath();
        STATIC String getCurWorkDir();
        STATIC String getDirName(const String &path);
        STATIC String getExtName(const String &path);
        STATIC String getFileName(const String &path);
        STATIC String getFileNameWithoutExtension(const String &path);
        STATIC bool hasExtension(const String &path);
    };
}

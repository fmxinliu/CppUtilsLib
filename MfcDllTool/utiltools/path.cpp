#include "StdAfx.h"
#include "path.h"
#include "StringUtils.h"
#include <stdio.h>

#ifdef WIN32
#include <io.h>
#include <direct.h>

#define F_OK  0
#define W_OK  2
#define R_OK  4
#define RW_OK 6
#pragma warning(disable: 4996)

#else

#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef UNICODE
  #define _T(x)      L ## x
  #define _tcscmp wcscmp
#else
  #define _T(x)      x
  #define _tcscmp strcmp
#endif
#endif

using namespace std;
namespace UtilTools
{
#if defined(WIN32)
    bool listFiles(const String &pathname, bool listsubdir, bool listhiddenfiles, vector<String> &result)
    {
        long hFile = 0;
        struct _finddata_t fileInfo;
        string pathName = StringUtils::endsWith(pathname, _T("\\")) ? pathName : pathname + _T("\\");

        if ((hFile = _findfirst((pathName + _T("*")).c_str(), &fileInfo)) == -1) {
            return false;
        }
        // List all the files in the directory with some info about them.
        do {
            if (!_tcscmp(fileInfo.name, _T(".")) || !_tcscmp(fileInfo.name, _T(".."))) {
                continue;
            } else if (fileInfo.attrib & _A_SYSTEM) {
                continue; // 系统文件
            } else if (!listhiddenfiles && fileInfo.attrib & _A_HIDDEN) {
                continue;
            } else if (listsubdir &&  (fileInfo.attrib & _A_SUBDIR)) {
                listFiles(pathName + fileInfo.name, true, listhiddenfiles, result);
            } else {
                result.push_back(pathName + fileInfo.name);
            }
            //cout << fileInfo.name << (fileInfo.attrib&_A_SUBDIR? "[folder]":"[file]") << endl;
        } while (_findnext(hFile, &fileInfo) == 0);
        _findclose(hFile);
        return true;
    }

    bool deleteDirectory(const String &pathname)
    {
        long hFile = 0;
        struct _finddata_t fileInfo;
        String pathName = StringUtils::endsWith(pathname, _T("\\")) ? pathname : pathname + _T("\\");

        if ((hFile = _findfirst((pathName + _T("*")).c_str(), &fileInfo)) == -1) {
            return false;
        }
        // Recursively delete all the file in the directory.
        do {
            if (!strcmp(fileInfo.name, _T(".")) || !strcmp(fileInfo.name, _T(".."))) {
                continue;
            } else if ((fileInfo.attrib & _A_SUBDIR)) {
                deleteDirectory(pathName + fileInfo.name);
            } else {
                remove((pathName + fileInfo.name).c_str());
            }
        } while (_findnext(hFile, &fileInfo) == 0);
        _findclose(hFile);
        return !rmdir(pathname.c_str()); // 删除空目录
    }
#else
    bool deleteDirectory(const String &pathname)
    {
        DIR *dirp;
        if((dirp = opendir(pathname.c_str()) == NULL) {
            return false;
        }

        string dir_full_path = endsWith(pathname, _T("/")) ? pathname : pathname + _T("/");
        struct dirent *dir;
        struct stat st;

        // Recursively delete all the file in the directory.
        while ((dir = readdir(dirp)) != NULL) {
            if (!_tcscmp(dir->d_name, _T(".")) || !_tcscmp(dir->d_name, _T(".."))) {
                continue;
            }

            String sub_path = dir_full_path + dir->d_name;
            if (lstat(sub_path.c_str(), &st) == -1) {
                continue;
            } else if (S_ISDIR(st.st_mode)) {
                deleteDirectory(sub_path); // 如果是目录文件，递归删除
            }
            else if (S_ISREG(st.st_mode)) {
                unlink(sub_path.c_str()); // 如果是普通文件，则 unlink
            }
        }
        closedir(dirp);
        return !rmdir(pathname.c_str()); // 删除空目录
    }
#endif
}
namespace UtilTools
{
    bool Path::isExist(const String &filename)
    {
        return !access(filename.c_str(), F_OK);
    }

    bool Path::isReadable(const String &filename)
    {
        return !access(filename.c_str(), R_OK);
    }

    bool Path::isWriteable(const String &filename)
    {
        return !access(filename.c_str(), W_OK);
    }

    bool Path::isExecutable(const String &filename)
    {
#if defined(WIN32)
        return StringUtils::endsWith(StringUtils::toLower(StringUtils::trimRight(filename)), _T(".exe"));
#else
        return !access(filename.c_str(), X_OK);
#endif
    }

    bool Path::isReadwrite(const String &filename)
    {
        return isReadable(filename) && isWriteable(filename);
    }

    bool Path::isFile(const String &pathname)
    {
#if defined(WIN32)
        return !!chdir(pathname.c_str()); // CreateFile 属性判断
#else
        struct stat st;
        if (lstat(pathname.c_str(), &st) != -1) && (S_ISREG(st.st_mode)); // 普通文件
#endif
    }

    bool Path::isFolder(const String &pathname)
    {
#if defined(WIN32)
        return !chdir(pathname.c_str());
#else
        struct stat st;
        if (lstat(pathname.c_str(), &st) != -1) && (S_ISDIR(st.st_mode));
#endif
    }

    bool Path::mkdir(const String &pathname)
    {
        if (isExist(pathname)) {
            return true;
        }

#if defined(WIN32)
        String cmd = _T("mkdir ") + pathname;
        return !system(cmd.c_str());
#else
        return !mkdir(pathname, RW_OK);
#endif
    }

    bool Path::rmdir(const String &pathname)
    {
        if (!isExist(pathname)) {
            return true;
        }
        if (isFolder(pathname)) {
            return deleteDirectory(pathname);
        }
        return false;
    }

    bool Path::rmove(const String &pathname)
    {
        if (!isExist(pathname)) {
            return true;
        }
        if (isFile(pathname)) {
            return !std::remove(pathname.c_str());
        }
        return false;
    }

    bool Path::rename(const String &oldname, const String &newname, bool overwrite)
    {
        if (overwrite) {
            if (isFile(oldname) && isFile(newname)) {
                rmove(newname);
            } else if (isFolder(oldname) && isFolder(newname)) {
                deleteDirectory(newname);
            }
        }
        return !std::rename(oldname.c_str(), newname.c_str());
    }
}

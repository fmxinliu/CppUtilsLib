#include "StdAfx.h"
#include "FileHelper.h"
#include <fstream>
#include "path.h"

#ifdef UNICODE
#define InputStream  wifstream
#define OutputStream wofstream
#else
#define InputStream  ifstream
#define OutputStream ofstream
#endif

using namespace std;
namespace UtilTools
{
    bool writeFile(const String &path, const String &contents, ios_base::openmode mode)
    {
        // ios::app 文件不存在，则创建；若存在，末尾追加
        // ios::trunc 文件不存在，则创建；若存在，清空文件
        OutputStream of;
        of.open(path, mode);
        if (!of.is_open()) {
            return false;
        }

        of << contents;
        //of.write(value.c_str(), value.length());
        of.close();
        return true;
    }
}

namespace UtilTools
{
    bool FileHelper::create(const String &filename)
    {
        if (Path::isFolder(filename)) {
            return false;
        }
        if (Path::isExist(filename)) {
            return false;
        }
        return writeFile(filename, _T(""), ios::trunc);
    }

    bool FileHelper::exists(const String &filename)
    {
        return Path::isFile(filename) && Path::isExist(filename);
    }

    bool FileHelper::remove(const String &filename)
    {
        return Path::isFile(filename) && Path::remove(filename);
    }

    bool FileHelper::copy(const String &sourceFileName, const String &destFileName, bool overwrite)
    {
        if (!Path::isExist(sourceFileName)) {
            return false;
        }
        if (Path::isExist(destFileName) && !overwrite) {
            return false;
        }

        InputStream in;
        in.open(sourceFileName, ios::in);
        if (!in.is_open()) {
            return false;
        }

        OutputStream out;
        out.open(destFileName, ios::trunc);
        if (!out.is_open()) {
            in.close();
            return false;
        }

        out << in.rdbuf();
        in.close();
        out.flush();
        out.close();
        return true;
    }

    bool FileHelper::move(const String &sourceFileName, const String &destFileName, bool overwrite)
    {
        if (!Path::isFile(sourceFileName)) {
            return false;
        }
        return Path::rename(sourceFileName, destFileName, overwrite);
    }
}

namespace UtilTools
{
    bool FileHelper::write(const String &path, const String &contents)
    {
        return writeFile(path, contents, ios::trunc);
    }

    bool FileHelper::write(const String &path, const vector<String> &contents)
    {
        return write(path, contents, _T(""));
    }

    bool FileHelper::write(const String &path, const vector<String> &contents, const String &separator)
    {
        OutputStream of;
        of.open(path, ios::trunc);
        if (!of.is_open()) {
            return false;
        }

        vector<String>::const_iterator it = contents.cbegin();
        for (; it != contents.cend(); ++it) {
            of << *it << separator;
        }

        of.close();
        return true;
    }

    bool FileHelper::append(const String &path, const String &contents)
    {
        return writeFile(path, contents, ios::app);
    }

    bool FileHelper::append(const String &path, const vector<String> &contents)
    {
        return append(path, contents, _T(""));
    }

    bool FileHelper::append(const String &path, const vector<String> &contents, const String &separator)
    {
        OutputStream of;
        of.open(path, ios::app);
        if (!of.is_open()) {
            return false;
        }

        vector<String>::const_iterator it = contents.cbegin();
        for (; it != contents.cend(); ++it) {
            of << *it << separator;
        }

        of.close();
        return true;
    }

    bool FileHelper::appendLine(const String &path, const String &contents)
    {
        return writeFile(path, _T("\n") + contents, ios::app);
    }

    bool FileHelper::appendLine(const String &path, const vector<String> &contents)
    {
        return appendLine(path, contents, _T(""));
    }

    bool FileHelper::appendLine(const String &path, const vector<String> &contents, const String &separator)
    {
        OutputStream of;
        of.open(path, ios::app);
        if (!of.is_open()) {
            return false;
        }

        vector<String>::const_iterator it = contents.cbegin();
        while (it != contents.cend()) {
            of << *it++;
            if (it != contents.cend()) {
                of << separator;
            } else {
                of << endl;
                break;
            }
        }

        of.close();
        return true;
    }
}

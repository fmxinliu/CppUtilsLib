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
    bool wtite(const String &filename, const String &contents, ios_base::openmode mode)
    {
        OutputStream of;
        of.open(filename, mode);
        if (!of.is_open()) {
            return false;
        }

        of << contents;
        //of.write(value.c_str(), value.length());
        of.close();
        return true;
    }

    bool FileHelper::create(const String &filename)
    {
        if (Path::isFolder(filename)) {
            return false;
        }
        if (Path::isExist(filename)) {
            return false;
        }
        return wtite(filename, _T(""), ios::trunc);
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

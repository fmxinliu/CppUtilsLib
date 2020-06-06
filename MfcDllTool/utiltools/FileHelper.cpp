#include "StdAfx.h"
#include "FileHelper.h"
#include <fstream>
#include <sstream>
#include <streambuf>
#include "path.h"

#ifdef UNICODE
#define IOStream            wiostream
#define InputStream         wifstream
#define OutputStream        wofstream

#define StringStream        wstringstream
#define InputStringStream   wistringstream
#define OutputStringStream  wostringstream

#define StringBuffer        wstreambuf

#else
#define IOStream            iostream
#define InputStream         ifstream
#define OutputStream        ofstream

#define StringStream        stringstream
#define InputStringStream   istringstream
#define OutputStringStream  ostringstream

#define StringBuffer        streambuf
#endif

using namespace std;
namespace UtilTools
{
    bool isBlank(const String &s)
    {
        if (s.empty()) {
            return true;
        }
        size_t length = s.length();
        for(size_t i = 0; i < length; ++i) {
            if (!isspace(s.at(i))) {
                return false;
            }
        }
        return true;
    }

    bool readFile(const String &path, String &contents, ios_base::openmode mode)
    {
        InputStream in(path, mode);
        if (!in) {
            return false;
        }

        // 方法1：
        StringStream ss;
        ss << in.rdbuf();
        contents = ss.str();

        // 方法2：
        //String contents2 = String(istreambuf_iterator<TCHAR>(in), istreambuf_iterator<TCHAR>());

        // 方法3：
        //in.seekg(0, ios::end);
        //streampos sp = in.tellg(); // 文件大小
        //in.seekg(0, ios::beg);
        //TCHAR *buffer = new TCHAR[sp];
        //memset(buffer, 0, sp);
        //streamsize length = in.read(buffer, sp).gcount(); // 读取的字节长度
        //delete[] buffer;

        in.close();
        return true;
    }

    bool writeFile(const String &path, const String &contents, ios_base::openmode mode)
    {
        // ios::app 文件不存在，则创建；若存在，末尾追加
        // ios::trunc 文件不存在，则创建；若存在，清空文件
        OutputStream of(path, mode);
        if (!of) {
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
    INT64 FileHelper::size(const String &filename)
    {
        INT64 fsize = -1;
        if (exists(filename)) {
            fsize = Path::getSizeAttr(filename);
        }
        return fsize;
    }

    INT64 FileHelper::length(const String &filename, LineEndOptions options)
    {
        // fstream 默认以文本方式打开文件, windows 换行符 \r\n 仅能读取到 \n。改用二进制方式打开文件即可
        INT64 len = -1;
        InputStream in(filename, (IgnoreCR == options) ? ios::in : ios::binary);
        if (in) {
            len = 0;
            int ch;
            //while ((ch = in.get()) != EOF) {
            //    len++;
            //}
            istream::sentry se(in, true);
            StringBuffer *sb = in.rdbuf();
            while (sb && (ch = sb->sbumpc()) != EOF) {
                len++;
            }
            in.close();
        }
        return len;
    }

    String FileHelper::createTime(const String &filename)
    {
        String time;
        if (exists(filename)) {
            time = Path::getTimeAttr(filename, CreateTime);
        }
        return time;
    }

    String FileHelper::lastWriteTime(const String &filename)
    {
        String time;
        if (exists(filename)) {
            time = Path::getTimeAttr(filename, LastWriteTime);
        }
        return time;
    }

    String FileHelper::lastAccessTime(const String &filename)
    {
        String time;
        if (exists(filename)) {
            time = Path::getTimeAttr(filename, LastAccessTime);
        }
        return time;
    }

    bool FileHelper::empty(const String &filename)
    {
        return (0 == size(filename));
    }

    bool FileHelper::setHidden(const String &filename)
    {
        return exists(filename) && Path::setHiddenAttr(filename, Hidden);
    }

    bool FileHelper::setUnHidden(const String &filename)
    {
        return exists(filename) && Path::setHiddenAttr(filename, UnHidden);
    }

    bool FileHelper::setReadOnly(const String &filename)
    {
        return exists(filename) && Path::setReadOnlyAttr(filename, ReadOnly);
    }

    bool FileHelper::setWritable(const String &filename)
    {
        return exists(filename) && Path::setReadOnlyAttr(filename, Writable);
    }
}

namespace UtilTools
{
    bool FileHelper::write(const String &path, const String &contents)
    {
        return writeFile(path, contents, ios::trunc);
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

namespace UtilTools
{
    bool FileHelper::read(const String &path, String &contents)
    {
        return readFile(path, contents, ios::in);
    }

    bool FileHelper::readLines(const String &path, vector<String> &contents, bool removeBlankLine)
    {
        InputStream in;
        in.open(path);
        if (!in.is_open()) {
            return false;
        }
        String lineString;
        while (getline(in, lineString)) {
            if (!removeBlankLine || !isBlank(lineString)) {
                contents.push_back(lineString);
            }
        }
        in.close();
        return true;
    }

    bool FileHelper::readLines(const String &path, vector<vector<String>> &contents, const TCHAR &separator, bool removeBlankLine)
    {
        InputStream in;
        in.open(path);
        if (!in.is_open()) {
            return false;
        }
        String lineString;
        while (getline(in, lineString)) {
            if (!removeBlankLine || !isBlank(lineString)) {
                String splitString;
                StringStream ss(lineString);
                vector<String> lineSplitStrings;
                while (getline(ss, splitString, separator)) {
                    lineSplitStrings.push_back(splitString);
                }
                contents.push_back(lineSplitStrings);
            }
        }
        in.close();
        return true;
    }
}

namespace UtilTools
{
/// 一次性读取所有文本
#define READ_ALL_TEXT() \
    String text; \
    if (!read(path, text)) { \
        return false; \
    }

/// 一次性写所有文本到文件
#define WRITE_ALL_TEXT(func) \
    StringStream ss_lines; \
    vector<String>::const_iterator it = contents.cbegin(); \
    while (it != contents.cend()) { \
        ss_lines << *it++; \
        if (it != contents.cend()) { \
            ss_lines << separator; \
        } else { \
            ss_lines << endl; \
            break; \
        } \
    } \
    return func(path, ss_lines.str())

    bool FileHelper::readLinesEx(const String &path, vector<String> &contents, bool removeBlankLine)
    {
        READ_ALL_TEXT();
        String lineString;
        StringStream ss_all_lines(text);
        while (getline(ss_all_lines, lineString, '\n')) {
            if (!removeBlankLine || !isBlank(lineString)) {
                contents.push_back(lineString);
            }
        }
#pragma region 逐个字符遍历解析
        //size_t startpos = 0;
        //for (size_t i = 0; i < text.length(); i++) {
        //    if ('\n' == text[i]) {
        //        String &lineString = text.substr(startpos, i - startpos);
        //        if (!removeBlankLine || !isBlank(lineString)) {
        //            contents.push_back(lineString);
        //        }
        //        startpos = i + 1;
        //    }
        //}
        //if (startpos < text.length()) {
        //    String &lineString = text.substr(startpos);
        //    if (!removeBlankLine || !isBlank(lineString)) {
        //        contents.push_back(lineString);
        //    }
        //}
#pragma endregion
        return true;
    }

    bool FileHelper::readLinesEx(const String &path, vector<vector<String>> &contents, const TCHAR &separator, bool removeBlankLine)
    {
        READ_ALL_TEXT();
        String lineString;
        StringStream ss_all_lines(text);
        while (getline(ss_all_lines, lineString, '\n')) {
            if (!removeBlankLine || !isBlank(lineString)) {
                String splitString;
                StringStream ss_one_line(lineString);
                vector<String> lineSplitStrings;
                while (getline(ss_one_line, splitString, separator)) {
                    lineSplitStrings.push_back(splitString);
                }
                contents.push_back(lineSplitStrings);
            }
        }
        return true;
    }

    bool FileHelper::writeEx(const String &path, const vector<String> &contents, const String &separator)
    {
        WRITE_ALL_TEXT(write);
    }

    bool FileHelper::appendEx(const String &path, const vector<String> &contents, const String &separator)
    {
        WRITE_ALL_TEXT(append);
    }

    bool FileHelper::appendLineEx(const String &path, const vector<String> &contents, const String &separator)
    {
        WRITE_ALL_TEXT(appendLine);
    }
}

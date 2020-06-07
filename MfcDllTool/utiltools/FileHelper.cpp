#include "StdAfx.h"
#include "FileHelper.h"
#include <fstream>
#include <sstream>
#include <streambuf>
#include "path.h"
#include "StringUtils.h"

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

        // ����1��
        StringStream ss;
        ss << in.rdbuf();
        contents = ss.str();

        // ����2��
        //String contents2 = String(istreambuf_iterator<TCHAR>(in), istreambuf_iterator<TCHAR>());

        // ����3��
        //in.seekg(0, ios::end);
        //streampos sp = in.tellg(); // �ļ���С
        //in.seekg(0, ios::beg);
        //TCHAR *buffer = new TCHAR[sp];
        //memset(buffer, 0, sp);
        //streamsize length = in.read(buffer, sp).gcount(); // ��ȡ���ֽڳ���
        //delete[] buffer;

        in.close();
        return true;
    }

    bool writeFile(const String &path, const String &contents, ios_base::openmode mode)
    {
        // ios::app �ļ������ڣ��򴴽��������ڣ�ĩβ׷��
        // ios::trunc �ļ������ڣ��򴴽��������ڣ�����ļ�
        OutputStream of(path, mode);
        if (!of) {
            return false;
        }

        of << contents;
        //of.write(value.c_str(), value.length());
        of.close();
        return true;
    }

    INT64 lineCountFast(const String &filename)
    {
#ifdef UNICODE
        WS2S_PTR(filename, fname);
#else
        const char * fname = filename.c_str();
#endif
#ifdef WIN32
        FILE * fp = fopen(fname, "rb");
        if (NULL == fp) {
            return -1;
        }
#else
        int fd = open(file, O_RDONLY | O_BINARY);
        if (-1 == fd) {
            return -1;
        }
#endif
        char buf[BUFSIZ];
        int len = 0;
        INT64 c = 0;
#ifdef WIN32
        while ((len = fread(buf, 1, BUFSIZ, fp)) != 0) {
#else
        while ((len = read(fd, buf, BUFSIZ)) != 0) {
#endif
            char * p  = buf;
            char * pe = buf + len;
            while ((p = (char*)memchr((void*)p, '\n', pe - p)) != NULL) {
                ++p;
                ++c;
            }

            if (p < pe) {
                ++c; // �������һ��
            }
        }
#ifdef WIN32
        fclose(fp);
#else
        close(fd);
#endif
        return c;
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

    bool FileHelper::copy(const String &sourceFileName, const String &destFileName, OverWriteOptions options)
    {
        if (!Path::isExist(sourceFileName)) {
            return false;
        }
        if (Path::isExist(destFileName) && (OverWriteIfExist != options)) {
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

    bool FileHelper::move(const String &sourceFileName, const String &destFileName, OverWriteOptions options)
    {
        if (!Path::isFile(sourceFileName)) {
            return false;
        }
        return Path::rename(sourceFileName, destFileName, (OverWriteIfExist == options));
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
        // fstream Ĭ�����ı���ʽ���ļ�, windows ���з� \r\n ���ܶ�ȡ�� \n�����ö����Ʒ�ʽ���ļ�����
        INT64 len = -1;
        InputStream in(filename, (IgnoreCR == options) ? ios::in : ios::binary);
        if (in) {
            len = 0;
            TCHAR ch;
            //while ((ch = in.get()) != EOF) {
            //    len++;
            //}
            InputStream::sentry se(in, true);
            StringBuffer *sb = in.rdbuf();
            while (sb && (ch = sb->sbumpc()) != (TCHAR)EOF) {
                len++;
            }
            in.close();
        }
        return len;
    }

    INT64 FileHelper::lineCount(const String &filename, LineCountOptions options)
    {
        if (UnCountBlankLine != options) {
            return lineCountFast(filename);
        }
        vector<String> contents;
        readLinesEx(filename, contents, RemoveBlankLine);
        return contents.size();
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

    bool FileHelper::readLines(const String &path, vector<String> &contents, BlankLineOptions options)
    {
        InputStream in;
        in.open(path);
        if (!in.is_open()) {
            return false;
        }
        String lineString;
        bool removeBlankLine = (RemoveBlankLine == options);
        while (getline(in, lineString)) {
            if (!removeBlankLine || !isBlank(lineString)) {
                contents.push_back(lineString);
            }
        }
        in.close();
        return true;
    }

    bool FileHelper::readLines(const String &path, vector<vector<String>> &contents, const TCHAR &separator, BlankLineOptions options)
    {
        InputStream in;
        in.open(path);
        if (!in.is_open()) {
            return false;
        }
        String lineString;
        bool removeBlankLine = (RemoveBlankLine == options);
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
/// һ���Զ�ȡ�����ı�
#define READ_ALL_TEXT() \
    String text; \
    if (!read(path, text)) { \
        return false; \
    }

/// һ����д�����ı����ļ�
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

    bool FileHelper::readLinesEx(const String &path, vector<String> &contents, BlankLineOptions options)
    {
        READ_ALL_TEXT();
        String lineString;
        StringStream ss_all_lines(text);
        bool removeBlankLine = (RemoveBlankLine == options);
        while (getline(ss_all_lines, lineString, TCHAR('\n'))) {
            if (!removeBlankLine || !isBlank(lineString)) {
                contents.push_back(lineString);
            }
        }
#pragma region ����ַ���������
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

    bool FileHelper::readLinesEx(const String &path, vector<vector<String>> &contents, const TCHAR &separator, BlankLineOptions options)
    {
        READ_ALL_TEXT();
        String lineString;
        StringStream ss_all_lines(text);
        bool removeBlankLine = (RemoveBlankLine == options);
        while (getline(ss_all_lines, lineString, TCHAR('\n'))) {
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

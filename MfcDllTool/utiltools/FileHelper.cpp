#include "StdAfx.h"
#include "FileHelper.h"
#include <fstream>
#include <sstream>
#include <streambuf>
#include "path.h"

#ifdef UNICODE
#define InputStream     wifstream
#define OutputStream    wofstream
#define StreamBuffer    wstringstream
#else
#define InputStream     ifstream
#define OutputStream    ofstream
#define StreamBuffer    stringstream
#endif

#if !defined(_WIN32)
#ifdef UNICODE
#define TCHAR           wchar_t
#else
#define TCHAR           char
#endif
#endif

using namespace std;
namespace UtilTools
{
    bool readFile(const String &path, String &contents, ios_base::openmode mode)
    {
        InputStream in(path, mode);
        if (!in) {
            return false;
        }

        // ����1��
        StreamBuffer buffer;
        buffer << in.rdbuf();
        contents = buffer.str();

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

namespace UtilTools
{
    bool FileHelper::read(const String &path, String &contents)
    {
        return readFile(path, contents, ios::in);
    }

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

    bool FileHelper::read(const String &path, vector<String> &contents, bool removeBlankLine)
    {
        String content;
        if (!read(path, content)) {
            return false;
        }
        size_t startpos = 0;
        for (size_t i = 0; i < content.length(); i++) {
            if ('\n' == content[i]) {
                String &lineString = content.substr(startpos, i - startpos);
                if (!removeBlankLine || !isBlank(lineString)) {
                    contents.push_back(lineString);
                }
                startpos = i + 1;
            }
        }
        if (startpos < content.length()) {
            String &lineString = content.substr(startpos);
            if (!removeBlankLine || !isBlank(lineString)) {
                contents.push_back(lineString);
            }
        }
        return true;
    }

    bool FileHelper::readLines(const String &path, vector<String> &contents)
    {
        return readLines(path, contents, false);
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
}

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

#define StreamBuffer        wstreambuf
#define StringBuffer        wstringbuf
#define FileBuffer          wfilebuf

#else
#define IOStream            iostream
#define InputStream         ifstream
#define OutputStream        ofstream

#define StringStream        stringstream
#define InputStringStream   istringstream
#define OutputStringStream  ostringstream

#define StreamBuffer        streambuf
#define StringBuffer        stringbuf
#define FileBuffer          filebuf
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

        in.imbue(locale(""));

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

        of.imbue(locale(""));
        of << contents;
        //of.write(contents.c_str(), contents.length());
        of.close();
        return true;
    }

    size_t lineCountFast(const String &filename)
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
        size_t c = 0;
#ifdef WIN32
        while ((len = fread(buf, 1, BUFSIZ, fp)) != 0) {
#else
        while ((len = read(fd, buf, BUFSIZ)) != 0) {
#endif
            char * p  = buf;
            char * pe = buf + len;
            char * pr = buf; // ��¼ÿ����ʼλ��
            while ((p = (char*)memchr((void*)p, '\n', pe - p)) != NULL) {
                ++p;
                ++c;
                pr = p;
            }

            if (pr != pe) {
                ++c; // �������һ�У��ǿգ����ٺ���һ���ַ�
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

    bool FileHelper::copy(const String &sourceFileName, const String &destFileName, BOOL skipIfExist)
    {
        if (!Path::isExist(sourceFileName)) {
            return false;
        }
        if (Path::isExist(destFileName) && !skipIfExist) {
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
        in.imbue(locale(""));
        out.imbue(locale(""));
        out << in.rdbuf();
        in.close();
        out.flush();
        out.close();
        return true;
    }

    bool FileHelper::move(const String &sourceFileName, const String &destFileName, BOOL skipIfExist)
    {
        if (!Path::isFile(sourceFileName)) {
            return false;
        }
        return Path::rename(sourceFileName, destFileName, !skipIfExist);
    }
}

namespace UtilTools
{
    long FileHelper::size(const String &filename)
    {
        long fsize = -1;
        if (exists(filename)) {
            fsize = Path::getSelfSizeAttr(filename);
        }
        return fsize;
    }

    long FileHelper::length(const String &filename, BOOL ignoreLineEndCR)
    {
        // fstream Ĭ�����ı���ʽ���ļ�, windows ���з� \r\n ���ܶ�ȡ�� \n�����ö����Ʒ�ʽ���ļ�����
        long len = -1;
        InputStream in(filename, ignoreLineEndCR ? ios::in : ios::binary);
        if (in) {
            len = 0;
            TCHAR ch;
            //while ((ch = in.get()) != EOF) {
            //    len++;
            //}
            in.imbue(locale(""));
            InputStream::sentry se(in, true);
            StreamBuffer *sb = in.rdbuf();
            while (sb && (ch = sb->sbumpc()) != (TCHAR)EOF) {
                len++;
            }
            in.close();
        }
        return len;
    }

    long FileHelper::lineCount(const String &filename, BOOL countBlankLine)
    {
        if (countBlankLine) {
            return lineCountFast(filename);
        }
        vector<String> contents;
        read(filename, contents, RemoveBlankLine);
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
    bool FileHelper::write(const String &path, const String &contents, BOOL append)
    {
        return UtilTools::writeFile(path, contents, !!append ? ios::app : ios::trunc);
    }

    bool FileHelper::write(const String &path, const std::vector<String> &contents, const String &separator, BOOL append)
    {
        OutputStream of;
        of.open(path, !!append ? ios::app : ios::trunc);
        if (!of.is_open()) {
            return false;
        }

        of.imbue(locale(""));
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
        of.flush();
        of.close();
        return true;
    }

    // д�ֽ�
    bool FileHelper::write(const String &path, const unsigned char *contents, size_t length, BOOL append)
    {
#ifdef UNICODE
        string s = StringUtils::wstringToString(path);
        const char *fname = s.c_str();
#else
        const char *fname = path.c_str();
#endif
        FILE *fp = fopen(fname, append ? "a": "w");
        if (!fp) {
            return false;
        }
        size_t len = fwrite(contents, 1, length, fp);
        fclose(fp);
        return len == length;
    }
}

namespace UtilTools
{
    bool FileHelper::read(const String &path, String &contents)
    {
        return UtilTools::readFile(path, contents, ios::in);
    }

    bool FileHelper::read(const String &path, vector<String> &contents, BOOL removeBlankLine)
    {
        InputStream in;
        in.open(path);
        if (!in.is_open()) {
            return false;
        }
        in.imbue(locale(""));
        String lineString;
        while (getline(in, lineString)) {
            if (!removeBlankLine || !isBlank(lineString)) {
                contents.push_back(lineString);
            }
        }
        in.close();
        return true;
    }

    bool FileHelper::read(const String &path, vector<vector<String>> &contents, const TCHAR &separator, BOOL removeBlankLine)
    {
        InputStream in;
        in.open(path);
        if (!in.is_open()) {
            return false;
        }
        in.imbue(locale(""));
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
    // �ļ�����ȫ�����浽���ݲ���Ч��һ������ xml �����ļ�
    bool xmlSafeSave(const String &path, const String &contents, bool app)
    {
        String bakpath = path + _T(".bak");

        // д��ʱ�ļ�
        bool savesuccess = false;
        savesuccess = FileHelper::write(bakpath, contents, app);
        if (!savesuccess) {
            return false;
        }

        // ��ʱ�ļ� -> Ŀ���ļ�
        savesuccess = FileHelper::move(bakpath, path, OverWriteIfExist);
        if (!savesuccess) {
            return false;
        }

        // ɾ����ʱ�ļ�
        FileHelper::remove(bakpath);
        return true;
    }

    bool FileHelper::safeSave(const String &path, const String &contents, BOOL app)
    {
        bool savesuccess = false;
        String bakpath = path + _T(".bak");
        if (FileHelper::exists(bakpath)) { // ���ڱ����ļ���˵��Ŀ���ļ����һ��д��ʧ�ܣ������ļ����������µģ�ֱ��д�����ļ�
            savesuccess = FileHelper::write(bakpath, contents, app);
            if (savesuccess) { // �����ļ�д�ɹ����������ļ�����ΪĿ���ļ�
                savesuccess = FileHelper::move(bakpath, path, OverWriteIfExist);
                if (savesuccess) { // Ŀ���ļ�д�ɹ���ɾ�������ļ�
                    FileHelper::remove(bakpath);
                }
            }
        } else { // �����ڱ����ļ���˵��Ŀ���ļ����һ��д��ɹ���ֱ��дĿ���ļ�
            savesuccess = FileHelper::write(path, contents, app);
            if (!savesuccess) { // Ŀ���ļ�дʧ�ܣ��类�򿪲鿴�����ȱ���Ŀ���ļ�
                savesuccess = FileHelper::copy(path, bakpath, OverWriteIfExist);
                if (savesuccess) { // ���ݳɹ���������д�뱸���ļ�
                    savesuccess = FileHelper::write(bakpath, contents, app);
                }
            }
        }
        return savesuccess;
    }

    bool FileHelper::safeSave(const String &path, const std::vector<String> &contents, BOOL app)
    {
        bool savesuccess = false;
        String bakpath = path + _T(".bak");
        if (FileHelper::exists(bakpath)) { // ���ڱ����ļ���˵��Ŀ���ļ����һ��д��ʧ�ܣ������ļ����������µģ�ֱ��д�����ļ�
            savesuccess = FileHelper::write(bakpath, contents, _T(""), app);
            if (savesuccess) { // �����ļ�д�ɹ����������ļ�����ΪĿ���ļ�
                savesuccess = FileHelper::move(bakpath, path, OverWriteIfExist);
                if (savesuccess) { // Ŀ���ļ�д�ɹ���ɾ�������ļ�
                    FileHelper::remove(bakpath);
                }
            }
        } else { // �����ڱ����ļ���˵��Ŀ���ļ����һ��д��ɹ���ֱ��дĿ���ļ�
            savesuccess = FileHelper::write(path, contents, _T(""), app);
            if (!savesuccess) { // Ŀ���ļ�дʧ�ܣ��类�򿪲鿴�����ȱ���Ŀ���ļ�
                savesuccess = FileHelper::copy(path, bakpath, OverWriteIfExist);
                if (savesuccess) { // ���ݳɹ���������д�뱸���ļ�
                    savesuccess = FileHelper::write(bakpath, contents, _T(""), app);
                }
            }
        }
        return savesuccess;
    }
}

#include "StdAfx.h"
#include "IniHelper.h"
#include "StringUtils.h"
#include <assert.h>
#include <sstream>     // std::stringstream
#include <algorithm>
#define MAX_LENGTH 2049

namespace UtilTools
{
    bool IniHelper::read(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, int defaultValue, int &value)
    {
        TCHAR mDataStr[MAX_LENGTH];
        DWORD strLength = ::GetPrivateProfileString(lpSection, lpKey, NULL, mDataStr, sizeof(mDataStr)-1, lpIniFileName);
        bool convsuccess = StringUtils::isNumeric(mDataStr);
        value = convsuccess ? _ttoi(mDataStr) : defaultValue;
        return convsuccess;
    }

    bool IniHelper::read(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, double defaultValue, double &value)
    {
        TCHAR mDataStr[MAX_LENGTH];
        DWORD strLength = ::GetPrivateProfileString(lpSection, lpKey, NULL, mDataStr, sizeof(mDataStr)-1, lpIniFileName);
        bool convsuccess = StringUtils::isNumeric(mDataStr);
        value = convsuccess ? _ttof(mDataStr) : defaultValue;
        return convsuccess;
    }

    int IniHelper::read(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, int defaultValue, bool *pRet)
    {
        TCHAR mDataStr[MAX_LENGTH];
        DWORD strLength = ::GetPrivateProfileString(lpSection, lpKey, NULL, mDataStr, sizeof(mDataStr)-1, lpIniFileName);
        bool convsuccess = StringUtils::isNumeric(mDataStr);
        int value = convsuccess ? _ttoi(mDataStr) : defaultValue;
        if (pRet && convsuccess) {
            *pRet = convsuccess;
        }
        return value;
    }

    double IniHelper::read(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, double defaultValue, bool *pRet)
    {
        TCHAR mDataStr[MAX_LENGTH];
        DWORD strLength = ::GetPrivateProfileString(lpSection, lpKey, NULL, mDataStr, sizeof(mDataStr)-1, lpIniFileName);
        bool convsuccess = StringUtils::isNumeric(mDataStr);
        double value = convsuccess ? _ttoi(mDataStr) : defaultValue;
        if (pRet && convsuccess) {
            *pRet = convsuccess;
        }
        return value;
    }
}

namespace UtilTools
{
    int IniHelper::readInt(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey)
    {
        TCHAR mDataStr[MAX_LENGTH];
        DWORD strLength = ::GetPrivateProfileString(lpSection, lpKey, NULL, mDataStr, sizeof(mDataStr)-1, lpIniFileName);
        int cfgOfInt = _ttoi(mDataStr);
        return cfgOfInt;
    }

    double IniHelper::readDouble(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey)
    {
        TCHAR mDataStr[MAX_LENGTH];
        DWORD strLength = ::GetPrivateProfileString(lpSection, lpKey, NULL, mDataStr, sizeof(mDataStr)-1, lpIniFileName);
        double cfgOfInt = _ttof(mDataStr);
        return cfgOfInt;
    }

    String IniHelper::readString(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey)
    {
        TCHAR mDataStr[MAX_LENGTH];
        DWORD strLength = ::GetPrivateProfileString(lpSection, lpKey, NULL, mDataStr, sizeof(mDataStr)-1, lpIniFileName);
        return mDataStr;
    }
}

namespace UtilTools
{
    template<class T>
    String toString(T value)
    {
#ifdef UNICODE
        std::wstringstream ss;
#else
        std::stringstream ss;
#endif
        ss << value;
        return ss.str();
    }

    bool IniHelper::write(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, const int &value)
    {
        String s = toString(value);
        return !!::WritePrivateProfileString(lpSection, lpKey, s.c_str(), lpIniFileName);
    }

    bool IniHelper::write(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, const double &value)
    {
        String s = toString(value);
        return !!::WritePrivateProfileString(lpSection, lpKey, s.c_str(), lpIniFileName);
    }

    bool IniHelper::write(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, const String &value)
    {
        return !!::WritePrivateProfileString(lpSection, lpKey, value.c_str(), lpIniFileName);
    }

    bool IniHelper::read(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, LPVOID lpStruct, UINT uSizeStruct)
    {
        return !!::GetPrivateProfileStruct(lpSection, lpKey, lpStruct, uSizeStruct, lpIniFileName);
    }

    bool IniHelper::write(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, LPVOID lpStruct, UINT uSizeStruct)
    {
        return !!::WritePrivateProfileStruct(lpSection, lpKey, lpStruct, uSizeStruct, lpIniFileName);
    }
}

namespace UtilTools
{
    using namespace std;
    vector<String> IniHelper::readSectionNames(LPCTSTR lpIniFileName)
    {
        vector<String> sectionNames;
        TCHAR mDataStr[MAX_LENGTH];
        DWORD length = ::GetPrivateProfileSectionNames(mDataStr, sizeof(mDataStr)-1, lpIniFileName);
        DWORD startpos = 0;
        for (DWORD i = 0; i < length; i++) {
            if ('\0' == mDataStr[i]) {
                sectionNames.push_back(String(mDataStr + startpos));
                startpos = i + 1;
            }
        }
        return sectionNames;
    }

    vector<String> IniHelper::readKeys(LPCTSTR lpIniFileName, LPCTSTR lpSection)
    {
        vector<String> keys;
        TCHAR mDataStr[MAX_LENGTH];
        DWORD length = ::GetPrivateProfileSection(lpSection, mDataStr, sizeof(mDataStr)-1, lpIniFileName);
        DWORD startpos = 0;
        for (DWORD i = 0; i < length; i++) {
            if ('\0' == mDataStr[i]) {
                String feild = mDataStr + startpos;
                size_t endpos = feild.find_first_of('=');
                if (string::npos != endpos) {
                    keys.push_back(feild.substr(0, endpos));
                }

                startpos = i + 1;
            }
        }
        return keys;
    }

    vector<String> readSection(LPCTSTR lpIniFileName, LPCTSTR lpSection)
    {
        vector<String> fields;
        TCHAR mDataStr[MAX_LENGTH];
        DWORD length = ::GetPrivateProfileSection(lpSection, mDataStr, sizeof(mDataStr)-1, lpIniFileName);
        DWORD startpos = 0;
        for (DWORD i = 0; i < length; i++) {
            if ('\0' == mDataStr[i]) {
                fields.push_back(String(mDataStr + startpos));
                startpos = i + 1;
            }
        }
        return fields;
    }

    map<String, String> IniHelper::readSection(LPCTSTR lpIniFileName, LPCTSTR lpSection)
    {
        map<String, String> feilds;
        TCHAR mDataStr[MAX_LENGTH];
        DWORD length = ::GetPrivateProfileSection(lpSection, mDataStr, sizeof(mDataStr)-1, lpIniFileName);
        DWORD startpos = 0;
        for (DWORD i = 0; i < length; i++) {
            if ('\0' == mDataStr[i]) {
                String feild = mDataStr + startpos;
                DWORD endpos = feild.find_first_of('=');
                if (string::npos != endpos) {
                    String key = feild.substr(0, endpos);
                    String value = feild.substr(endpos + 1);
                    if (0 == feilds.count(key)) {
                        feilds.insert(make_pair(key, value));
                    } else {
                        assert(false); // Key �Դ���
                    }
                }

                startpos = i + 1;
            }
        }

        return feilds;
    }

    map<String, map<String, String>> IniHelper::readSections(LPCTSTR lpIniFileName)
    {
        map<String, map<String, String>> sections;
        vector<String> sectionNames = readSectionNames(lpIniFileName);
        for (vector<String>::const_iterator it = sectionNames.cbegin(); it != sectionNames.cend(); ++it) {
            sections.insert(make_pair(*it, readSection(lpIniFileName, it->c_str())));
        }
        return sections;
    }

    bool IniHelper::sectionExist(LPCTSTR lpIniFileName, LPCTSTR lpSection)
    {
        vector<String> sections = readSectionNames(lpIniFileName);
        vector<String>::const_iterator it = find(sections.begin(), sections.end(), lpSection);
        if (sections.end() != it) {
            return true;
        }
        return false;
    }

    bool IniHelper::removeSection(LPCTSTR lpIniFileName, LPCTSTR lpSection)
    {
        return !!::WritePrivateProfileSection(lpSection, NULL, lpIniFileName);
    }

    bool IniHelper::keyExist(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey)
    {
        vector<String> keys = readKeys(lpIniFileName, lpSection);
        vector<String>::const_iterator it = find(keys.begin(), keys.end(), lpKey);
        if (keys.end() != it) {
            return true;
        }
        return false;
    }

    bool IniHelper::removeKey(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey)
    {
        return !!::WritePrivateProfileString(lpSection, lpKey, NULL, lpIniFileName);
    }

    bool IniHelper::write(LPCTSTR lpIniFileName, LPCTSTR lpSection, const map<String, String> &value)
    {
        bool ok = true;
        map<String, String>::const_iterator it = value.cbegin();
        for (; it != value.cend(); ++it) {
            ok = ok && write(lpIniFileName, lpSection, it->first.c_str(), it->second);
        }
        return ok;
    }

    bool IniHelper::write(LPCTSTR lpIniFileName, const map<String, map<String, String>> &value)
    {
        bool ok = true;
        map<String, map<String, String>>::const_iterator it = value.cbegin();
        for (; it != value.cend(); ++it) {
            ok = ok && write(lpIniFileName, it->first.c_str(), it->second);
        }
        return ok;
    }
}

// https://www.cnblogs.com/my-blog/archive/2008/11/14/1333470.html
namespace TestIniHelperApi
{
    /// ��ȡ�޷�������
    UINT readInt(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, int defaultValue)
    {
        return ::GetPrivateProfileInt(lpSection, lpKey, defaultValue, lpIniFileName);
    }

    /// ��ȡ�ַ���
    String readString(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, LPCTSTR defaultValue)
    {
        TCHAR mDataStr[MAX_LENGTH];
        DWORD strLength = ::GetPrivateProfileString(lpSection, lpKey, defaultValue, mDataStr, sizeof(mDataStr)-1, lpIniFileName);
        return mDataStr;
    }

// https://www.cnblogs.com/yinheyi/p/5971614.html
#pragma region �ַ���ƴ�Ӻ�
#define _STR(s) _T(#s)
#define STR(s) _STR(s)
#define KEY(type, index) type ## _ ## index
#define KEY_STR(type, index) STR(KEY(type, index))
#define KEY_INT(index) KEY_STR(int, index)
#define KEY_STRING(index) KEY_STR(string, index)
#pragma endregion

    void testReadInt()
    {
        LPCTSTR lpIniFileName = _T("d:\\IniHelper.ini"); // Ini �ļ������ڣ����Զ�����
        LPCTSTR lpSection = _T("section_int"); // Section �����ڣ����Զ�����
        int defaultValue = -999;

        int value[15] = {};
        value[1]  = readInt(lpIniFileName, lpSection, KEY_INT(1), defaultValue); // = �����пո�
        value[2]  = readInt(lpIniFileName, lpSection, KEY_INT(2), defaultValue); // = �����޿ո�
        value[3]  = readInt(lpIniFileName, lpSection, KEY_INT(3), defaultValue); // ֵǰ�пո�
        value[4]  = readInt(lpIniFileName, lpSection, KEY_INT(4), defaultValue); // ֵ���пո�

        value[5]  = readInt(lpIniFileName, lpSection, KEY_INT(5), defaultValue); // ֵ����С����
        value[6]  = readInt(lpIniFileName, lpSection, KEY_INT(6), defaultValue); // ֵǰ��С����
        value[7]  = readInt(lpIniFileName, lpSection, KEY_INT(7), defaultValue); // ֵǰ�� 0.

        value[8]  = readInt(lpIniFileName, lpSection, KEY_INT(8),  defaultValue); // ֵǰ�� +
        value[9]  = readInt(lpIniFileName, lpSection, KEY_INT(9),  defaultValue); // ֵǰ�� -
        value[10] = readInt(lpIniFileName, lpSection, KEY_INT(10), defaultValue); // ֵǰ�� 0

        value[11] = readInt(lpIniFileName, lpSection, KEY_INT(11), defaultValue); // ֵǰ�������ַ�
        value[12] = readInt(lpIniFileName, lpSection, KEY_INT(12), defaultValue); // ֵ���������ַ�

        value[13] = readInt(lpIniFileName, lpSection, KEY_INT(13), defaultValue); // ���ظ�Key
        value[14] = readInt(lpIniFileName, lpSection, KEY_INT(14), defaultValue); // Key��Сд�Ƿ�����
        value[0]  = readInt(lpIniFileName, lpSection, KEY_INT(0),  defaultValue); // Key�����ڣ�����Ĭ��ֵ
    }

    void testReadString()
    {
        LPCTSTR lpIniFileName = _T("d:\\IniHelper.ini"); // Ini �ļ������ڣ����Զ�����
        LPCTSTR lpSection = _T("section_string"); // Section �����ڣ����Զ�����
        LPCTSTR lpDefault = _T("ERROR");

        String s0 = readString(lpIniFileName, lpSection, KEY_STRING(0), lpDefault); // Key������

        String s1 = readString(lpIniFileName, lpSection, KEY_STRING(1), lpDefault); // ֵǰ�пո�
        String s2 = readString(lpIniFileName, lpSection, KEY_STRING(2), lpDefault); // ֵ���пո�
        String s3 = readString(lpIniFileName, lpSection, KEY_STRING(3), lpDefault); // ֵ�м��пո�

        String s4 = readString(lpIniFileName, lpSection, KEY_STRING(4), lpDefault); // ת���ַ�1 - ֵǰ\���пո�
        String s5 = readString(lpIniFileName, lpSection, KEY_STRING(5), lpDefault); // ת���ַ�2 - ֵ��\���пո񣬽�β�ո񱻺���

        String s6 = readString(lpIniFileName, lpSection, KEY_STRING(6), lpDefault); // ˫����1 - ֵǰ�пո�
        String s7 = readString(lpIniFileName, lpSection, KEY_STRING(7), lpDefault); // ˫����2 - ֵ���пո�
        String s8 = readString(lpIniFileName, lpSection, KEY_STRING(8), lpDefault); // ˫����3 - ֵ�м��пո�
        String s9 = readString(lpIniFileName, lpSection, KEY_STRING(9), lpDefault); // ˫����4 - ֵ�м���˫����
    }

    void testWriteSection()
    {
        // ɾ������ keys����д��ָ���� Keys
        LPCTSTR lpIniFileName = _T("d:\\IniHelper.ini"); // Ini �ļ������ڣ����Զ�����
        LPCTSTR lpSection = _T("section_other"); // Section �����ڣ����Զ�����
        LPCTSTR lpValue = _T("other_1=123\0other_2=45.6"); // Key �����ڣ����Զ����������ڣ��򸲸�
        bool b = !!::WritePrivateProfileSection(lpSection, lpValue, lpIniFileName);
    }
}

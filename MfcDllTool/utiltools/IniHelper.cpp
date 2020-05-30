#include "StdAfx.h"
#include "IniHelper.h"
#include <assert.h>
#include <sstream>     // std::stringstream
#include <algorithm>

#ifndef USE_STRINGUTILS
#include "StringUtils.h"
namespace UtilTools
{
    bool IniHelper::read(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, int defaultValue, int &value)
    {
        TCHAR mDataStr[MAX_PATH];
        DWORD strLength = ::GetPrivateProfileString(lpSection, lpKey, NULL, mDataStr, sizeof(mDataStr)-1, lpIniFileName);
        bool convsuccess = StringUtils::isNumeric(mDataStr);
        value = convsuccess ? _ttoi(mDataStr) : defaultValue;
        return convsuccess;
    }

    bool IniHelper::read(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, double defaultValue, double &value)
    {
        TCHAR mDataStr[MAX_PATH];
        DWORD strLength = ::GetPrivateProfileString(lpSection, lpKey, NULL, mDataStr, sizeof(mDataStr)-1, lpIniFileName);
        bool convsuccess = StringUtils::isNumeric(mDataStr);
        value = convsuccess ? _ttof(mDataStr) : defaultValue;
        return convsuccess;
    }

    int IniHelper::read(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, int defaultValue, bool *pRet)
    {
        TCHAR mDataStr[MAX_PATH];
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
        TCHAR mDataStr[MAX_PATH];
        DWORD strLength = ::GetPrivateProfileString(lpSection, lpKey, NULL, mDataStr, sizeof(mDataStr)-1, lpIniFileName);
        bool convsuccess = StringUtils::isNumeric(mDataStr);
        double value = convsuccess ? _ttoi(mDataStr) : defaultValue;
        if (pRet && convsuccess) {
            *pRet = convsuccess;
        }
        return value;
    }
}
#endif

namespace UtilTools
{
    /// 读取无符号整型
    UINT readInt(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, int defaultValue)
    {
        return ::GetPrivateProfileInt(lpSection, lpKey, defaultValue, lpIniFileName);
    }

    /// 读取字符串
    String readString(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey, LPTSTR defaultValue)
    {
        TCHAR mDataStr[MAX_PATH];
        DWORD strLength = ::GetPrivateProfileString(lpSection, lpKey, defaultValue, mDataStr, sizeof(mDataStr)-1, lpIniFileName);
        return mDataStr;
    }
}

namespace UtilTools
{
    int IniHelper::readInt(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey)
    {
        TCHAR mDataStr[MAX_PATH];
        DWORD strLength = ::GetPrivateProfileString(lpSection, lpKey, NULL, mDataStr, sizeof(mDataStr)-1, lpIniFileName);
        int cfgOfInt = _ttoi(mDataStr);
        return cfgOfInt;
    }

    double IniHelper::readDouble(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey)
    {
        TCHAR mDataStr[MAX_PATH];
        DWORD strLength = ::GetPrivateProfileString(lpSection, lpKey, NULL, mDataStr, sizeof(mDataStr)-1, lpIniFileName);
        double cfgOfInt = _ttof(mDataStr);
        return cfgOfInt;
    }

    String IniHelper::readString(LPCTSTR lpIniFileName, LPCTSTR lpSection, LPCTSTR lpKey)
    {
        TCHAR mDataStr[MAX_PATH];
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
        TCHAR mDataStr[2049];
        DWORD length = ::GetPrivateProfileSectionNames(mDataStr, sizeof(mDataStr)-1, lpIniFileName);
        size_t startpos = 0;
        for (int i = 0; i < length; i++) {
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
        TCHAR mDataStr[2049];
        DWORD length = ::GetPrivateProfileSection(lpSection, mDataStr, sizeof(mDataStr)-1, lpIniFileName);
        size_t startpos = 0;
        for (int i = 0; i < length; i++) {
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
        TCHAR mDataStr[2049];
        DWORD length = ::GetPrivateProfileSection(lpSection, mDataStr, sizeof(mDataStr)-1, lpIniFileName);
        size_t startpos = 0;
        for (int i = 0; i < length; i++) {
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
        TCHAR mDataStr[2049];
        DWORD length = ::GetPrivateProfileSection(lpSection, mDataStr, sizeof(mDataStr)-1, lpIniFileName);
        size_t startpos = 0;
        for (int i = 0; i < length; i++) {
            if ('\0' == mDataStr[i]) {
                String feild = mDataStr + startpos;
                size_t endpos = feild.find_first_of('=');
                if (string::npos != endpos) {
                    String key = feild.substr(0, endpos);
                    String value = feild.substr(endpos + 1);
                    if (0 == feilds.count(key)) {
                        feilds.insert(make_pair(key, value));
                    } else {
                        assert(false); // Key 以存在
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

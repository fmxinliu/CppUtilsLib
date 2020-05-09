#pragma once
#include <tchar.h>
#include "dllexport.h"

namespace UtilTools
{
    class DLL_API Char
    {
    public:
        /** 判断是否为空白符
         * ' '    (0x20)  space (SPC)
         * '\t'   (0x09)  horizontal tab (TAB)
         * '\n'   (0x0a)  newline (LF)
         * '\v'   (0x0b)  vertical tab (VT)
         * '\f'   (0x0c)  feed (FF)
         * '\r'   (0x0d)  carriage return (CR)
         */
        static inline bool isWhitespace(TCHAR ch)
        {
            return !!isspace(ch);
        }
        // 判断是否为小数点
        static inline bool isDot(TCHAR ch)
        {
            return ch == '.';
        }
        // 判断是否为正负号
        static inline bool isSign(TCHAR ch)
        {
            return ch == '+' || ch == '-';
        }
        // 判断是否为数字
        static inline bool isDigit(TCHAR ch, bool ishex = false)
        {
            if (ishex) {
                return !!isxdigit(ch); // 十六进制
            }
            return !!isdigit(ch); // 十进制
        }
        // 判断是否为字母
        static inline bool isAlpha(TCHAR ch)
        {
            return !!isalpha(ch);
        }
        // 判断是否为小写字母
        static inline bool isLower(TCHAR ch)
        {
            return !!islower(ch);
        }
        // 判断是否为大写字母
        static inline bool isUpper(TCHAR ch)
        {
            return !!isupper(ch);
        }
        // 判断是否为控制符
        static inline bool isCntrl(TCHAR ch)
        {
            return !!iscntrl(ch);
        }
        // 判断是否为标点符号
        static inline bool isPunct(TCHAR ch)
        {
            return !!ispunct(ch);
        }
        // 转小写字母
        static inline TCHAR toLower(TCHAR ch)
        {
            return tolower(ch);
        }
        // 转大写字母
        static inline TCHAR toUpper(TCHAR ch)
        {
            return toupper(ch);
        }
    };
}
#pragma once
#include <tchar.h>
#include "dllexport.h"

namespace UtilTools
{
    class DLL_API Char
    {
    public:
        /** �ж��Ƿ�Ϊ�հ׷�
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
        // �ж��Ƿ�ΪС����
        static inline bool isDot(TCHAR ch)
        {
            return ch == '.';
        }
        // �ж��Ƿ�Ϊ������
        static inline bool isSign(TCHAR ch)
        {
            return ch == '+' || ch == '-';
        }
        // �ж��Ƿ�Ϊ����
        static inline bool isDigit(TCHAR ch, bool ishex = false)
        {
            if (ishex) {
                return !!isxdigit(ch); // ʮ������
            }
            return !!isdigit(ch); // ʮ����
        }
        // �ж��Ƿ�Ϊ��ĸ
        static inline bool isAlpha(TCHAR ch)
        {
            return !!isalpha(ch);
        }
        // �ж��Ƿ�ΪСд��ĸ
        static inline bool isLower(TCHAR ch)
        {
            return !!islower(ch);
        }
        // �ж��Ƿ�Ϊ��д��ĸ
        static inline bool isUpper(TCHAR ch)
        {
            return !!isupper(ch);
        }
        // �ж��Ƿ�Ϊ���Ʒ�
        static inline bool isCntrl(TCHAR ch)
        {
            return !!iscntrl(ch);
        }
        // �ж��Ƿ�Ϊ������
        static inline bool isPunct(TCHAR ch)
        {
            return !!ispunct(ch);
        }
        // תСд��ĸ
        static inline TCHAR toLower(TCHAR ch)
        {
            return tolower(ch);
        }
        // ת��д��ĸ
        static inline TCHAR toUpper(TCHAR ch)
        {
            return toupper(ch);
        }
    };
}
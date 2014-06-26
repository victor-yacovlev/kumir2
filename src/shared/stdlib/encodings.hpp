#ifndef ENCODINGS_HPP
#define ENCODINGS_HPP

#include <string>
#include <iostream>
#include <stdint.h>
#include <iterator>

namespace Kumir {

    enum Encoding { DefaultEncoding, ASCII, UTF8, UTF16INTEL, UTF16MOTOROLA, CP866, CP1251, KOI8R };
    enum EncodingError { NoEncodingError = 0, OutOfTable, StreamEnded };

    typedef const char * charptr;

    class AsciiCodingTable {
    public:
        static unsigned char enc(uint32_t symb, EncodingError & error) {
            error = NoEncodingError;
            uint32_t k = static_cast<uint32_t>(symb);
            if (k<128) {
                return static_cast<unsigned char>(k);
            }
            else {
                error = OutOfTable;
                return '?';
            }
        }
        static uint32_t dec(charptr & from, EncodingError & error) {
            error = NoEncodingError;
            if (from==0 || (*from)=='\0') {
                return L'\0';
            }
            unsigned char k = static_cast<unsigned char>(*from);
            from ++;
            if (k<128) {
                return static_cast<uint32_t>(k);
            }
            else {
                error = OutOfTable;
                return L'?';
            }
        }
    };

    class CP866CodingTable {
    public:
        static unsigned char enc(uint32_t symb, EncodingError & error) {
            error = NoEncodingError;
            uint32_t k = static_cast<uint32_t>(symb);
            if (k<128) {
                return static_cast<unsigned char>(k);
            }
            switch (k) {
#include "wchar_cp866.table"
                default: error = OutOfTable; return '?';
            }
        }
        static uint32_t dec(charptr & from, EncodingError & error) {
            error = NoEncodingError;
            if (from==0 || (*from)=='\0') {
                return L'\0';
            }
            unsigned char k = static_cast<unsigned char>(*from);
            from ++;
            if (k<128) {
                return static_cast<uint32_t>(k);
            }
            switch (k) {
#include "cp866_wchar.table"
                default: error = OutOfTable; return L'?';
            }
        }
    };

    class CP1251CodingTable {
    public:
        static unsigned char enc(uint32_t symb, EncodingError & error) {
            error = NoEncodingError;
            uint32_t k = static_cast<uint32_t>(symb);
            if (k<128) {
                return static_cast<unsigned char>(k);
            }
            switch (k) {
#include "wchar_windows-1251.table"
                default: error = OutOfTable; return '?';
            }
        }
        static uint32_t dec(charptr & from, EncodingError & error) {
            error = NoEncodingError;
            if (from==0 || (*from)=='\0')
                return L'\0';
            unsigned char k = static_cast<unsigned char>(*from);
            from ++;
            if (k<128)
                return static_cast<uint32_t>(k);
            switch (k) {
#include "windows-1251_wchar.table"
                default:
            {
                error = OutOfTable; return L'?';
            }
            }
        }
    };

    class KOI8RCodingTable {
    public:
        static unsigned char enc(uint32_t symb, EncodingError & error) {
            error = NoEncodingError;
            uint32_t k = static_cast<uint32_t>(symb);
            if (k<128)
                return static_cast<unsigned char>(k);
            switch (k) {
#include "wchar_koi8-r.table"
                default: error = OutOfTable; return '?';
            }
        }
        static uint32_t dec(charptr & from, EncodingError & error) {
            error = NoEncodingError;
            if (from==0 || (*from)=='\0')
                return L'\0';
            unsigned char k = static_cast<unsigned char>(*from);
            from ++;
            if (k<128)
                return static_cast<uint32_t>(k);
            switch (k) {
#include "koi8-r_wchar.table"
                default: error = OutOfTable; return L'?';
            }
        }
    };

    struct MultiByte {
        unsigned char data[3];
        unsigned char size;
    };

    class UTF8CodingTable {
    public:
        static MultiByte enc(uint32_t k, EncodingError & error) {
            // for implementation details see:
            //   man utf-8
            error = NoEncodingError;
            uint32_t v = static_cast<uint32_t>(k);
            MultiByte result;
            if (v <= 0x7F) {
                // 0xxxxxxx
                result.size = 1;
                result.data[0] = v & 0x7F; // 0x7F = 01111111
            }
            else if (v>=0x80 && v<=0x7FF) {
                // 110xxxxx,10xxxxxx
                result.size = 2;
                result.data[1] = v & 0x3F; // 0x3F = 00111111
                result.data[1] |= 0x80; // 0x80 = 10000000
                v = v >> 6;
                result.data[0] = v & 0x1F; // 0x1F = 00011111
                result.data[0] |= 0xC0; // 0xC0 = 11000000
            }
            else if (v>=0x800 && v<=0xFFFF) {
                // 1110xxxx, 10xxxxxx, 10xxxxxx
                result.size = 3;
                result.data[2] = v & 0x3F; // 0x3F = 00111111
                result.data[2] |= 0x80; // 0x80 = 10000000
                v = v >> 6;
                result.data[1] = v & 0x3F; // 0x3F = 00111111
                result.data[1] |= 0x80; // 0x80 = 10000000
                v = v >> 6;
                result.data[0] = v & 0x0F; // 0x0F = 00001111
                result.data[0] |= 0xE0; // 0xE0 = 11100000
            }
            else {
                // Support only Unicode Plane 0 (Base) !!!
                result.size = 1;
                result.data[0] = '?';
                error = OutOfTable;
            }
            return result;
        }
        inline static uint32_t dec(charptr & from, EncodingError & error) {
            error = NoEncodingError;
            uint32_t v = 0;
            if (from == 0 || (*from)=='\0') {
                error = StreamEnded;
                return L'\0';
            }
            unsigned char byte = (*from);
            from ++;
            unsigned char byte_first_3_bits = byte >> 5;
            unsigned char byte_first_4_bits = byte >> 4;
            if ( (byte & 0x80) == 0 ) {
                // first byte mask: 0xxxxxxx
                // -- only one byte used
                v = byte & 0x7F; // 0x7F = 01111111;
            }
            else if ( byte_first_3_bits==0x06 ) {
                // first byte mask: 110xxxxx
                // -- use two bytes
                v = byte & 0x1F; // 0x1F = 000xxxxx
                if (from == 0 || (*from)=='\0') {
                    error = StreamEnded;
                    return L'?';
                }
                byte = (*from);
                from ++;
                v = (v << 6) | (byte & 0x3F); // 0x3F = 00111111
            }
            else if (byte_first_4_bits==0x0E ) {
                // first byte mask: 1110xxxx
                // -- use three bytes
                v = byte & 0x0F; // 0x0F = 00001111
                if (from == 0 || (*from)=='\0') {
                    error = StreamEnded;
                    return L'?';
                }
                byte = (*from);
                from ++;
                v = (v << 6) | (byte & 0x3F); // 0x3F = 00111111
                if (from == 0 || (*from)=='\0') {
                    error = StreamEnded;
                    return L'?';
                }
                byte = (*from);
                from ++;
                v = (v << 6) | (byte & 0x3F); // 0x3F = 00111111
            }
            else {
                // Something going wrong:
                //   1) corrupted data; or
                //   2) symbol out of Basic Unicode
                //      (we don't support supplementary plane)
                error = OutOfTable;
                return L'?';
            }
            return v;
        }
    };

#ifndef NO_UNICODE
    class Coder {
    public:
        inline static std::string encode(Encoding E, const std::wstring & src, EncodingError &error) {
            error = NoEncodingError;
            std::string result;
            if (E!=UTF8) {
                result.reserve(src.length());
                for (size_t i=0; i<src.length(); i++) {
                    char ch = '\0';
                    if (E==CP866)
                        ch = CP866CodingTable::enc(src[i], error);
                    else if (E==CP1251)
                        ch = CP1251CodingTable::enc(src[i], error);
                    else if (E==KOI8R)
                        ch = KOI8RCodingTable::enc(src[i], error);
                    else if (E==ASCII)
                        ch = AsciiCodingTable::enc(src[i], error);
                    if (error) return result;
                    result.push_back(ch);
                }
            }
            else {
                result.reserve(3*src.length());
                for (size_t i=0; i<src.length(); i++) {
                    MultiByte mb = UTF8CodingTable::enc(src[i], error);
                    if (error) return result;
                    for (unsigned char j=0; j<mb.size; j++)
                        result.push_back(mb.data[j]);
                }
            }
            return result;
        }
        inline static std::wstring decode(Encoding E, const std::string & src, EncodingError &error) {
            error = NoEncodingError;
            std::wstring result;
            result.reserve(src.length());
            charptr p = src.c_str();
            wchar_t wch = L'\0';
            while (p!=0 && (*p)!='\0') {
                if (E==CP866) {
                    wch = CP866CodingTable::dec(p, error);
                }
                else if (E==CP1251) {
                    wch = CP1251CodingTable::dec(p, error);
                }
                else if (E==KOI8R) {
                    wch = KOI8RCodingTable::dec(p, error);
                }
                else if (E==UTF8) {
                    wch = UTF8CodingTable::dec(p, error);
                }
                else if (E==ASCII) {
                    wch = AsciiCodingTable::dec(p, error);
                }
                if (error) return result;
                result.push_back(wch);
            }
            return result;
        }
    };
#else
#   error Not implemented yet
#endif
}

#endif

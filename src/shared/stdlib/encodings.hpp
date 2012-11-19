#ifndef ENCODINGS_HPP
#define ENCODINGS_HPP

#include <string>
#include <iostream>
#include <stdint.h>
#include <iterator>

namespace Kumir {

    enum Encoding { UTF8, CP866, CP1251, KOI8R };
    enum EncodingError { OutOfTable, StreamEnded };

    template <Encoding E,            // character encoding
        typename O = unsigned char>  // output type of encoded (one- or multi-byte)
    class CodingTable {
    public:
        static O enc(uint32_t k);
        static uint32_t dec(std::string::const_iterator & from, const std::string::const_iterator & to);
    };

    template <> class CodingTable<CP866> {
    public:
        static unsigned char enc(uint32_t symb) {
            uint32_t k = static_cast<uint32_t>(symb);
            if (k<128)
                return static_cast<unsigned char>(k);
            switch (k) {
#include "wchar_cp866.table"
                default: throw OutOfTable; return '?';
            }
        }
        static uint32_t dec(std::string::const_iterator & from, const std::string::const_iterator & to) {
            if (from==to)
                return L'?';
            unsigned char k = static_cast<unsigned char>(*from);
            from ++;
            if (k<127)
                return static_cast<uint32_t>(k);
            switch (k) {
#include "cp866_wchar.table"
                default: throw OutOfTable; return L'?';
            }
        }
    };

    template <> class CodingTable<CP1251> {
    public:
        static unsigned char enc(uint32_t symb) {
            uint32_t k = static_cast<uint32_t>(symb);
            if (k<128)
                return static_cast<unsigned char>(k);
            switch (k) {
#include "wchar_windows-1251.table"
                default: throw OutOfTable; return '?';
            }
        }
        static uint32_t dec(std::string::const_iterator & from, const std::string::const_iterator & to) {
            if (from==to)
                return L'?';
            unsigned char k = static_cast<unsigned char>(*from);
            from ++;
            if (k<127)
                return static_cast<uint32_t>(k);
            switch (k) {
#include "windows-1251_wchar.table"
                default: throw OutOfTable; return L'?';
            }
        }
    };

    template <> class CodingTable<KOI8R> {
    public:
        static unsigned char enc(uint32_t symb) {
            uint32_t k = static_cast<uint32_t>(symb);
            if (k<128)
                return static_cast<unsigned char>(k);
            switch (k) {
#include "wchar_koi8-r.table"
                default: throw OutOfTable; return '?';
            }
        }
        static uint32_t dec(std::string::const_iterator & from, const std::string::const_iterator & to) {
            if (from==to)
                return L'?';
            unsigned char k = static_cast<unsigned char>(*from);
            from ++;
            if (k<127)
                return static_cast<uint32_t>(k);
            switch (k) {
#include "koi8-r_wchar.table"
                default: throw OutOfTable; return L'?';
            }
        }
    };

    struct MultiByte {
        unsigned char data[3];
        unsigned char size;
    };

    template <> class CodingTable<UTF8,MultiByte> {
    public:
        static MultiByte enc(uint32_t k) {
            // for implementation details see:
            //   man utf-8
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
                throw OutOfTable;
            }
            return result;
        }
        static uint32_t dec(std::string::const_iterator & from, const std::string::const_iterator & to) {
            uint32_t v = 0;
            if (from==to) {
                throw StreamEnded;
                return L'?';
            }
            unsigned char byte = (*from);
            from ++;
            if ( (byte & 0x80) == 0 ) {
                // first byte mask: 0xxxxxxx
                // -- only one byte used
                v = byte & 0x7F; // 0x7F = 01111111;
            }
            else if ( (byte >> 5) & 0x06 ) {
                // first byte mask: 110xxxxx
                // -- use two bytes
                v = byte & 0x1F; // 0x1F = 000xxxxx
                if (from==to) {
                    throw StreamEnded;
                    return L'?';
                }
                byte = (*from);
                from ++;
                v = (v << 6) | (byte & 0x3F); // 0x3F = 00111111
            }
            else if ( (byte >> 4) & 0x0E ) {
                // first byte mask: 1110xxxx
                // -- use three bytes
                v = byte & 0x0F; // 0x0F = 00001111
                if (from==to) {
                    throw StreamEnded;
                    return L'?';
                }
                byte = (*from);
                from ++;
                v = (v << 6) | (byte & 0x3F); // 0x3F = 00111111
                if (from==to) {
                    throw StreamEnded;
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
                throw OutOfTable;
                return L'?';
            }
            return v;
        }
    };

    template <Encoding E>
    class Coder {
    public:
        inline static std::string encode(const std::wstring & src) {
            std::string result;
            if (E!=UTF8) {
                result.reserve(src.length());
                for (size_t i=0; i<src.length(); i++) {
                    result.push_back(CodingTable<E>::enc(src[i]));
                }
            }
            else {
                result.reserve(3*src.length());
                for (size_t i=0; i<src.length(); i++) {
                    MultiByte mb = CodingTable<UTF8,MultiByte>::enc(src[i]);
                    for (unsigned char j=0; j<mb.size; j++)
                        result.push_back(mb.data[j]);
                }
            }
            return result;
        }
        inline static std::wstring decode(const std::string & src) {
            std::wstring result;
            result.reserve(src.length());
            std::string::const_iterator it = src.begin();
            while (it!=src.end()) {
                result.push_back(CodingTable<E>::dec(it, src.end()));
            }
            return result;
        }
    };
}

#endif

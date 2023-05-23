#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <locale>
#include <codecvt>
#include "windows.h"
#include <algorithm>
#include <cctype>

class BinaryUtils {

public:

    static std::string to_hex_string(const unsigned int i) {
        std::stringstream s;
        s << std::hex << i;
        return s.str();
    }

    static int roundUp( int numToRound, int multiple) {
        int remainder;

        if (multiple == 0)
            return numToRound;
        remainder = numToRound % multiple;
        if (remainder == 0)
            return numToRound;
        
        return (numToRound + multiple - remainder);
    }

    static constexpr unsigned int hash(const char* s, int off = 0) {
        return !s[off] ? 5381 : (hash(s, off + 1) * 33) ^ s[off];
    }

    static std::string int_to_ANSI( u_long bytes ) {
        return std::string(reinterpret_cast<char*>(&bytes), sizeof(bytes));
    }

    static std::string wchar_to_string(std::wstring wideStr) {

        //setup converter
        using convert_type = std::codecvt_utf8<wchar_t>;
        std::wstring_convert<convert_type, wchar_t> converter;

        //use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
        std::string converted_str = converter.to_bytes(wideStr);

        return converted_str;
    }

    static wchar_t* string_to_wchar( std::string narrow_string) {

        int wchars_num = MultiByteToWideChar(CP_UTF8, 0, narrow_string.c_str(), -1, NULL, 0);
        wchar_t* wStr = new wchar_t[wchars_num];
        MultiByteToWideChar(CP_UTF8, 0, narrow_string.c_str(), -1, wStr, (wchars_num));
        return wStr;
    }

  
    static std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
        }
        return str;
    }

    static std::string str_tolower(std::string s) {
        std::transform(s.begin(), s.end(), s.begin(),
            // static_cast<int(*)(int)>(std::tolower)         // wrong
            // [](int c){ return std::tolower(c); }           // wrong
            // [](char c){ return std::tolower(c); }          // wrong
            [](unsigned char c) { return tolower(c); } // correct
        );
        return s;
    }


};
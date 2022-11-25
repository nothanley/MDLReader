
#include <iostream>
#include <string>
#include <sstream>

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

};
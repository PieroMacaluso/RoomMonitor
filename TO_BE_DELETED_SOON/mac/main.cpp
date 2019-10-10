#include <iostream>
#include <bitset>
#include <sstream>

int main() {
    std::string s = "0x0";
    std::stringstream ss;
    ss << std::hex << s;
    unsigned n;
    ss >> n;
    std::bitset<4> b(n);
    // outputs "00000000000000000000000000001010"
    std::cout << b.to_string() << std::endl;

    std::cout << b.to_string()[2] << std::endl;
}
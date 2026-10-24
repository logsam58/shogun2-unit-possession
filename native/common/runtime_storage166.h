#pragma once
// Player builds never record gameplay/input to disk. Status channels remain
// available in memory for an explicitly requested live check.
#include <ostream>
#include <streambuf>
namespace storage166 {
class DiscardBuffer final : public std::streambuf {
    std::streamsize xsputn(const char*,std::streamsize count) override{return count;}
    int_type overflow(int_type value) override{return traits_type::not_eof(value);}
};
class DiscardLog final : public std::ostream {
    DiscardBuffer buffer;
public:
    DiscardLog():std::ostream(nullptr){rdbuf(&buffer);clear();}
    static constexpr bool enabled(){return false;}
};
}

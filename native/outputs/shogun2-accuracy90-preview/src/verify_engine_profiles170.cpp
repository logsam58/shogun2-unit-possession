#include "../../../common/engine_profiles170.h"
#include <iostream>
#include <stdexcept>
#include <string>
static void check170(bool ok,const char* why){if(!ok)throw std::runtime_error(why);}
int main(){try{
    for(const char* value:{engine170::stock_sha,engine170::bugfix_sha}){
        std::string sha=value;check170(sha.size()==64&&engine170::supported(sha),"Reviewed profile refused");
        for(size_t i=0;i<sha.size();++i){auto edited=sha;edited[i]=edited[i]=='0'?'1':'0';
            check170(!engine170::supported(edited),"Unreviewed modified image accepted");}
        check170(!engine170::supported(sha+"0")&&!engine170::supported(sha.substr(1)),"Wrong hash length accepted");
        check170(!engine170::supported(std::string("\0",1)+sha)&&!engine170::supported(sha+std::string("\0",1)),"Embedded terminator accepted");
    }
    check170(!engine170::supported("")&&!engine170::supported("unknown"),"Unknown engine accepted");
    std::cout<<"PASS stock/reviewed ModDB image profiles; 128 altered digests and invalid lengths/terminators refused.\n";
    return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}

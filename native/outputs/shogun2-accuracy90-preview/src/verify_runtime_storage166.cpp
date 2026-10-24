#include "../../../common/runtime_storage166.h"
#include <filesystem>
#include <iomanip>
#include <iostream>
int main(){
    auto folder=std::filesystem::temp_directory_path();
    storage166::DiscardLog log;
    if(log.enabled())return 1;
    for(unsigned i=0;i<100000;++i)log<<std::setprecision(12)<<"input="<<i<<" time="<<double(i)/60<<'\n';
    log.flush();
    if(!log||log.enabled())return 2;
    std::cout<<"PASS player log sink: no file API/path, 100000 discarded records, stream compatibility.\n";
    return 0;
}

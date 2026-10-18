#pragma once
#ifndef SHOGUN2_TRACE_FIXTURE_SHA256
#error Experimental tracing requires a pinned toy-executable fingerprint.
#endif
inline void require_trace_fixture111(HANDLE process,const std::wstring&folder){
    wchar_t actual[32768]{};DWORD n=32768;
    require(QueryFullProcessImageNameW(process,0,actual,&n)!=FALSE,"Toy image query failed");
    wchar_t own[32768]{};require(GetModuleFileNameW(nullptr,own,32768)>0,"Debugger image query failed");
    std::wstring expected(own);expected=expected.substr(0,expected.find_last_of(L"\\/"))+L"\\trace-fixture110.exe";
    require(_wcsicmp(expected.c_str(),actual)==0,"Experimental debugger accepts only its adjacent pinned toy executable");
    require(hash_file(actual)==SHOGUN2_TRACE_FIXTURE_SHA256,"Toy executable fingerprint differs");
    require(GetFileAttributesW((folder+L"\\targeted110-fixture.txt").c_str())!=INVALID_FILE_ATTRIBUTES,"Toy handshake absent");
}

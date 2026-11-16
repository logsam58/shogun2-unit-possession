// Exercise the production pinch release with the original game constructor,
// queue writer, group ID and field serializers. The engine never starts.
#define wmain old_ui138
#include "verify_native_ui185.cpp"
#undef wmain
#include "../../native/deck/frontage137.h"
#include <unordered_set>

static std::unordered_set<void*> allocated;
static unsigned registered_order[7]{};
static constexpr unsigned descriptor=0x12345678;
static void* __cdecl allocate138(unsigned size,void*) {
    auto address=calloc(1,size);
    assert(address&&allocated.insert(address).second);
    return address;
}
static void __cdecl free138(void* address) {
    if(address){assert(allocated.erase(address)==1);free(address);}
}
static void* __cdecl registry138(unsigned key) {
    assert(key==descriptor);
    return registered_order;
}
static float __fastcall minimum138(void*,void*) {return 10.f;}
static float __fastcall maximum138(void*,void*) {return 50.f;}

template<class T>static void put(uintptr_t address,const T& value) {
    memcpy(reinterpret_cast<void*>(address),&value,sizeof value);
}
static unsigned queue_size(uintptr_t queue) {
    unsigned length{};
    assert(native214::value(queue+4+0x5000,length));
    return length;
}

int wmain(int argc,wchar_t** argv) {
    assert(argc==3);
    SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);
    unsigned cases{};
    for(int engine=1;engine<argc;++engine) {
        auto module=LoadLibraryExW(argv[engine],nullptr,DONT_RESOLVE_DLL_REFERENCES);
        assert(module);
        const auto image=reinterpret_cast<uintptr_t>(module);
        {
            std::array<unsigned char,0xe00> root_buffer{};
            std::array<unsigned char,0x28400> environment_buffer{};
            std::array<unsigned char,0x200> owner_buffer{};
            std::array<unsigned char,0x500> selection_buffer{};
            std::array<unsigned char,0x5010> queue_buffer{};
            std::array<unsigned char,0x100> group_buffer{};
            auto start=[](auto& buffer){return reinterpret_cast<uintptr_t>(buffer.data());};
            const auto root=start(root_buffer),env=start(environment_buffer),owner=start(owner_buffer);
            const auto selection=start(selection_buffer),queue=start(queue_buffer),group=start(group_buffer);
            const uintptr_t groups[]{group};
            const uintptr_t commander[]{0,67};
            const uintptr_t policy_table[]{0,0,0,0,image+0x197430,image+0x2e6170};
            const uintptr_t policy=reinterpret_cast<uintptr_t>(policy_table);
            put(root+0x110,env);
            put(root+0x5b8,env);
            put(root+0x5b4,owner);
            put(env+0x2826c,selection);
            put(env+0x28048,queue);
            put(selection+0x4a8,owner);
            put(owner+0xb0,image+0x15c4da0);
            put(owner+0xb4,1u);
            put(owner+0xb8,1u);
            put(owner+0xbc,reinterpret_cast<uintptr_t>(groups));
            put(owner+0xe4,reinterpret_cast<uintptr_t>(commander));
            put(owner+0xe8,1u);put(owner+0xec,1u);put(owner+0xf0,reinterpret_cast<uintptr_t>(groups));
            put(group,image+0x15c4da0);
            put(group+0x28,reinterpret_cast<uintptr_t>(&policy));
            put(group+0x7c,0x1357u);
            put(group+0x44,26.f);
            const terrain224::Point point{{127.25f,4.5f,192.75f},0,0};
            const uint16_t facing=0x2345;
            put(group+0x2c,point);
            put(group+0x40,facing);

            auto stub=[&](uintptr_t rva,void* method){
                return jump185(image+rva,method);
            };
            const auto alloc=stub(0xf0510,reinterpret_cast<void*>(&allocate138));
            const auto dealloc=stub(0xf04d0,reinterpret_cast<void*>(&free138));
            const auto lookup=stub(0x207490,reinterpret_cast<void*>(&registry138));
            const auto low=stub(0x197430,reinterpret_cast<void*>(&minimum138));
            const auto high=stub(0x2e6170,reinterpret_cast<void*>(&maximum138));
            registered_order[0]=descriptor;
            const uintptr_t registry=reinterpret_cast<uintptr_t>(registered_order);
            const unsigned count=1;
            Patch185 root_global(image+0x1bc8180,&root,4),key(image+0x192eaac,&descriptor,4);
            Patch185 table_address(image+0x18d9000,&registry,4),table_count(image+0x18d9004,&count,4);
            Patch185 allocator(image+0xf0510,alloc.data(),5),deallocator(image+0xf04d0,dealloc.data(),5);
            Patch185 registry_patch(image+0x207490,lookup.data(),5),lower(image+0x197430,low.data(),5),upper(image+0x2e6170,high.data(),5);

            deck175::GroupFrontage gesture;
            const auto current=[](){return true;};
            for(float spread:{-1.f,-.5f,.5f,1.f}) {
                assert(gesture.begin(image,root,env,current)&&queue_size(queue)==0);
                assert(gesture.selection_live(image,root)&&queue_size(queue)==0);
                assert(gesture.release(image,root,env,spread,current)&&allocated.empty());
                const auto length=queue_size(queue);
                assert(length>=32&&length<128);
                const auto* bytes=queue_buffer.data()+4;
                const unsigned key_read=unsigned(bytes[2])<<24|unsigned(bytes[3])<<16|unsigned(bytes[4])<<8|bytes[5];
                assert(key_read==descriptor);
                const float expected=spread<0?26.f+16.f*spread:26.f+24.f*spread;
                assert(bytes[27]==0x2b&&bytes[28]==0x57&&bytes[29]==0x13);
                assert(!memcmp(bytes+38,point.p,sizeof point.p));
                assert(bytes[61]==0x45&&bytes[62]==0x23);
                float width{};
                memcpy(&width,bytes+64,4);
                assert(width==expected);
                assert(!gesture.release(image,root,env,spread,current)&&queue_size(queue)==length);

                std::array<unsigned char,0x5010> stock_queue{};
                alignas(16) unsigned char order[0x74]{};
                using Construct=void*(__thiscall*)(void*,uintptr_t,const terrain224::Point*,unsigned,float,unsigned,unsigned);
                using Dispatch=unsigned char(__thiscall*)(uintptr_t,void*);
                using Cleanup=void(__thiscall*)(void*);
                reinterpret_cast<Construct>(image+0x2557e0)(order,owner+0xb0,&point,facing,expected,0,1);
                assert(reinterpret_cast<Dispatch>(image+0x23d220)(start(stock_queue),order));
                reinterpret_cast<Cleanup>(image+0x1d10f0)(order+4);
                assert(queue_size(start(stock_queue))==length&&allocated.empty());
                assert(!memcmp(stock_queue.data()+4,bytes,length));
                put(queue+4+0x5000,0u);
                ++cases;
            }
            assert(gesture.begin(image,root,env,current));
            put(group+0x7c,0x1358u);
            assert(!gesture.release(image,root,env,1.f,current)&&queue_size(queue)==0);++cases;
            assert(gesture.begin(image,root,env,current));
            assert(!gesture.release(image,root,env,0.f,current)&&queue_size(queue)==0);++cases;
            assert(gesture.begin(image,root,env,current));
            put(queue+4+0x5000,0x4fffu);
            assert(!gesture.release(image,root,env,1.f,current)&&queue_size(queue)==0x4fff);++cases;
        }
        assert(FreeLibrary(module));
    }
    printf("PASS %u group frontage release cases through original native order constructor and queue writer\n",cases);
}

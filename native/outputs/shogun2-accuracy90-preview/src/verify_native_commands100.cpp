// Executes production adapter guards/handlers with bounded original native
// constructors, serializers, scene-query glue and skin cache code. Geometry,
// allocation/vector primitives and event providers are isolated fixture stubs.
#include "adapter.cpp"
#include <thread>
struct Packet100 {unsigned key{},integer_count{},byte_count{},short_count{},float_count{};unsigned integers[8]{},bytes[8]{},shorts[4]{};float floats[4]{};MovePoint100 point{};bool has_point{};};
struct Writer100 {unsigned char* packet;unsigned size,key,index;};
static std::vector<Packet100> packets100;
static unsigned group_cleanups100,context_cleanups100,sorts100,queries100,frees100,native_releases100,release_stage100;
static unsigned char native_cursor_hit100[60]{},native_cursor_copy100[60]{};static unsigned native_cursor_frees100;
static std::vector<UINT> native_messages100;static std::vector<unsigned> native_events100;
static void* __cdecl cursor_alloc100(unsigned size){require(size==60,"Unexpected fixture cursor allocation");return native_cursor_copy100;}
static void* __fastcall cursor_copy100(void* out,void*,const void* in){memcpy(out,in,60);return out;}
static void __cdecl cursor_free100(void* block,unsigned size){require(block==native_cursor_copy100&&size==60,"Foreign cursor cleanup");++native_cursor_frees100;}
static void __fastcall cursor_refresh100(void* cursor,void*,unsigned,unsigned,unsigned){field<uintptr_t>(reinterpret_cast<uintptr_t>(cursor),0x24)=reinterpret_cast<uintptr_t>(native_cursor_hit100);}
static LRESULT CALLBACK stock_window100(HWND window,UINT message,WPARAM flags,LPARAM point){
    native_messages100.push_back(message);if(message==WM_CANCELMODE)return 0;
    if(message==WM_RBUTTONDOWN||message==WM_RBUTTONUP)require((flags&(MK_SHIFT|MK_CONTROL))==command_native_modifiers100(),"Native Q lost Shift/Ctrl modifiers");
    RECT client{};GetClientRect(window,&client);
    unsigned type=reinterpret_cast<NativeMouseDecode100>(base+0xbd4a10)(window,message,flags,point,client.right,client.bottom,0);native_events100.push_back(type);
    if(type==5||type==4||type==12){auto root=field<uintptr_t>(base,0x1bc8180);reinterpret_cast<NativeCursorEvent100>(base+0x2f7350)(reinterpret_cast<void*>(root+0x10c),&type,0,0,1);}
    return 0;
}

static uintptr_t fixture_group100,fixture_world_one100,fixture_world_two100;
static unsigned scene_object100[4]{};static unsigned char scene_hits100[28]{};
static void append100(Writer100* writer,const void* bytes,unsigned count){
    require(writer->size+count<QUEUE_CAPACITY,"Fixture writer overflow");memcpy(writer->packet+writer->size,bytes,count);writer->size+=count;
    writer->packet[0]=static_cast<unsigned char>(writer->size);writer->packet[1]=static_cast<unsigned char>(writer->size>>8);memcpy(writer->packet+QUEUE_CAPACITY,&writer->size,4);
}
static void* __fastcall context100(void* memory,void*,void* packet,unsigned key,unsigned flags){
    auto writer=static_cast<Writer100*>(memory);*writer={static_cast<unsigned char*>(packet),7,key,unsigned(packets100.size())};
    Packet100 capture{};capture.key=key;packets100.push_back(capture);writer->packet[0]=7;writer->packet[1]=0;
    for(unsigned i=0;i<4;++i)writer->packet[2+i]=static_cast<unsigned char>(key>>(24-i*8));writer->packet[6]=static_cast<unsigned char>(flags);memcpy(writer->packet+QUEUE_CAPACITY,&writer->size,4);return memory;
}
static void __fastcall destroy_context100(void*,void*){++context_cleanups100;}
static void __fastcall destroy_group100(void* group,void*){require(*static_cast<uintptr_t*>(group)==base+0x15c4da0,"Destructor received wrong group");++group_cleanups100;}
static void* __fastcall copy_vector100(void* out,void*,const void* in){memcpy(out,in,12);return out;}
static void* __cdecl write_group100(void* context,const void* group){
    auto pointer=reinterpret_cast<uintptr_t>(group);require(field<uintptr_t>(pointer,0)==base+0x15c4da0&&field<unsigned>(pointer,0x14)==1,"Native serializer group identity differs");
    auto writer=static_cast<Writer100*>(context);unsigned id=7;append100(writer,&id,4);return context;
}
static void* __cdecl write_point100(void* context,const void* point){
    auto writer=static_cast<Writer100*>(context);auto& capture=packets100[writer->index];memcpy(&capture.point,point,20);capture.has_point=true;
    append100(writer,point,12);append100(writer,static_cast<const unsigned char*>(point)+16,4);return context;
}
static void* __cdecl write_uint100(void* context,unsigned value){auto writer=static_cast<Writer100*>(context);auto& c=packets100[writer->index];c.integers[c.integer_count++]=value;append100(writer,&value,4);return context;}
static void* __cdecl write_byte100(void* context,unsigned char value){auto writer=static_cast<Writer100*>(context);auto& c=packets100[writer->index];c.bytes[c.byte_count++]=value;append100(writer,&value,1);return context;}
static void* __cdecl write_short100(void* context,unsigned short value){auto writer=static_cast<Writer100*>(context);auto& c=packets100[writer->index];c.shorts[c.short_count++]=value;append100(writer,&value,2);return context;}
static void* __cdecl write_float100(void* context,float value){auto writer=static_cast<Writer100*>(context);auto& c=packets100[writer->index];c.floats[c.float_count++]=value;append100(writer,&value,4);return context;}
static uintptr_t __fastcall cell100(void*,void*,const float*,unsigned){return 1;}
static float __fastcall ground100(void*,void*,const float* xz){return xz[0]*.1f+xz[1]*.2f;}
static void __fastcall world_one100(void* world,void*,void* result,const void* shape,unsigned mode,unsigned sorted,unsigned mask,unsigned visible){
    require(reinterpret_cast<uintptr_t>(world)==fixture_world_one100&&mode==0&&sorted==1&&mask==0x303,"Native scene-query receiver/arguments differ");
    require(*static_cast<const unsigned*>(shape)==3,"Placement ray is not a native segment");(void)visible;
    auto ray=static_cast<const float*>(shape);require(std::abs(std::hypot(std::hypot(ray[4]-ray[1],ray[5]-ray[2]),ray[6]-ray[3])-4096.f)<.01f,"Native ray extent differs");
    auto vector=static_cast<unsigned*>(result);vector[0]=vector[1]=1;vector[2]=reinterpret_cast<unsigned>(scene_hits100);++queries100;
}
static void __fastcall world_two100(void* world,void*,void*,const void*,unsigned,unsigned,unsigned,unsigned,unsigned){require(reinterpret_cast<uintptr_t>(world)==fixture_world_two100,"Native second scene provider differs");}
static void __cdecl sort100(void* start,void* end,unsigned count,unsigned mode){require(start==scene_hits100&&reinterpret_cast<uintptr_t>(end)-reinterpret_cast<uintptr_t>(start)==28&&count==1&&mode==1,"Native ray result stride/sort differs");++sorts100;}
static void __cdecl free100(void* block){require(block==scene_hits100,"Native query freed foreign memory");++frees100;}
static bool __fastcall gate100(void*,void*){return true;}
static void __fastcall release100(void* task,void*){
    ++native_releases100;auto bytes=static_cast<unsigned char*>(task);
    if(release_stage100==1)bytes[0x6c]=1;
    else if(release_stage100==2)bytes[0x6d]=bytes[0x6e]=1;
}
struct NativeFixture100 {
    void* image{};Status status{};
    std::vector<unsigned char> root=std::vector<unsigned char>(0x1000),env=std::vector<unsigned char>(0x28300),common=std::vector<unsigned char>(0x200),phase=std::vector<unsigned char>(0x2900),
        manager=std::vector<unsigned char>(0x910),point_manager=std::vector<unsigned char>(0x100),camera=std::vector<unsigned char>(0x200),view=std::vector<unsigned char>(0x40),
        holder=std::vector<unsigned char>(0x210),peer_holder=std::vector<unsigned char>(0x210),selected=std::vector<unsigned char>(0x500),owner=std::vector<unsigned char>(0x200),
        config=std::vector<unsigned char>(0x100),actor=std::vector<unsigned char>(0x700),peer=std::vector<unsigned char>(0x700),actor_unit=std::vector<unsigned char>(0x1800),
        queue=std::vector<unsigned char>(QUEUE_CAPACITY+8),world_one=std::vector<unsigned char>(32),world_two=std::vector<unsigned char>(32);
    std::array<uintptr_t,2> holders{};std::array<uintptr_t,1> units{};unsigned char descriptors[6*28]{};
    static uintptr_t ptr(std::vector<unsigned char>& v){return reinterpret_cast<uintptr_t>(v.data());}
    void jump(unsigned address,void* destination){auto code=reinterpret_cast<unsigned char*>(base+address);code[0]=0xe9;unsigned delta=unsigned(reinterpret_cast<uintptr_t>(destination)-base-address-5);memcpy(code+1,&delta,4);}
    NativeFixture100(){
        const std::wstring path=L"C:\\Program Files (x86)\\Steam\\steamapps\\common\\Total War SHOGUN 2\\Empire.Retail.dll";require(hash_file(path)==EXPECTED_HASH,"Exact engine required");
        std::ifstream file(path,std::ios::binary);std::vector<unsigned char> bytes((std::istreambuf_iterator<char>(file)),{});
        auto dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(bytes.data());auto nt=reinterpret_cast<const IMAGE_NT_HEADERS32*>(bytes.data()+dos->e_lfanew);
        image=VirtualAlloc(nullptr,nt->OptionalHeader.SizeOfImage,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);require(image!=nullptr,"Private native fixture allocation");base=reinterpret_cast<uintptr_t>(image);
        memcpy(image,bytes.data(),nt->OptionalHeader.SizeOfHeaders);auto sections=IMAGE_FIRST_SECTION(nt);
        for(unsigned i=0;i<nt->FileHeader.NumberOfSections;++i){const auto& section=sections[i];if(section.SizeOfRawData)memcpy(reinterpret_cast<void*>(base+section.VirtualAddress),bytes.data()+section.PointerToRawData,section.SizeOfRawData);}
        auto reloc=nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];unsigned cursor=0;
        while(cursor<reloc.Size){auto block=reinterpret_cast<const IMAGE_BASE_RELOCATION*>(base+reloc.VirtualAddress+cursor);require(block->SizeOfBlock>=8,"Fixture relocation bounds");
            auto entries=reinterpret_cast<const unsigned short*>(block+1);unsigned count=(block->SizeOfBlock-8)/2;
            for(unsigned i=0;i<count;++i)if((entries[i]>>12)==IMAGE_REL_BASED_HIGHLOW)field<unsigned>(base,block->VirtualAddress+(entries[i]&4095))+=unsigned(base-nt->OptionalHeader.ImageBase);
            cursor+=block->SizeOfBlock;
        }
        jump(0x10ac90,reinterpret_cast<void*>(&copy_vector100));jump(0x1cc200,reinterpret_cast<void*>(&context100));jump(0x1d0d40,reinterpret_cast<void*>(&destroy_context100));jump(0x1d10f0,reinterpret_cast<void*>(&destroy_group100));
        jump(0x342930,reinterpret_cast<void*>(&write_group100));jump(0x285990,reinterpret_cast<void*>(&write_point100));jump(0x2859e0,reinterpret_cast<void*>(&write_uint100));jump(0x285a00,reinterpret_cast<void*>(&write_uint100));
        jump(0x285d00,reinterpret_cast<void*>(&write_byte100));jump(0x285ce0,reinterpret_cast<void*>(&write_short100));jump(0x285a20,reinterpret_cast<void*>(&write_float100));
        jump(0x3f6290,reinterpret_cast<void*>(&cell100));jump(0x3ea8a0,reinterpret_cast<void*>(&ground100));jump(0x36e8f0,reinterpret_cast<void*>(&world_one100));jump(0x36e7b0,reinterpret_cast<void*>(&world_two100));jump(0x101a80,reinterpret_cast<void*>(&sort100));jump(0xf04d0,reinterpret_cast<void*>(&free100));
        jump(0x335260,reinterpret_cast<void*>(&cursor_refresh100));jump(0xf7440,reinterpret_cast<void*>(&cursor_alloc100));jump(0x252bf0,reinterpret_cast<void*>(&cursor_copy100));jump(0x14fbc77,reinterpret_cast<void*>(&cursor_free100));
        field<unsigned>(reinterpret_cast<uintptr_t>(native_cursor_hit100),4)=1;field<unsigned char>(base,0x1b3116b)=1;
        const unsigned char routing[]={0x31,0xc0,0xc3};memcpy(reinterpret_cast<void*>(base+0x1821e0),routing,sizeof routing);
        DWORD previous{};require(VirtualProtect(image,0x1580000,PAGE_EXECUTE_READ,&previous)!=0,"Fixture code protection");FlushInstructionCache(GetCurrentProcess(),image,0x1580000);
        state=&status;enabled=1;possessed=ptr(actor);environment=ptr(env);unit=ptr(actor_unit);state->mode=POSSESSED;state->pid=GetCurrentProcessId();state->window_thread=GetCurrentThreadId();
        field<uintptr_t>(ptr(root),0)=base+0x170d284;field<uintptr_t>(ptr(root),0x18)=base+0x170d290;field<uintptr_t>(ptr(root),0x10c)=base+0x170d304;
        field<uintptr_t>(base,0x1bc8180)=ptr(root);field<uintptr_t>(ptr(root),0x110)=environment;field<uintptr_t>(environment,8)=ptr(common);
        field<uintptr_t>(ptr(common),0xb4)=ptr(phase);field<uintptr_t>(ptr(phase),0)=environment;field<unsigned>(ptr(phase),0x14)=6;
        field<uintptr_t>(environment,0x281fc)=ptr(manager);field<uintptr_t>(ptr(manager),0x8e0)=ptr(camera);field<uintptr_t>(ptr(manager),0x8fc)=possessed;
        field<uintptr_t>(ptr(common),0xb8)=ptr(point_manager);field<uintptr_t>(ptr(point_manager),0)=environment;
        field<uintptr_t>(ptr(common),0xc0)=ptr(world_one);field<uintptr_t>(ptr(common),0xc4)=ptr(world_two);fixture_world_one100=ptr(world_one);fixture_world_two100=ptr(world_two);
        field<uintptr_t>(ptr(camera),0)=base+0x15d22e8;field<uintptr_t>(ptr(camera),0x180)=ptr(manager);field<uintptr_t>(ptr(camera),0x188)=possessed;field<uintptr_t>(ptr(camera),0x184)=ptr(view);
        field<unsigned>(ptr(view),0x18)=2;holders={ptr(holder),ptr(peer_holder)};field<uintptr_t>(ptr(view),0x1c)=reinterpret_cast<uintptr_t>(holders.data());field<uintptr_t>(ptr(holder),8)=possessed;field<uintptr_t>(ptr(peer_holder),8)=ptr(peer);
        field<uintptr_t>(environment,0x2826c)=ptr(selected);field<uintptr_t>(ptr(selected),0x4a8)=ptr(owner);fixture_group100=ptr(owner)+0xb0;
        units={unit};field<uintptr_t>(fixture_group100,0)=base+0x15c4da0;field<unsigned>(fixture_group100,0x10)=field<unsigned>(fixture_group100,0x14)=1;field<uintptr_t>(fixture_group100,0x18)=reinterpret_cast<uintptr_t>(units.data());
        for(auto a:{possessed,ptr(peer)}){field<uintptr_t>(a,0)=base+0x15dfec8;field<unsigned char>(a,0xf0)=field<unsigned char>(a,0xa1)=1;field<uintptr_t>(a,0x214)=unit;field<uintptr_t>(a,0x28)=environment;field<uintptr_t>(a,0x20c)=ptr(config);}
        field<unsigned>(ptr(peer),0x1c8)=12;field<unsigned>(unit,4)=7;field<uintptr_t>(unit,0xf9c)=ptr(config);field<unsigned char>(unit,0xeb0)=1;
        require(followed(possessed,true),"Production current lease fixture invalid");state->lease_generation=possession_lease.generation;state->actor_id=0;state->unit_id=7;
        unsigned offsets[]={0x192e0d0,0x192da04,0x192d014,0x192eaac,0x192ca68,0x192e90c};
        for(unsigned i=0;i<6;++i){unsigned key=0x10203001+i;field<unsigned>(base,offsets[i])=key;memcpy(descriptors+i*28,&key,4);descriptors[i*28+16]=3;}
        field<uintptr_t>(base,0x18d9000)=reinterpret_cast<uintptr_t>(descriptors);field<unsigned>(base,0x18d9004)=6;field<uintptr_t>(environment,0x28048)=ptr(queue);
        scene_object100[0]=1;field<uintptr_t>(reinterpret_cast<uintptr_t>(scene_hits100),0)=reinterpret_cast<uintptr_t>(scene_object100);field<float>(reinterpret_cast<uintptr_t>(scene_hits100),4)=10;field<float>(reinterpret_cast<uintptr_t>(scene_hits100),8)=0;field<float>(reinterpret_cast<uintptr_t>(scene_hits100),12)=20;
        original_draw_gate=reinterpret_cast<RangedGate>(&gate100);original_native_release=reinterpret_cast<RangedAction>(&release100);original_skin_ready=reinterpret_cast<SkinReady>(base+0x4c56d0);
        command_generation100=possession_lease.generation;command_fire_mode100=LONG(command100::FireMode::AtWill);
    }
    ~NativeFixture100(){enabled=0;command_reset100();if(command_shared100){UnmapViewOfFile(command_shared100);command_shared100=nullptr;}if(command_mapping100){CloseHandle(command_mapping100);command_mapping100=nullptr;}state=nullptr;possession_lease.release();possessed=environment=unit=0;if(image)VirtualFree(image,0,MEM_RELEASE);base=0;}
    void clear_queue(){field<unsigned>(ptr(queue)+4,QUEUE_CAPACITY)=0;packets100.clear();}
};

static LONG WINAPI fault100(EXCEPTION_POINTERS* exception){std::cerr<<"Fixture fault code="<<std::hex<<exception->ExceptionRecord->ExceptionCode<<" rva="<<(reinterpret_cast<uintptr_t>(exception->ExceptionRecord->ExceptionAddress)-base)<<'\n';return EXCEPTION_EXECUTE_HANDLER;}
int main(){SetUnhandledExceptionFilter(&fault100);try{
    NativeFixture100 fixture;using P=NativeFixture100;
    require(followed(possessed,false)&&selected_possessed_group()==fixture_group100,"Actual production ownership/group guard failed");
    require(command_native_signatures100(base),"Exact native command signatures differ");
    MovePoint100 destination{{10,3,20},0,0};unsigned before{},after{},calls=0;
    for(unsigned facing:{0u,16384u,32768u,65535u})for(float width:{0.f,1.f,25.f,80.f})for(unsigned repeat=0;repeat<100;++repeat){
        fixture.clear_queue();__asm mov before,esp
        require(command_serialize100(&destination,facing,width),"Native facing/width packet refused");
        __asm mov after,esp
        require(before==after,"Native formation ABI stack imbalance");const auto& packet=packets100.back();
        require(packet.has_point&&packet.point.position[0]==10&&packet.point.position[1]==3&&packet.point.position[2]==20&&packet.short_count==1&&packet.shorts[0]==facing,"Native formation position/heading differs");
        if(width>0)require(packet.float_count==1&&packet.floats[0]==width&&packet.integer_count==2&&packet.integers[0]==0&&packet.integers[1]==9&&packet.byte_count==2,"Native width/move-speed/queue fields differ");
        else require(packet.float_count==0&&packet.byte_count==2&&packet.bytes[0]==0&&packet.bytes[1]==0,"Native preserved-width orientation flags differ");
        ++calls;
    }
    fixture.clear_queue();require(command_serialize100(&destination,0,0,-1,true),"Native point move refused");require(packets100.back().short_count==0&&packets100.back().float_count==0,"Point move changed native facing or width");
    for(unsigned mode:{0u,1u}){fixture.clear_queue();unsigned char native_before=field<unsigned char>(unit,0xeb0);require(command_serialize100(nullptr,0,0,int(mode)),"Native fire policy packet refused");
        require(packets100.back().integer_count==1&&packets100.back().integers[0]==mode&&field<unsigned char>(unit,0xeb0)==native_before,"Fire policy bypassed native dispatch or changed enum");}
    fixture.clear_queue();require(command_hold_here100(destination,16384),"Hold + native guard transaction refused");
    require(packets100.size()==2&&packets100[0].shorts[0]==16384&&packets100[1].byte_count==1&&packets100[1].bytes[0]==1,"Native guard-mode packet wrong");
    unsigned committed=field<unsigned>(P::ptr(fixture.queue)+4,QUEUE_CAPACITY);require(committed>0,"Hold transaction never published");
    fixture.clear_queue();field<unsigned>(P::ptr(fixture.queue)+4,QUEUE_CAPACITY)=QUEUE_CAPACITY-1;
    require(!command_hold_here100(destination,0)&&field<unsigned>(P::ptr(fixture.queue)+4,QUEUE_CAPACITY)==QUEUE_CAPACITY-1,"Full queue published a partial hold/guard order");
    fixture.clear_queue();unsigned contexts=context_cleanups100,groups=group_cleanups100;
    field<unsigned>(fixture_group100,0x14)=2;require(!command_serialize100(&destination,0,1)&&context_cleanups100==contexts&&group_cleanups100==groups,"Foreign/multiple selection constructed an order");field<unsigned>(fixture_group100,0x14)=1;
    unsigned serial=field<unsigned>(possessed,0x1c8);field<unsigned>(possessed,0x1c8)=serial+1;
    require(!command_serialize100(&destination,0,1),"Reused owned actor serial issued a native command");field<unsigned>(possessed,0x1c8)=serial;
    ViewRay ray{{0,2,0},{0,-1,0}};MovePoint100 picked{};
    require(command_pick100(ray,picked)&&picked.position[0]==10&&picked.position[1]==5&&picked.position[2]==20,"Native scene pick did not retain native terrain position");
    require(queries100==1&&sorts100==1&&frees100==1,"Native query vector/sort cleanup differs");
    scene_object100[0]=8;require(!command_pick100(ray,picked)&&frees100==2,"Actor hit became a move target or native result leaked");scene_object100[0]=1;
    // Actual production same-unit peer authorization, including original forwarding.
    command_fire_mode100=LONG(command100::FireMode::OnCommand);command_volley100.reset();memset(command_permits100,0,sizeof command_permits100);
    unsigned char task[0x90]{};field<uintptr_t>(reinterpret_cast<uintptr_t>(task),4)=P::ptr(fixture.peer);field<unsigned>(reinterpret_cast<uintptr_t>(task),0x78)=100;field<unsigned>(reinterpret_cast<uintptr_t>(task),0x88)=130;
    command100::ActorKey peer_key{};require(command_peer100(P::ptr(fixture.peer),peer_key),"Valid peer did not pass full production ownership proof");
    command_draw_started100(task);require(!weapon_draw_gate(task,nullptr),"Peer was ready before a successful player shot");command_on_shot100();require(weapon_draw_gate(task,nullptr),"Player shot did not release native draw wait");
    release_stage100=0;unsigned native_before=native_releases100;require(command_release_managed100(task)&&native_releases100==native_before+1,"Pre-marker native release not forwarded");
    require(command_volley100.available(peer_key),"Permission consumed before native release marker");
    release_stage100=1;require(command_release_managed100(task)&&!command_volley100.available(peer_key),"First actual marker did not consume one volley");
    release_stage100=2;native_before=native_releases100;require(command_release_managed100(task)&&native_releases100==native_before+1&&task[0x6e]==1,"Second native weapon event was cut short");
    // Reuse the same native task, timestamp and clip: the draw hook's actual
    // generation must still revoke the preceding draw's permission.
    task[0x6c]=task[0x6d]=task[0x6e]=0;command_draw_started100(task);native_before=native_releases100;
    require(command_release_managed100(task)&&native_releases100==native_before&&task[0x6e]==1,"Reused task inherited prior draw authorization");
    task[0x6c]=task[0x6d]=task[0x6e]=0;command_draw_started100(task);command_on_shot100();release_stage100=1;require(command_release_managed100(task)&&task[0x6c],"Second successful player shot did not authorize the next draw");
    command_fire_mode100=LONG(command100::FireMode::AtWill);require(weapon_draw_gate(task,nullptr)&&!command_release_managed100(task),"Fire-at-will native forwarding changed");
    command_fire_mode100=LONG(command100::FireMode::OnCommand);field<uintptr_t>(P::ptr(fixture.peer),0x214)=unit+16;
    require(weapon_draw_gate(task,nullptr)&&!command_release_managed100(task),"Foreign unit firing was intercepted");field<uintptr_t>(P::ptr(fixture.peer),0x214)=unit;
    // Exercise the actual native skin pointer/cache routine around the new
    // visibility pass. No camera/physics cache may receive the collapsed head.
    std::vector<unsigned char> binding(0xc0),asset(0x50),hierarchy(8*20),bones(7*8),render_object(0x20),result(12);
    float palette[96]{},raw[96]{};int32_t parents[8]={-1,0,0,0,3,-1,-1,-1};unsigned indices[7]={4,3,1,2,5,6,7},offsets[7]={0x9c,0xa0,0xa8,0xa4,0x90,0x94,0x98};
    for(unsigned i=0;i<8;++i){palette[i*12]=palette[i*12+5]=palette[i*12+10]=1;palette[i*12+7]=1.6f;field<int32_t>(P::ptr(hierarchy),i*20+16)=parents[i];}palette[4*12+7]=1.8f;memcpy(raw,palette,sizeof raw);
    for(unsigned i=0;i<7;++i){field<unsigned>(P::ptr(bones),i*8+4)=indices[i];field<uintptr_t>(P::ptr(binding),offsets[i])=P::ptr(bones)+i*8;}
    field<uintptr_t>(P::ptr(binding),0x1c)=P::ptr(asset);field<unsigned>(P::ptr(asset),0x3c)=8;field<uintptr_t>(P::ptr(asset),0x40)=P::ptr(hierarchy);
    field<uintptr_t>(P::ptr(fixture.holder),0x1e4)=P::ptr(binding);field<uintptr_t>(P::ptr(fixture.holder),0x14)=P::ptr(render_object);
    field<unsigned>(P::ptr(result),4)=8;field<uintptr_t>(P::ptr(result),8)=reinterpret_cast<uintptr_t>(palette);
    for(unsigned mode:{0u,8u,14u,18u,30u})for(unsigned frame=0;frame<200;++frame){
        field<unsigned>(possessed,0x1f0)=mode;skin_ready_hook(fixture.holder.data(),nullptr,result.data());
        require(std::abs(field<float>(P::ptr(fixture.holder),0x1b0)-1.8f)<1e-6f,"Native cached head position followed the collapsed head");
        require(palette[3*12]==.0001f&&palette[4*12]==.0001f,"Self visibility missing or accumulated on non-ranged/paired states");
        for(unsigned bone:{0u,1u,2u,5u,6u,7u})require(!memcmp(palette+bone*12,raw+bone*12,48),"Body/arms/weapon altered by self visibility");
    }
    require(field<uintptr_t>(P::ptr(render_object),0x14)==P::ptr(result),"Native renderer did not retain its actual palette");
    restore_soldier_pose();require(!memcmp(palette,raw,sizeof raw),"First-person exit did not restore exact native model matrices");
    SelfSkin100 prepared{};require(prepare_self_skin100(P::ptr(fixture.holder),P::ptr(result),prepared)&&current_self_skin100(prepared),"Current self-skin snapshot rejected");
    field<uintptr_t>(P::ptr(binding),0x1c)=P::ptr(asset)+4;require(!current_self_skin100(prepared),"Changed model asset reused a self-skin snapshot");finish_self_skin100(prepared);require(!memcmp(palette,raw,sizeof raw),"Retired model snapshot changed its palette");field<uintptr_t>(P::ptr(binding),0x1c)=P::ptr(asset);
    // Native skin calls for another actor still see their original full model.
    field<uintptr_t>(P::ptr(fixture.peer_holder),0x1e4)=P::ptr(binding);field<uintptr_t>(P::ptr(fixture.peer_holder),0x14)=P::ptr(render_object);
    skin_ready_hook(fixture.peer_holder.data(),nullptr,result.data());require(!memcmp(palette,raw,sizeof raw),"Foreign model was hidden");
    // Q is the stock RMB message path. Run the actual native mouse decoder and
    // cursor press/release cleanup; the window host, world refresh, allocator and
    // snapshot-copy primitives are fixture substitutions. No custom Q packet.
    HWND window=CreateWindowExW(0,L"STATIC",L"Command100 fixture",WS_POPUP,0,0,1280,720,nullptr,nullptr,GetModuleHandleW(nullptr),nullptr);require(window!=nullptr,"Hidden input fixture window");
    input_window=window;native_window_proc=&stock_window100;state->heartbeat=GetTickCount64();
    auto cursor=P::ptr(fixture.root)+0x10c;unsigned packets_before=unsigned(packets100.size());
    require(command_window100(window,WM_KEYDOWN,'Q',0)&&command_window100(window,WM_KEYDOWN,'Q',1L<<30),"Q press/repeat capture differs");
    require(command_placing100&&native_messages100.size()==1&&native_messages100[0]==WM_RBUTTONDOWN&&native_events100[0]==5&&field<unsigned char>(cursor,0x5b)==1,"Q did not enter exact native RMB/cursor state");
    require(command_window100(window,WM_MOUSEMOVE,0,MAKELPARAM(64,96))&&field<unsigned char>(base,0x17acf95)==1,"Q drag lost the native right-button bit");
    require(command_window100(window,WM_KEYUP,'Q',0)&&!command_placing100&&native_messages100.back()==WM_RBUTTONUP&&native_events100.back()==4&&field<unsigned char>(cursor,0x5b)==0&&field<unsigned char>(base,0x17acf95)==0,"Q release did not use native event4 cleanup");
    command100::Event event{};require(!command_pop100(event)&&packets100.size()==packets_before,"Q constructed a custom movement packet");
    require(command_window100(window,WM_KEYDOWN,'Q',0),"Second Q press");auto release_count=command_native_releases100;
    require(!command_window100(window,WM_KILLFOCUS,0,0)&&!command_placing100&&command_native_releases100==release_count&&field<unsigned char>(cursor,0x5b)==0&&field<unsigned char>(base,0x17acf95)==0,"Focus loss committed or retained native RMB gesture");
    require(command_native_presses100==2&&command_native_moves100==1&&command_native_releases100==1&&command_native_cancels100==1&&native_cursor_frees100==2,"Native Q lifecycle count differs");
    require(command_window100(window,WM_KEYDOWN,'Q',0),"Third Q press after focus cleanup");state->release=1;command_native_cancel100();require(!command_placing100&&field<unsigned char>(cursor,0x5b)==0,"Ownership release retained native placement");state->release=0;command_key_held100=0;
    require(command_window100(window,WM_KEYDOWN,'Q',0),"Q before asynchronous release");auto messages_before=native_messages100.size();
    std::thread release_worker([]{command_native_cancel100();});release_worker.join();
    require(native_messages100.size()==messages_before&&!command_placing100&&command_native_root100,"Native UI cleanup ran on the worker thread");
    command_window100(window,COMMAND_CANCEL_MESSAGE100,command_native_sequence100,0);require(!command_native_root100&&field<unsigned char>(cursor,0x5b)==0,"Posted window-thread cleanup did not finish");command_key_held100=0;
    field<uintptr_t>(P::ptr(fixture.root),0)=0;require(command_window100(window,WM_KEYDOWN,'Q',0)&&!command_placing100,"Foreign UI root admitted stock input");field<uintptr_t>(P::ptr(fixture.root),0)=base+0x170d284;
    auto characters_before=native_messages100.size();for(auto key:{'q','e','z','c','r'})weapon_window_proc(window,WM_CHAR,key,0);
    require(native_messages100.size()==characters_before&&!keyboard_window_owned84(VK_F2),"Command characters leaked or native F2 was remapped");
    input_window=nullptr;native_window_proc=nullptr;DestroyWindow(window);
    require(context_cleanups100==group_cleanups100,"Native order/context cleanup lifecycles differ");
    std::cout<<"PASS: "<<calls<<" original native formation packets; point/fire/guard ABI and atomic queue failure; actual lease/foreign guards; native scene/sort lifetime; per-draw command volleys; 1000 original skin-cache/visibility cycles and restoration; stock native Q/RMB press-drag-release and no-order focus cleanup. Providers/allocators are fixture substitutions; no game loaded or launched.\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}

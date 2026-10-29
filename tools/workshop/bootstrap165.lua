-- First-party Workshop startup only. No original game script/artwork is embedded.
-- File offsets and owned-pack decoding belong to the native uint64 reader.
local args={...}
local arg_count=select("#",...)
local stock_invoked167=false
local error_reporter167
local function run_stock167(original)
    local chunk,why=loadstring(original,"@owned-game/frontend-root");assert(chunk,why)
    setfenv(chunk,getfenv(1))
    -- Never run the original twice after partially registering callbacks.
    stock_invoked167=true
    chunk(unpack(args,1,arg_count))
end
local function fallback_stock167()
    -- Emergency menu recovery only; normal startup retains native uint64 I/O.
    -- Split offsets into base-65536 limbs so float Lua cannot round away bytes.
    -- Restrict seeks to signed-long range for the game's Windows Lua CRT.
    local f=assert(io.open("data/data.pack","rb"),"Cannot open owned data.pack for menu recovery")
    local ok,result=pcall(function()
        local header=assert(f:read(32));assert(#header==32 and header:sub(1,4)=="PFH3","Unsupported owned pack")
        local function pair(s,p)
            local a,b,c,d=s:byte(p,p+3);assert(d,"Truncated pack integer")
            return a+b*256,c+d*256
        end
        local function small(s,p)local lo,hi=pair(s,p);return lo+hi*65536 end
        local flags=small(header,5);local count=small(header,17);local size=small(header,21)
        assert((flags==0 or flags==1 or flags==3) and small(header,9)==0 and small(header,13)==0,"Unsupported owned pack variant")
        assert(count<=100000 and size<=8388608,"Owned pack index exceeds safe Lua recovery bound")
        local index=assert(f:read(size));assert(#index==size,"Truncated owned pack index")
        local base=32+size;local hi=math.floor(base/65536);local lo=base-hi*65536
        local cursor=1;local root_hi,root_lo,root_size
        for i=1,count do
            local nlo,nhi=pair(index,cursor);cursor=cursor+4
            local last=assert(index:find("\0",cursor,true),"Unterminated owned pack path")
            local name=index:sub(cursor,last-1):gsub("\\","/"):lower();cursor=last+1
            if name=="ui/frontend ui/layout_scripts/root.luac" then
                assert(not root_size and nhi<4,"Duplicate or excessive original UI script")
                root_hi,root_lo,root_size=hi,lo,nlo+nhi*65536
            end
            local sum=lo+nlo;local carry=math.floor(sum/65536);lo=sum-carry*65536;hi=hi+nhi+carry
            assert(hi<=262144,"Owned pack offsets exceed recovery bound")
        end
        assert(cursor==#index+1 and root_size and root_size>0,"Original UI entry unavailable")
        assert(root_hi<32768,"Original UI requires native 64-bit recovery")
        assert(f:seek("set",root_hi*65536),"Cannot seek owned UI block")
        assert(f:seek("cur",root_lo),"Cannot seek exact owned UI byte")
        local original=assert(f:read(root_size));assert(#original==root_size,"Truncated original UI script")
        assert(original:sub(1,12)==string.char(27,76,117,97,81,0,1,4,4,4,4,0),"Original UI bytecode differs")
        return original
    end)
    f:close();assert(ok,result);run_stock167(result)
end
local function initialize165()
    assert(type(io)=="table" and type(io.open)=="function" and type(os)=="table" and type(os.getenv)=="function","Local cache I/O unavailable")
    assert(type(package)=="table" and type(package.loadlib)=="function","Native Lua loading unavailable")
    local folder=assert(os.getenv("LOCALAPPDATA"),"User cache unavailable")
    local path=folder.."/S2UP-Workshop165-__NATIVE_SHA165__.dll"
    local payload=table.concat({
-- __NATIVE_PAYLOAD165__
    })
    local function matches(candidate)
        local f=io.open(candidate,"rb");if not f then return false end
        local actual=f:read(#payload+1);f:close();return actual==payload
    end
    local existing=io.open(path,"rb")
    if existing then existing:close();assert(matches(path),"Existing native cache differs; preserved")
    else
        local temporary,file
        for i=1,32 do
            local candidate=path..".new."..i
            -- Reuse a fully written stage left by an interrupted launch.
            if matches(candidate) then
                if not os.rename(candidate,path) then
                    assert(matches(path),"Native cache commit failed; existing target preserved")
                    os.remove(candidate)
                end
                assert(matches(path),"Native cache readback differs")
                temporary=nil;break
            end
            -- The pinned game CRT implements x (exclusive creation). Never use wb.
            local f=io.open(candidate,"wbx")
            if f then temporary,file=candidate,f;break end
        end
        if not matches(path) then
            assert(file,"Cannot create exclusive native staging file")
            local ok,written=pcall(function()return file:write(payload)end)
            local closed=file:close()
            if not (ok and written and closed and matches(temporary)) then
                -- This invocation exclusively created the staging file.
                os.remove(temporary)
                error("Native staging write failed")
            end
            local renamed=os.rename(temporary,path)
            if not renamed then
                if matches(temporary) then os.remove(temporary) end
                assert(matches(path),"Native cache commit failed; existing target preserved")
            end
        elseif file then
            file:close();os.remove(temporary) -- Another invocation committed first.
        end
        assert(matches(path),"Native cache readback differs")
    end
    payload=nil
    local prepare,why=package.loadlib(path,"s2up_workshop_prepare165");assert(type(prepare)=="function",why)
    local start,reason=package.loadlib(path,"s2up_workshop_start165");assert(type(start)=="function",reason)
    error_reporter167=package.loadlib(path,"s2up_workshop_error167")
    -- Each entry checks exactly one argument, then returns that existing value
    -- on success; no Lua stack writes or engine object calls are needed.
    assert(prepare(true),"Owned root preparation failed")
    -- Native guard owns this temporary file. D shares delete access; this mode
    -- is verified in the supported engine CRT and the matching float Lua host.
    local f=assert(io.open(path..".root.luac","rbD"),"Prepared UI root unavailable")
    local original=f:read(262145);f:close();assert(original and #original>0 and #original<=262144,"Prepared UI root exceeds bound")
    run_stock167(original)
    assert(start(true),"Native worker startup failed")
end
local ok,why=pcall(initialize165)
if not ok then
    if not stock_invoked167 then
        local recovered,recovery=pcall(fallback_stock167)
        if not recovered then pcall(function()print("Unit Possession menu recovery: "..string.sub(tostring(recovery),1,512))end) end
    end
    pcall(function()print("Shogun 2 Unit Possession: "..string.sub(tostring(why),1,512))end)
    -- A single bounded failure note also works when the DLL was blocked.
    -- Exclusive creation preserves any existing file; no per-session history.
    pcall(function()
        local folder=os.getenv("LOCALAPPDATA");if not folder then return end
        local file=io.open(folder.."/S2UP-Workshop-startup-error.txt","wbx")
        if file then
            local note="Shogun 2 Unit Possession bootstrap error\nNative __NATIVE_SHA165__\n"..tostring(why)
            file:write(note:sub(1,4096));file:close()
        end
    end)
    if type(error_reporter167)=="function" then pcall(error_reporter167) end
end

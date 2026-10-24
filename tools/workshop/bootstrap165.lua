-- First-party Workshop startup only. No original game script/artwork is embedded.
-- File offsets and owned-pack decoding belong to the native uint64 reader.
local args={...}
local arg_count=select("#",...)
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
    -- Each entry checks exactly one argument, then returns that existing value
    -- on success; no Lua stack writes or engine object calls are needed.
    assert(prepare(true),"Owned root preparation failed")
    local f=assert(io.open(path..".root.luac","rb"),"Prepared owned root unavailable")
    local original=f:read(13740);f:close();assert(original and #original==13739,"Prepared root size differs")
    local chunk,error=loadstring(original,"@owned-game/frontend-root");assert(chunk,error)
    setfenv(chunk,getfenv(1));chunk(unpack(args,1,arg_count))
    assert(start(true),"Native worker startup failed")
end
local ok,why=pcall(initialize165)
if not ok then
    pcall(function()print("Shogun 2 Unit Possession: "..string.sub(tostring(why),1,512))end)
end

-- Real package.loadlib/float Lua bridge; game UI itself is an explicit fixture.
local original_path,wrapper_path=assert(arg[1]),assert(arg[2])
local function env165()
    local count={clear=0,required=0,colour=0,metrics=0,shortcut=0}
    local c={Find=function(self)return self end}
    c.SetImageColour=function()count.colour=count.colour+1 end
    c.SetImageMetrics=function()count.metrics=count.metrics+1 end
    c.StealShortcutKey=function()count.shortcut=count.shortcut+1 end
    local env={system={ClearRequiredFiles=function()count.clear=count.clear+1 end},
        UIComponent=function()return c end,Address=1234,FrontEnd={ScreenSize=function()return 1920,1080 end},
        require=function(name)assert(name=="Utilities");count.required=count.required+1;return {} end,
        package={path="",loadlib=package.loadlib},print=function()end}
    setmetatable(env,{__index=_G})
    local initial={};for k in pairs(env)do initial[k]=true end
    return env,count,initial
end
local function run(path)
    local env,count,initial=env165();local chunk=assert(loadfile(path));setfenv(chunk,env);chunk("arg",nil,"last")
    local callbacks={};local n=0
    for k,v in pairs(env)do if not initial[k] then
        assert(type(v)=="function" and getfenv(v)==env,"Callback/environment changed")
        callbacks[k]=string.dump(v);n=n+1
    end end
    assert(n==37,"Missing stock callback")
    assert(count.clear==1 and count.required==1 and count.colour==1 and count.metrics==1 and count.shortcut==1)
    return callbacks
end
local original=run(original_path)
for i=1,2 do
    local actual=run(wrapper_path)
    for k,v in pairs(original)do assert(actual[k]==v,"Callback differs: "..k)end
end
-- Win32 delete-on-close can deny new opens while the native guard lives.
-- Rename reports ENOENT only after the bridge has retired its own handoff.
local function assert_gone(path)
    local ok,_,code=os.rename(path,path..".probe")
    if ok then os.rename(path..".probe",path)end
    assert(not ok and code==2,"Frontend left a protected handoff on disk")
end
local function failure_case(mode)
    local env,count=env165()
    local handoff,checked
    env.package.loadlib=function(path,symbol)
        handoff=path..".root.luac"
        return package.loadlib(path,symbol)
    end
    if mode=="callback" then
        env.system.ClearRequiredFiles=function()
            count.clear=count.clear+1
            assert_gone(assert(handoff))
            checked=true
            error("Fixture stock callback failed")
        end
    else
        env.io=setmetatable({open=function(path,flags)
            if path==handoff then
                if mode=="open" then return nil end
                return {read=function()error("Fixture read failed")end,
                        close=function()checked=true end}
            end
            if path=="data/data.pack" then
                return io.open(assert(os.getenv("S2UP165_FIXTURE_GAME")).."/data/data.pack",flags)
            end
            return io.open(path,flags)
        end},{__index=io})
    end
    local chunk=assert(loadfile(wrapper_path))
    setfenv(chunk,env)
    chunk()
    assert(count.clear==1,"Stock menu did not run exactly once")
    if mode~="open" then assert(checked,"Frontend did not clean up before stock entry or fallback")end
    assert_gone(assert(handoff))
end
for _,mode in ipairs({"callback","open","read"})do failure_case(mode)end
print("PASS frontend handoff released before original callback and after open/read errors")

-- The production DLL must refuse this standalone Lua host without a worker.
local prod=assert(arg[3]);local prepare=assert(package.loadlib(prod,"s2up_workshop_prepare165"))
assert(prepare(true)==nil,"Production bridge accepted non-game host")
local fixture=assert(arg[4]);local prepare_fixture=assert(package.loadlib(fixture,"s2up_workshop_prepare165"))
local start_fixture=assert(package.loadlib(fixture,"s2up_workshop_start165"))
assert(start_fixture(true)==nil,"Worker accepted start before preparation")
assert(prepare_fixture()==nil and prepare_fixture(true,true)==nil,"Invalid bridge arity accepted")
assert(prepare_fixture(true)==true and start_fixture(true)==true,"Existing Lua argument result contract failed")
local report=assert(package.loadlib(fixture,"s2up_workshop_error167"))
assert(report()==nil and report(true,true)==nil,"Error reporter must return zero results without Lua stack access")
print("PASS real loadlib/owned-root recovery/all37 callbacks/environments/repeat; production non-game rejection. Fixture gameplay worker disabled.")

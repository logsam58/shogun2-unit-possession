-- Actual float-Lua menu preservation under injected native startup failures.
local original,wrapper,owned_pack,fixture_dll=assert(arg[1]),assert(arg[2]),assert(arg[3]),assert(arg[4])
local failure_note167
local function environment(mode)
    local count={clear=0,required=0,colour=0,metrics=0,shortcut=0,notified=0}
    local component={Find=function(self)return self end}
    component.SetImageColour=function()count.colour=count.colour+1 end
    component.SetImageMetrics=function()count.metrics=count.metrics+1 end
    component.StealShortcutKey=function()count.shortcut=count.shortcut+1 end
    local env={system={ClearRequiredFiles=function()count.clear=count.clear+1 end},
        UIComponent=function()return component end,Address=1234,
        FrontEnd={ScreenSize=function()return 1920,1080 end},
        require=function(name)assert(name=="Utilities");count.required=count.required+1;return {} end,
        package={path=""},print=function()end}
    env.os=setmetatable({getenv=function()return "fixture-cache-denied"end},{__index=os})
    env.io=setmetatable({open=function(path,mode_open)
        if path=="data/data.pack" then assert(mode_open=="rb");return io.open(owned_pack,"rb") end
        if path:sub(-31)=="S2UP-Workshop-startup-error.txt" then
            assert(mode_open=="wbx","Failure note must never overwrite a file")
            if failure_note167 then return nil end
            return {write=function(self,value)assert(#value<=4096);failure_note167=value;return self end,
                    close=function()return true end}
        end
        if mode=="cache-denied" then return nil,"fixture cache unavailable" end
        if path:sub(-10)==".root.luac" then return io.open(original,"rb") end
        if path:sub(-4)==".dll" then return io.open(fixture_dll,"rb") end
        return nil,"fixture staging denied"
    end},{__index=io})
    env.package.loadlib=function(path,symbol)
        if mode=="loadlib-denied" then return nil,string.rep("fixture native load rejected ",1000) end
        if symbol=="s2up_workshop_error167" then return function()count.notified=count.notified+1 end end
        if symbol=="s2up_workshop_prepare165" then return function()return mode~="prepare-denied" and true or nil end end
        if symbol=="s2up_workshop_start165" then return function()assert(count.clear==1);return nil end end
        error("Unknown native entry")
    end
    setmetatable(env,{__index=_G});local initial={};for k in pairs(env)do initial[k]=true end
    return env,count,initial
end
local function run(file,mode)
    local env,count,initial=environment(mode);local chunk=assert(loadfile(file));setfenv(chunk,env);chunk("arg",nil,"last")
    local callbacks={};local n=0
    for key,value in pairs(env)do if not initial[key] then
        assert(type(value)=="function" and getfenv(value)==env,"Stock callback/environment replaced")
        callbacks[key]=string.dump(value);n=n+1
    end end
    assert(n==37,"Stock menu callbacks were lost after "..mode)
    assert(count.clear==1 and count.required==1 and count.colour==1 and count.metrics==1 and count.shortcut==1,"Stock initialization duplicated")
    return callbacks,count
end
local baseline=run(original,"original")
for _,mode in ipairs({"loadlib-denied","cache-denied","prepare-denied","start-denied"})do
    local actual,count=run(wrapper,mode)
    for key,value in pairs(baseline)do assert(actual[key]==value,"Stock callback changed: "..key)end
    if mode=="prepare-denied" or mode=="start-denied" then assert(count.notified==1)end
end
assert(failure_note167 and #failure_note167==4096,"Failure note was not bounded")
failure_note167="foreign file"
run(wrapper,"prepare-denied")
assert(failure_note167=="foreign file","An existing failure note was overwritten")
print("PASS cache/loadlib/prepare failures preserve all37 original menu callbacks; start failure does not initialize UI twice. Exact float-Lua seek on the owned pack exercised.")

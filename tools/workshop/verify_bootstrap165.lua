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
-- The production DLL must refuse this standalone Lua host without a worker.
local prod=assert(arg[3]);local prepare=assert(package.loadlib(prod,"s2up_workshop_prepare165"))
assert(prepare(true)==nil,"Production bridge accepted non-game host")
local fixture=assert(arg[4]);local prepare_fixture=assert(package.loadlib(fixture,"s2up_workshop_prepare165"))
local start_fixture=assert(package.loadlib(fixture,"s2up_workshop_start165"))
assert(start_fixture(true)==nil,"Worker accepted start before preparation")
assert(prepare_fixture()==nil and prepare_fixture(true,true)==nil,"Invalid bridge arity accepted")
assert(prepare_fixture(true)==true and start_fixture(true)==true,"Existing Lua argument result contract failed")
print("PASS real loadlib/owned-root recovery/all37 callbacks/environments/repeat; production non-game rejection. Fixture gameplay worker disabled.")

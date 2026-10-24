-- Matching float Lua fixture. Game UI calls are stubs, native load is mocked.
local stock_path,wrapper_path=assert(arg[1]),assert(arg[2])
local function environment(fail_native)
    local count={clear=0,require=0,colour=0,metrics=0,shortcut=0,loadlib=0,entry=0}
    local component={}
    component.Find=function()return component end
    component.SetImageColour=function()count.colour=count.colour+1 end
    component.SetImageMetrics=function()count.metrics=count.metrics+1 end
    component.StealShortcutKey=function()count.shortcut=count.shortcut+1 end
    local env={}
    env.system={ClearRequiredFiles=function()count.clear=count.clear+1 end}
    env.require=function(name)assert(name=="Utilities");count.require=count.require+1;return {} end
    env.UIComponent=function()return component end
    env.Address=1234
    env.FrontEnd={ScreenSize=function()return 1920,1080 end}
    env.print=function()end
    env.package={path="",loadlib=function()
        -- Native initialization must happen after the original root is complete.
        assert(type(env.OnAdoptChild)=="function" and type(env.SetTooltip)=="function")
        assert(count.clear==1 and count.shortcut==1)
        count.loadlib=count.loadlib+1
        if fail_native then error("injected native-load failure") end
        return function()count.entry=count.entry+1 end
    end}
    setmetatable(env,{__index=_G})
    local initial={};for key in pairs(env)do initial[key]=true end
    return env,count,initial
end
local function run(path,fail_native)
    local env,count,initial=environment(fail_native)
    local chunk=assert(loadfile(path));setfenv(chunk,env);chunk("fixture-argument")
    local callbacks={};local total=0
    for key,value in pairs(env)do
        if not initial[key] then
            assert(type(value)=="function","Unexpected original global: "..key)
            assert(getfenv(value)==env,"Original callback environment changed")
            callbacks[key]=string.dump(value);total=total+1
        end
    end
    assert(total==37,"Missing original callback")
    assert(count.clear==1 and count.require==1 and count.colour==1 and count.metrics==1 and count.shortcut==1)
    return callbacks,count,env
end
local expected,baseline,stock_env=run(stock_path,false)
assert(baseline.loadlib==0 and baseline.entry==0)
for _,fail in ipairs({false,true})do
    local actual,count,env=run(wrapper_path,fail)
    for key,value in pairs(expected)do assert(actual[key]==value,"Callback bytecode changed: "..key) end
    assert(env.package.path==stock_env.package.path)
    assert(count.loadlib==1 and count.entry==(fail and 0 or 1))
end
print("PASS all37 original root callbacks byte-exact, original environments and initialization counts, native-after-original ordering, contained native failure (native loading mocked)")

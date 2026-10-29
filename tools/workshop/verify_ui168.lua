-- Actual float Lua and production bootstrap/fixture native bridge.
local wrapper=assert(arg[1])
local count={clear=0}
local c={Find=function(self)return self end,SetImageColour=function()end,SetImageMetrics=function()end,StealShortcutKey=function()end}
local env={system={ClearRequiredFiles=function()count.clear=count.clear+1 end},
    UIComponent=function()return c end,Address=1234,FrontEnd={ScreenSize=function()return 1920,1080 end},
    require=function(name)assert(name=="Utilities");return {} end,package={path="",loadlib=package.loadlib},print=print}
setmetatable(env,{__index=_G})
local initial={};for key in pairs(env)do initial[key]=true end
local chunk=assert(loadfile(wrapper));setfenv(chunk,env);chunk("arg",nil,"last")
assert(count.clear==1,"Other UI initialization was lost or repeated")
assert(type(env.CompatibilityMenu168)=="function" and env.CompatibilityMenu168()=="other menu mod","Enabled UI callback was discarded")
local exported=0
for key,value in pairs(env)do if not initial[key]then
    assert(type(value)=="function" and getfenv(value)==env,"UI callback/environment changed");exported=exported+1
end end
assert(exported==38,"Stock or additional UI callbacks were lost")
print("PASS actual float-Lua bootstrap preserves all37 stock callbacks plus the enabled UI mod, environments and single initialization")

-- Appended AFTER the exact locally owned frontend script, in a PRIVATE probe.
-- No shared-module replacement and no change to existing UI callbacks.
do
    local function observe_possession_startup164()
        local folder=type(os)=="table" and type(os.getenv)=="function" and os.getenv("LOCALAPPDATA") or nil
        local function note(message)
            pcall(function()
                if type(print)=="function" then print("S2UP Workshop164: "..message) end
                if not folder or #folder==0 or type(io)~="table" or type(io.open)~="function" then return end
                local path=folder.."/S2UP-Workshop-Probe164-__NATIVE_SHA164__.lua.log"
                local previous=io.open(path,"rb")
                if previous then
                    local bytes=previous:read(32769);previous:close()
                    if bytes and #bytes>32768 then return end
                end
                local log=io.open(path,"ab")
                if log then log:write(message.."\n");log:close() end
            end)
        end
        note("script-entry; loadlib="..tostring(type(package)=="table" and type(package.loadlib) or "unavailable")..
            "; io.open="..tostring(type(io)=="table" and type(io.open) or "unavailable")..
            "; getenv="..tostring(type(os)=="table" and type(os.getenv) or "unavailable"))
        if type(io)~="table" or type(io.open)~="function" or
           type(os)~="table" or type(os.getenv)~="function" or
           type(package)~="table" or type(package.loadlib)~="function" then note("missing-capability");return end
        if not folder or #folder==0 then note("localappdata-unavailable");return end
        local payload=table.concat({
-- __NATIVE_PAYLOAD164__
        })
        local path=folder.."/S2UP-Workshop-Probe164-__NATIVE_SHA164__.dll"
        local file=io.open(path,"rb")
        if file then
            -- Bounded read; refuse foreign/truncated/extended files without replacing them.
            local actual=file:read(#payload+1);file:close()
            if actual~=payload then note("cache-conflict-preserved");return end
        else
            -- Lua has no exclusive-create mode. The content-addressed name and native
            -- probe only are suitable for this private experiment, not the final loader.
            file=io.open(path,"wb")
            if not file then note("cache-create-denied");return end
            local ok=file:write(payload);file:close()
            if not ok then note("cache-write-failed");return end
            file=io.open(path,"rb");if not file then note("cache-readback-failed");return end
            local actual=file:read(#payload+1);file:close()
            if actual~=payload then note("cache-readback-differs");return end
        end
        note("native-cache-verified")
        local entry,reason=package.loadlib(path,"luaopen_s2up_workshop_probe164")
        if type(entry)=="function" then
            local ok=pcall(entry)
            note(ok and "native-entry-called" or "native-entry-failed")
        else note("loadlib-failed: "..string.sub(tostring(reason),1,512)) end
    end
    -- A failed probe must not throw through the original menu script.
    pcall(observe_possession_startup164)
end

-- PRIVATE diagnostic wrapper. Original bytecode is taken from the owned game.
-- Keep its environment, arguments and exported callbacks; no CoreUtils override.
do
    local bytes=table.concat({
-- __ORIGINAL_CHUNK164__
    })
    local original,reason=loadstring(bytes,"@owned-game/frontend-root")
    assert(original,reason)
    setfenv(original,getfenv(1))
    original(...)
end
-- __PROBE_INITIALIZER164__

set_xmakever("2.9.4")

set_defaultmode("debug")
add_includedirs("src", "vendor")

add_rules("mode.debug", "mode.release")
set_allowedmodes("debug", "release")
add_links("d3d11","user32.lib", "dxgi", "dxguid", "comctl32.lib", "d3dcompiler.lib", "gdi32.lib", "kernel32.lib", "shell32.lib")
add_defines("UNICODE")
set_languages("cxx20", "c17")
set_warnings("all", "extra")
if(mode == "debug") then
    add_defines("DEBUG")
    set_optimize("none")

elseif(mode == "release") then
    add_defines("NDEBUG")
    set_policy("build.optimization.lto", true)
    set_optimize("fastest")
end

target("scene")
    set_default(true)
    set_kind("binary")
    add_files("scene.cpp", "main.cpp")
    add_headerfiles("scene.h")



includes("**/xmake.lua")
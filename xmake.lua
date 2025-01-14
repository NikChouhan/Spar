set_xmakever("2.9.4")
add_requires("stb",
{
    debug =	is_mode("debug"),
    configs =
    {
        shared = false,
        runtimes = "MTd",
    }
})

add_requires("libsdl",
{
    debug =	is_mode("debug"),
    configs =
    {
        shared = false,
        runtimes = "MTd",
    }
})

add_requires("imgui docking", 
{
    configs = { dx11 = true, sdl2 = true,  }
})


add_requires("cgltf",
{
    debug =	is_mode("debug"),
    configs =
    {
        shared = false,
        runtimes = "MTd",
    }
})


add_requires("directxtk")

set_defaultmode("debug")
add_includedirs("src", "vendor")

add_rules("mode.debug", "mode.release")
set_allowedmodes("debug", "release")
add_syslinks("d3d11","user32.lib", "dxgi", "dxguid", "comctl32.lib", "d3dcompiler.lib", "gdi32.lib", "kernel32.lib", "shell32.lib")
add_defines("UNICODE", "_UNICODE")

add_includedirs("src", "vendor", "src/Core", "src/Renderer")

set_languages("cxx20", "c17")
set_warnings("all", "extra")
if(is_mode("debug")) then
    set_symbols("debug")
    add_defines("DEBUG")
    set_optimize("none")

elseif(is_mode("release")) then
    add_defines("NDEBUG")
    set_policy("build.optimization.lto", true)
    set_optimize("fastest")
end

target("scene")
    set_default(true)
    set_kind("binary")
    add_files("Application.cpp", "main.cpp")
    add_headerfiles("Application.h")
    add_deps("core", "renderer")

    after_link(function (target) 
    print("Linking commands for " .. target:name())
end)
    -- on_run(function (target)
    --     os.execv("cmd", {"/c", "c:/dev/raddbg/raddbg.exe", target:installdir()})
    -- end)
target_end()

includes("**/xmake.lua")


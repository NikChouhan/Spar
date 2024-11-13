target("core")
    set_group("Spar")
    set_default(false)
    set_kind("static")
    add_files("Core/**.cpp")
    add_deps("stb")
    add_headerfiles("Core/**.h")
    add_includedirs("../vendor/stb")

target_end()

target("renderer")
    set_group("Spar")
    set_default(false)
    set_kind("static")
    add_files("Renderer/**.cpp")
    add_headerfiles("Renderer/**.h")
target_end()


target("Math")
    set_group("ext")
    set_default(false)
    set_kind("static")
    add_headerfiles("Math/**.h")
    add_includedirs("Math", {public = true})  -- Make Math headers public
    add_files("Math/**.cpp")
target_end()
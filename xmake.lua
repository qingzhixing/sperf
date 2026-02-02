set_targetdir("./build")
set_languages("c++23")

includes("testkit")

target("sperf")
    set_kind("binary")
    add_files("src/*.cpp")
    add_files("test/*.cpp")
    add_deps("testkit")
    add_includedirs("./include",{public = true})
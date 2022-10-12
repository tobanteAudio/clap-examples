from conans import ConanFile, CMake


class ClapExamples(ConanFile):
    name = "clap-examples"
    url = "https://github.com/tobanteAudio/clap-examples"
    description = "CLAP Audio-Plugin examples"
    license = "BSL-1.0"

    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake_find_package_multi", "markdown"

    def requirements(self):
        self.requires("mc-core/0.12.0@modern-circuits/stable")
        self.requires("imgui/cci.20220621+1.88.docking")
        if self.settings.os != "Emscripten":
            self.requires("cairo/1.17.4")
            self.requires("glfw/3.3.6")
            self.requires("glew/2.2.0")
            self.requires("sdl/2.24.0")

    def configure(self):
        self.options["imgui"].shared = False
        if self.settings.os != "Emscripten":
            self.options["cairo"].shared = False
            self.options["glfw"].shared = False
            self.options["glew"].shared = False

    def imports(self):
        src = "res/bindings"
        dest = "bindings"

        self.copy("imgui_impl_opengl3.h", dst=dest, src=src)
        self.copy("imgui_impl_opengl3.cpp", dst=dest, src=src)
        self.copy("imgui_impl_opengl3_loader.h", dst=dest, src=src)

        if self.settings.os == "Emscripten":
            self.copy("imgui_impl_sdl.h", dst=dest, src=src)
            self.copy("imgui_impl_sdl.cpp", dst=dest, src=src)
        else:
            self.copy("imgui_impl_glfw.h", dst=dest, src=src)
            self.copy("imgui_impl_glfw.cpp", dst=dest, src=src)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

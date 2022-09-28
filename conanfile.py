from conans import ConanFile, CMake


class ClapExamples(ConanFile):
    name = "clap-examples"
    url = "https://github.com/tobanteAudio/clap-examples"
    description = "CLAP Audio-Plugin examples"
    license = "BSL-1.0"

    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake_find_package_multi", "markdown"

    def requirements(self):
        self.requires("imgui/cci.20220621+1.88.docking")
        if self.settings.os != "Emscripten":
            self.requires("glfw/3.3.6")
            self.requires("glew/2.2.0")

    def configure(self):
        if self.settings.os != "Emscripten":
            self.options["glew"].shared = False

    def imports(self):
        src = "res/bindings"
        dest = "bindings"

        if self.settings.os != "Emscripten":
            self.copy("imgui_impl_glfw.h", dst=dest, src=src)
            self.copy("imgui_impl_glfw.cpp", dst=dest, src=src)
            self.copy("imgui_impl_opengl3.h", dst=dest, src=src)
            self.copy("imgui_impl_opengl3.cpp", dst=dest, src=src)
            self.copy("imgui_impl_opengl3_loader.h", dst=dest, src=src)
        else:
            self.copy("imgui_impl_sdl.h", dst=dest, src=src)
            self.copy("imgui_impl_sdl.cpp", dst=dest, src=src)

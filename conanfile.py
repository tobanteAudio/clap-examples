import os

from conan import ConanFile
from conan.tools.cmake import cmake_layout
from conan.tools.files import copy


class ImGuiExample(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        self.requires("cairo/1.18.0")
        # self.requires("imgui/1.89.7-docking")

        if self.settings.os != "Emscripten":
            self.requires("glew/2.2.0")
            self.requires("glfw/3.4")
            if self.settings.os != "Macos":
                    self.requires("sdl/2.28.5")

    def configure(self):
        # self.options["imgui"].shared = False
        self.options["cairo"].shared = False
        self.options["cairo"].with_fontconfig = False
        self.options["cairo"].with_glib = False
        self.options["freetype"].with_png = False

        if self.settings.os != "Emscripten":
            self.options["glew"].shared = False
            self.options["glfw"].shared = False

        if self.settings.os == "Linux":
            self.options["sdl"].alsa = False
            self.options["sdl"].pulse = False
            self.options["sdl"].vulkan = False

        if self.settings.os == "Windows":
            self.options["sdl"].iconv = False


    def generate(self):
        pass
        # src = os.path.join(self.dependencies["imgui"].package_folder, "res", "bindings")
        # dest = os.path.join(self.source_folder, "bindings")

        # copy(self, "*opengl3*", src, dest)
        # if self.settings.os == "Emscripten":
        #     copy(self, "*sdl*", src, dest)
        # else:
        #     copy(self, "*glfw*", src, dest)

    def layout(self):
        cmake_layout(self)

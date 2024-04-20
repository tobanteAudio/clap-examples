import os

from conan import ConanFile
from conan.tools.cmake import cmake_layout
from conan.tools.files import copy


class ImGuiExample(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        self.requires("cairo/1.18.0")
        self.requires("imgui/1.89.7-docking")
        self.requires("glew/2.2.0")
        self.requires("glfw/3.4")

    def configure(self):
        self.options["imgui"].shared = False
        self.options["cairo"].shared = False
        self.options["cairo"].with_fontconfig = False
        self.options["cairo"].with_glib = False
        self.options["glew"].shared = False
        self.options["glfw"].shared = False

    def generate(self):
        copy(self, "*glfw*", os.path.join(self.dependencies["imgui"].package_folder,
             "res", "bindings"), os.path.join(self.source_folder, "bindings"))
        copy(self, "*opengl3*", os.path.join(self.dependencies["imgui"].package_folder,
             "res", "bindings"), os.path.join(self.source_folder, "bindings"))

    def layout(self):
        cmake_layout(self)

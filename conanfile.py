from conan import ConanFile
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout

class blocks(ConanFile):
    settings = 'os', 'compiler', 'build_type', 'arch'

    def requirements(self):
        self.requires("sfml/2.6.2")
        self.requires("box2d/2.4.2")

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
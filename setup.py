# from setuptools import setup, Extension
# from cmake_build import CMakeExtension, BuildCMakeExtension

# setup(
#     name='lele',
#     version='1.0',
#     description='This is a demo package',
#     ext_modules=[
#     CMakeExtension(name="lele_module", install_prefix="lele_python_package", source_dir="src/cpp_project")
#     ],
#     cmdclass={
#         "build_ext": BuildCMakeExtension,
#     },
# )
###############

from setuptools import setup
# from cmake_build import CMakeExtension, BuildCMakeExtension
# from cmake_build_extension import CMakeBuild, CMakeExtension
from cmake_build_extension import BuildExtension, CMakeExtension, CMakeBuild

setup(
    # ... other setup arguments ...
    cmdclass={"build_ext": CMakeBuild},
    ext_modules=[
        CMakeBuild(
            name="lele",  # The name of your Python module
            source_dir="./", # Path to your CMakeLists.txt
            cmake_args=[
                "-G Ninja",
                "-DCMAKE_PREFIX_PATH=${cwd}/cmake",
                "-DLV_USE_WAYLAND=1",
                "-DCMAKE_BUILD_TYPE=Debug"], # Optional CMake arguments
            # ... other CMakeExtension arguments like build_args, install_args ...
        ),
    ],
)
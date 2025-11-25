from setuptools import setup, Extension
from cmake_build import CMakeExtension, BuildCMakeExtension

setup(
    name='lele',
    version='1.0',
    description='This is a demo package',
    ext_modules=[
    CMakeExtension(name="lele_module", install_prefix="lele_python_package", source_dir="src/cpp_project")
    ],
    cmdclass={
        "build_ext": BuildCMakeExtension,
    },
)

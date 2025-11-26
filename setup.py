from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import distutils.ccompiler
from pathlib import Path
import os

cpp_compile_args=[]
# cpp_compile_args=['-std=c++17']
c_compile_args=['-std=c99']
class custom_build_ext(build_ext):
    def build_extension(self, ext):
        # Store original flags
        original_extra_compile_args = list(ext.extra_compile_args)
        
        # Iterate over source files to check for specific flags
        for source in ext.sources:
            # Get the base filename for mapping
            filename = os.path.basename(source)
            # filetype = os.path.extension(source)
            root, extension = os.path.splitext(source)
            # print(f"@@@ filename:{filename}, extension:{extension}")
            
            # Apply specific flags if found in the mapping
            if extension == '.c':
                ext.extra_compile_args = c_compile_args
                # print(f"Applying flags {c_compile_args} to {filename}")
            else:
                # print(f"Applying flags {original_extra_compile_args} to {filename}")
                ext.extra_compile_args = original_extra_compile_args
            
            # Compile the file using the parent method
            # The 'sources' list is temporarily modified to contain only the current file
            # for the parent build_extension method to process it individually.
            original_sources = list(ext.sources)
            ext.sources = [source]
            build_ext.build_extension(self, ext)
            ext.sources = original_sources
            
        # Restore original flags and sources (though not strictly necessary here, good practice)
        ext.extra_compile_args = original_extra_compile_args


setup(name='lele',
      version='1.0',
      description='This is a demo package',
      ext_modules=[
        Extension('lele',
            sources=[
                'src/python_binding.cpp','src/graphics_backend.cpp','src/lelewidgets/lelebase.cpp','src/lelewidgets/lelebutton.cpp','src/lelewidgets/leleevent.cpp','src/lelewidgets/leleimage.cpp','src/lelewidgets/lelelabel.cpp','src/lelewidgets/lelenullwidget.cpp','src/lelewidgets/lelestackview.cpp','src/lelewidgets/lelestyle.cpp','src/lelewidgets/leletabview.cpp','src/lelewidgets/leletextbox.cpp','src/lelewidgets/leleview.cpp','src/lelewidgets/lelemessagebox.cpp','src/lelewidgets/lelewidgetfactory.cpp','src/lib/driver_backends.c','src/lib/mouse_cursor_icon.c','src/lib/simulator_util.c','src/lib/indev_backends/evdev.c','src/lib/display_backends/wayland.c','/repos/lv_port_linux/wl_protocols/wayland_xdg_shell.c','/repos/lv_port_linux/x86-build/res/img_dsc.cpp'
            ],
            include_dirs=['.','x86-build','src', 'src/Python-3.13.3', 'src/Python-3.13.3/Include'],  # Directories to search for headers
            library_dirs=['src','x86-build/src/Python-3.13.3'],  # Directories to search for libraries
            libraries=['lvgl_linux','lvgl','m','pthread','evdev','wayland-client','wayland-cursor','xkbcommon','utils','image_converter','python3.13','crypt','pthread','dl','util','m'],  # Names of libraries to link (e.g., 'libsomething.a' becomes 'something')
            define_macros=[('LV_CONF_INCLUDE_SIMPLE', '1')], # List of compile definitions
            extra_compile_args=cpp_compile_args, # Additional compiler flags (optional)
            language='c++' # Specify the language as C++
        )
      ]
    #   ,cmdclass={'build_ext': custom_build_ext} 
    )
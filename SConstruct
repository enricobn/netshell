import os
# Tell SCons to create our build files in the 'build' directory
VariantDir('build', 'src', duplicate=0)

# Set our source files
source_files = Glob('build/*.cpp', 'build/*.h')

# Set our required libraries
libraries = ['pthread','ssl','crypto']
#libraries = []
library_paths = ''

env = Environment()

subpackages = ['service', 'http', 'terminal']

for package in subpackages:
    pack_objects = env.SConscript(os.path.join('build', package, 'SConscript'), exports = 'env')
    source_files.extend(pack_objects)   

# Set our g++ compiler flags
env.Append( CPPFLAGS=['-std=c++0x', '-pthread', '-lssl', '-lcrypto', '-Wall', '-g'] )

# Tell SCons the program to build
env.Program('build/netshell', source_files, LIBS = libraries, LIBPATH = library_paths)

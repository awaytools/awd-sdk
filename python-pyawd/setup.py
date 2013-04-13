#!/usr/bin/env python

import os
import sys
import getopt
from distutils.core import setup, Extension

version_macro = ('PYTHON_VERSION', sys.version_info[0])

source_files = [
    'src/util.cc',
    'src/bcache.cc',
    'src/prepgeom.cc',
    'src/prepscene.cc',
    'src/prepmaterial.cc',
    'src/prepanim.cc',
    'src/prepattr.cc',
    'src/cpyawd.cc',
    'src/AWDWriter.cc',
    'src/utils/build_geom.cc'
]

def parse_bool(str):
    if str=='0' or str.lower()=='false':
        return False
    else:
        return True

use_libawd = True
opts, args = getopt.getopt(sys.argv[1:], None, ['use-libawd='])
for opt in opts:
    if opt[0] == '--use-libawd':
        use_libawd = parse_bool(opt[1])

ext_modules = []
if use_libawd:
    ext_modules.append(Extension('pyawd.cpyawd', 
        libraries = [ 'awd' ],
        include_dirs = [ 'include', '/usr/local/include' ],
        define_macros = [ version_macro ],
        sources = source_files ))

setup(name = 'pyawd',
    version = '0.1.0',
    description = 'Python package for dealing with Away3D AWD files',
    ext_modules = ext_modules,
    package_dir = { 'pyawd': 'src/pyawd', 'pyawd.utils': 'src/pyawd/utils' },
    packages = [ 'pyawd', 'pyawd.utils' ]
    )


from glob import glob
from setuptools import setup, Extension
from distutils.command.build import build as build_orig
from setuptools.command.build_ext import build_ext as build_ext_orig
import itertools
import os
import subprocess

# set things up
this_dir = os.path.dirname(os.path.abspath(__file__))
compact_tree_h = os.path.join(this_dir, 'CompactTree', 'compact_tree.h')
compact_tree_version = [l for l in open(compact_tree_h) if l.strip().startswith('#define COMPACTTREE_VERSION')][0].split()[-1].replace('"','')
try:
    python_include_dir = list(glob('/usr/include/python3*'))[0]
except:
    raise RuntimeError("Unable to find python3* folder in /usr/include")

# fix pip install
def partition(pred, iterable):
    t1, t2 = itertools.tee(iterable)
    return itertools.filterfalse(pred, t1), filter(pred, t2)
class build(build_orig):
    def finalize_options(self):
        super().finalize_options()
        condition = lambda el: el[0] == 'build_ext'
        rest, sub_build_ext = partition(condition, self.sub_commands)
        self.sub_commands[:] = list(sub_build_ext) + list(rest)

# run SWIG and compile
class build_ext(build_ext_orig):
    def run(self):
        subprocess.check_call(['swig', '-c++', '-python', 'CompactTree/compact_tree.i'])
        subprocess.check_call(['g++', '-fpic', '-c', 'CompactTree/compact_tree.h', 'CompactTree/compact_tree_wrap.cxx', '-I%s' % python_include_dir])
        subprocess.check_call(['g++', '-shared', 'compact_tree_wrap.o', '-o', 'CompactTree/_compact_tree.so', '-lstdc++'])
        build_ext_orig.run(self)

# package info
setup(
    name='CompactTree',
    version=compact_tree_version,
    author='Niema Moshiri',
    author_email='niemamoshiri@gmail.com',
    description='CompactTree: Lightweight header-only C++ library for loading and traversing trees',
    long_description='CompactTree is a header-only C++ library for loading and traversing trees.',
    long_description_content_type='text/markdown',
    url='https://github.com/niemasd/CompactTree',
    classifiers=[
        'Development Status :: 3 - Alpha',
        'Intended Audience :: Developers',
        'Topic :: Software Development :: Build Tools',
        'License :: OSI Approved :: GNU General Public License v3 or later (GPLv3+)',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.7',
        'Programming Language :: Python :: 3.8',
        'Programming Language :: Python :: 3.9',
        'Programming Language :: Python :: 3.10',
        'Programming Language :: Python :: 3.11',
    ],
    keywords='tree phylogenetics fast',
    packages=['CompactTree'],
    ext_modules=[Extension('_compact_tree', sources=['CompactTree/compact_tree_wrap.cxx'])],
    cmdclass={'build_ext': build_ext, 'build': build},
    include_package_data=True,
    package_data={
        'CompactTree': ['_compact_tree.so', 'compact_tree.py'],
    },
    zip_safe=False,
    install_requires=[],
)

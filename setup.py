from glob import glob
from setuptools import setup, Extension, find_packages
from setuptools.command.build_ext import build_ext
import os
import shutil
import subprocess

this_dir = os.path.dirname(os.path.abspath(__file__))
compact_tree_h = os.path.join(this_dir, 'compact_tree.h')
compact_tree_version = [l for l in open(compact_tree_h) if l.strip().startswith('#define COMPACTTREE_VERSION')][0].split()[-1].replace('"','')

try:
    python_include_dir = list(glob('/usr/include/python3*'))[0]
except:
    raise RuntimeError("Unable to find python3* folder in /usr/include")

class BuildSwigExtension(build_ext):
    def run(self):
        os.makedirs(self.build_temp, exist_ok=True)
        shutil.copy2(compact_tree_h, self.build_temp)
        subprocess.check_call([
            'swig', '-c++', '-python',
            '-outdir', self.build_temp,
            '-o', os.path.join(self.build_temp, 'compact_tree_wrap.cxx'),
            os.path.join(this_dir, 'compact_tree.i')
        ])
        shutil.copy2(os.path.join(self.build_temp, 'compact_tree.py'), os.path.join(this_dir, 'compact_tree'))
        super().run()

    def build_extensions(self):
        for ext in self.extensions:
            ext.sources = [os.path.join(self.build_temp, 'compact_tree_wrap.cxx')] + ext.sources
            ext.include_dirs += [python_include_dir, self.build_temp]
        super().build_extensions()

setup(
    name='compact_tree',
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
    packages=find_packages(exclude=['doxygen-awesome-css', 'example']),
    ext_modules=[
        Extension(
            name='_compact_tree',
            sources=[],  # Sources will be set during build_ext run
            include_dirs=[python_include_dir, this_dir],
            language='c++',
        ),
    ],
    cmdclass={
        'build_ext': BuildSwigExtension,
    },
    py_modules=["compact_tree"],
    zip_safe=False,
)

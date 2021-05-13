from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext

source = [
  #"./main.cpp",
  "./main.pyx"
  ]

setup(
  #name = "gamelib",
  cmdclass = dict(build_ext = build_ext),
  ext_modules = [
    Extension(
      "gamelib",
      source,
      language = "c++",
    )
  ]
)
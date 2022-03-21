from conans import ConanFile, CMake

class ConanTest (ConanFile):
  name = 'Conan Test'
  version = '1.0'
  settings = 'os', 'compiler', 'build_type', 'arch'
  generators = 'cmake', 'cmake_find_package'
  requires = [
    ('boost/1.78.0'), 
    ('openssl/1.1.1m')
  ]

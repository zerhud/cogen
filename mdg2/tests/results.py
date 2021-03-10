import subprocess as sp
import shutil
import os

test_dir = 'test_dir'
mdg_run = ['./mdgwr.py', '-iix3=ix3_example', '-Ietc/mdg2/examples/incs']
cpp_decl = ['-gcpp_decl']
cpp_py = ['-gcpp_py']

def setup_function():
    if os.path.exists(test_dir):
        shutil.rmtree(test_dir)

def teardown_function():
    if os.path.exists(test_dir):
        #shutil.rmtree(test_dir)
        pass

def resource(file):
    return os.path.dirname(os.path.abspath(__file__)) + '/resources/' + file

def copy_resource_to_test(src, dest=''):
    if dest == '': dest = src
    shutil.copyfile(resource(src), test_dir + '/' + dest)

def test_cpp_decl():
    build_dir = test_dir + '/build'
    r_mdg = sp.run(mdg_run + cpp_decl + ['-o', test_dir])
    os.mkdir(build_dir)
    r_cmake = sp.run(['cmake', '..'], cwd = build_dir)
    r_make = sp.run(['cmake', '--build', '.'], cwd = build_dir)
    assert r_make.returncode == 0
    assert os.path.exists(build_dir + '/libinterface.a')

def test_cpp_py():
    build_dir = test_dir + '/build'
    py_dir = test_dir + '/gen'
    r_mdg = sp.run(mdg_run + cpp_py + ['-o', py_dir])
    os.mkdir(build_dir)
    copy_resource_to_test('exmp_mod.cpp')
    copy_resource_to_test('exmp_combine_module.cpp')
    copy_resource_to_test('exmp_external_module.cpp')
    copy_resource_to_test('exmp_cmake.cmake', 'CMakeLists.txt')
    r_cmake = sp.run(['cmake', '..'], cwd = build_dir)
    r_make = sp.run(['cmake', '--build', '.'], cwd = build_dir)
    assert r_make.returncode == 0


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
    r_mdg = sp.run(mdg_run + cpp_py + ['-o', test_dir])
    os.mkdir(build_dir)
    r_cmake = sp.run(['cmake', '../gen'], cwd = build_dir)
    r_make = sp.run(['cmake', '--build', '.'], cwd = build_dir)
    assert r_make.returncode == 0


import subprocess as sp
import shutil
import os

test_dir = 'test_dir'
cogen_run = ['./cogenwr.py', '-iix3=ix3_example', '-Ietc/cogen/examples/incs']
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
    r_cogen = sp.run(cogen_run + cpp_decl + ['-o', test_dir])
    os.mkdir(build_dir)
    r_cmake = sp.run(['cmake', '..'], cwd = build_dir)
    r_make = sp.run(['cmake', '--build', '.'], cwd = build_dir)
    assert r_make.returncode == 0
    assert os.path.exists(build_dir + '/libinterface.a')

def test_cpp_py():
    build_dir = test_dir + '/build'
    py_dir = test_dir + '/gen'
    r_cogen = sp.run(cogen_run + cpp_py + ['-o', py_dir])
    os.mkdir(build_dir)
    copy_resource_to_test('exmp_mod.cpp')
    copy_resource_to_test('exmp_combine_module.cpp')
    copy_resource_to_test('exmp_external_module.cpp')
    copy_resource_to_test('exmp_cmake.cmake', 'CMakeLists.txt')
    copy_resource_to_test('__init__.py', '__init__.py')
    r_cmake = sp.run(['cmake', '..'], cwd = build_dir)
    r_make = sp.run(['cmake', '--build', '.', '-j4'], cwd = build_dir)
    assert r_make.returncode == 0

    from test_dir.build import mod
    from test_dir.build import combine_module
    from test_dir.build import external_module

    assert combine_module.combine([external_module.ExternalEnum.ex_two, external_module.ExternalEnum.ex_one]) == "ex_two ex_one "

    ex_vd = external_module.VariusData()
    external_module.fillData(ex_vd, "test msg")
    assert ex_vd.message == "test msg"

    assert external_module.genStr(external_module.ExternalEnum.ex_one) == "gen_one"

    class EchoCallback(mod.EchoCallback):
        def __init__(self):
            super(EchoCallback, self).__init__()
            self.count = 0
        def onMessage(self, msg):
            self.count += 1
            assert msg == "test"
    cb = EchoCallback()
    srv = mod.CreateEchoService(10, cb)
    msg = mod.EchoMesage()
    msg.msg = "test"
    srv.generate(msg)
    assert cb.count == 1


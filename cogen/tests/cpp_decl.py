import subprocess as sp
import json

ix3_example_opts = ['-iix3=ix3_example', '-gcpp_decl', '-Ietc/cogen/examples/incs']

def check_example_json(result, tmpl, out):
    assert 'file' in result[0]
    assert 'out_file' in result[0]
    assert 'data' in result[0]
    assert tmpl in result[0]['file']
    assert result[0]['out_file'] == out

def check_example_data(result, count=3):
    assert b'"vector"' in result
    assert b'"string"' in result
    assert b'"cinttypes"' in result
    assert b'std::int64_t' in result
    assert b'std::pmr::vector' in result
    assert b'std::pmr::string' in result
    parsed = json.loads(result)
    assert len(parsed[0]['data']['data']) == count
    data = parsed[0]['data']['data']
    assert 'ix3' in data

def test_pathes():
    r=sp.run(['./cogen', '--which_g', 'cpp_decl.info'], stdout=sp.PIPE, stderr=sp.PIPE)
    assert b'cogen/etc/cogen/generators/cpp_decl.info' in r.stdout;
    r=sp.run(['./cogen', '--which_t', 'cpp/render.jinja'], stdout=sp.PIPE, stderr=sp.PIPE)
    assert b'cogen/etc/cogen/lib/cpp/render.jinja' in r.stdout;

def test_wrong_mode():
    r=sp.run(['./cogen', '-m', 'wrong'] + ix3_example_opts, stdout=sp.PIPE, stderr=sp.PIPE)
    assert r.stdout == b'';
    assert r.stderr == b'wrong generation mode "wrong"\n'

def test_flist_mode():
    r=sp.run(['./cogen', '-m', 'flist', '-o', 'outdir'] + ix3_example_opts, stdout=sp.PIPE, stderr=sp.PIPE)
    assert b'declarations.hpp\nmodule.hpp\nmodule.cpp\nCMakeLists.txt' == r.stdout;
    r=sp.run(['./cogen', '-m', 'flist;', '-o', 'outdir'] + ix3_example_opts, stdout=sp.PIPE, stderr=sp.PIPE)
    assert b'declarations.hpp;module.hpp;module.cpp;CMakeLists.txt' == r.stdout;

def test_simple_config():
    r=sp.run(['./cogen', '-m', 'json', '-o', 'outdir'] + ix3_example_opts, stdout=sp.PIPE, stderr=sp.PIPE)
    assert r.stderr == b''
    result = json.loads(r.stdout)
    assert len(result) == 4
    check_example_json(result, 'declarations.jinja', 'outdir/declarations.hpp')
    check_example_data(r.stdout)


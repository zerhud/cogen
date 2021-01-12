import subprocess as sp
import json

def check_example_json(result, tmpl, out):
    assert 'file' in result[0]
    assert 'out_file' in result[0]
    assert 'data' in result[0]
    assert tmpl in result[0]['file']
    assert result[0]['out_file'] == out

def check_example_data(result, count=2):
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

def test_wrong_mode():
    r=sp.run(['./mdg2', '-m', 'wrong', '-iix3=ix3_example', '-gcpp_decl'], stdout=sp.PIPE, stderr=sp.PIPE)
    assert r.stdout == b'';
    assert r.stderr == b'wrong generation mode "wrong"\n'

def test_no_dir_yet():
    r=sp.run(['./mdg2', '-m', 'dir', '-iix3=ix3_example', '-gcpp_decl'], stdout=sp.PIPE, stderr=sp.PIPE)
    assert r.stdout == b'';
    assert r.stderr == b'dir mode are not ready yet\n'

def test_simple_config():
    r=sp.run(['./mdg2', '-m', 'json', '-o', 'outdir', '-iix3=ix3_example', '-gcpp_decl'], stdout=sp.PIPE, stderr=sp.PIPE)
    assert r.stderr == b''
    result = json.loads(r.stdout)
    assert len(result) == 4
    check_example_json(result, 'declarations.jinja', 'outdir/declarations.hpp')
    check_example_data(r.stdout)


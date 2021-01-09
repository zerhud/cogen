import subprocess as sp
import json

def check_example_json(result, tmpl, out):
    assert 'file' in result[0]
    assert 'out_file' in result[0]
    assert 'data' in result[0]
    assert result[0]['file'] == tmpl
    assert result[0]['out_file'] == out

def check_example_data(result, count=1):
    assert "\"vector\"" in result
    assert "\"string\"" in result
    assert "\"cinttypes\"" in result
    assert "std::int64_t" in result
    assert "std::vector<std::string>" in result
    parsed = json.loads(result)
    assert len(parsed[0]['data']['data']) == count
    data = parsed[0]['data']['data']
    assert data[0]['name']=='ix3'
    assert len(data[0]['mods'])==1

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


import subprocess as sp
import json

def check_example_json(result, tmpl, out):
    assert 'file' in result[0]
    assert 'out_file' in result[0]
    assert 'data' in result[0]
    assert result[0]['file'] == tmpl
    assert result[0]['out_file'] == out
    assert len(result[0]['data']) == 1
    assert result[0]['data'][0]['name']=='ix3'
    assert len(result[0]['data'][0]['mods'])==1

def test_wrong_mode():
    r=sp.run(['./mdg2', '-m', 'wrong', '-iix3=ix3_example'], stdout=sp.PIPE, stderr=sp.PIPE)
    assert r.stdout == b'';
    assert r.stderr == b'wrong generation mode "wrong"\n'

def test_no_dir_yet():
    r=sp.run(['./mdg2', '-m', 'dir', '-iix3=ix3_example'], stdout=sp.PIPE, stderr=sp.PIPE)
    assert r.stdout == b'';
    assert r.stderr == b'dir mode are not ready yet\n'

def test_no_config():
    r=sp.run(['./mdg2', '-m', 'json', '-iix3=ix3_example'], stdout=sp.PIPE, stderr=sp.PIPE)
    assert r.stderr == b''
    result = json.loads(r.stdout)
    assert len(result) == 1
    check_example_json(result, '', '')

def test_simple_config():
    r=sp.run(['./mdg2', '-m', 'json', '-o', 'outdir', '-iix3=ix3_example', '-gsimple_example'], stdout=sp.PIPE, stderr=sp.PIPE)
    assert r.stderr == b''
    result = json.loads(r.stdout)
    assert len(result) == 1
    check_example_json(result, 'template_file.in', 'outdir/output_file.h')


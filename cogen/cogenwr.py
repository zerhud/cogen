#!/usr/bin/env python3

import os
import sys
import json
import jinja2
import subprocess as sp

current_template_place = []

def template_loader(name):
    for place in current_template_place:
        place = os.path.abspath(place).decode("utf-8")
        print('serach ' + name + ' in ' + place)
        cur_path = os.path.join(place, name)
        if os.path.isfile(cur_path):
            return open(cur_path).read()
    return None

def extract_name(obj, ind=-1):
    if isinstance(obj['name'], list):
        return obj['name'][ind if 0 < ind else 0]
    return obj['name']

def apply_sufix(obj, sufix, nind=0):
    naming = obj['naming'][nind]
    if naming == "underscore" or naming == "as_is":
        return extract_name(obj,nind) + '_' + sufix
    if naming == "title_case" or naming == "camel_case":
        return extract_name(obj,nind) + sufix.title()
    return extract_name(obj,nind) + sufix

def apply_prefix(obj, prefix, nind=0):
    naming = obj['naming'][nind]
    if naming == "underscore": return prefix + '_' + extract_name(obj,nind)
    if naming == "title_case": return prefix.title() + extract_name(obj,nind)
    if naming == "camel_case":
        return prefix.title() + extract_name(obj,nind)[:1].upper() + extract_name(obj,nind)[1:]
    return prefix + extract_name(obj,nind)

def unique(objs):
    ret = []
    for obj in objs:
        if obj not in ret:
            ret.append(obj)
    return ret

def select_keys(objs, vals):
    ret = []
    for key, val in objs:
        if key not in vals: continue
        if(type(val) == type([])):
            for v in val: ret.append(v)
        else:
            ret.append(val)
    return ret

jinja_env = jinja2.Environment(
    block_start_string = '<%',
    block_end_string = '%>',
    variable_start_string = '<=',
    variable_end_string = '=>',
    comment_start_string = '<*',
    comment_end_string = '*>',
	loader = jinja2.ChoiceLoader( [
		jinja2.FunctionLoader( template_loader ),
		jinja2.FileSystemLoader(os.path.abspath('.'), followlinks=True),
		jinja2.FileSystemLoader('${templates_dir_path}/', followlinks=True),
	] )
)

jinja_env.filters["apply_sufix"] = apply_sufix
jinja_env.filters["apply_prefix"] = apply_prefix
jinja_env.filters["unique"] = unique
jinja_env.filters["name"] = extract_name
jinja_env.filters["select_keys"] = select_keys

def gen_file(src, to, data):
    print(src + "\t → \t" + to)
    to_dir = os.path.abspath(os.path.dirname(to))
    os.makedirs(to_dir, exist_ok=True)
    tmpl = jinja_env.from_string(open(src).read())
    result = tmpl.render(cdata = data)
    if(to == '-'):
        print(result)
    else:
        open(to, 'w').write(result)

def get_path(main_file, which):
    input = sp.run([main_file, '--help_pathes={}'.format(which)], stdout=sp.PIPE)
    input_lines = input.stdout.splitlines()
    input_lines.pop(0)
    for line in input_lines:
        current_template_place.append(line[2:-1])

def get_pathes(main_file):
    get_path(main_file, 'input')
    get_path(main_file, 'libraries')

if __name__ == "__main__":
    self_path = os.path.dirname(os.path.realpath(__file__))
    print('self path ' + self_path)
    main_file = os.path.join(self_path, "cogen")
    get_pathes(main_file)
    cogen = sp.run([main_file, '-m', 'json'] + sys.argv, stdout=sp.PIPE)
    parsed = json.loads(cogen.stdout)
    for p in parsed:
        gen_file(p['file'], p['out_file'], p['data'])

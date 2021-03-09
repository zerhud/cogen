#!/usr/bin/env python3

import os
import sys
import json
import jinja2
import subprocess as sp

current_template_place = './etc/mdg2/lib/'

def template_loader(name):
    print('serach ' + name + ' in ' + os.path.abspath(current_template_place))
    cur_path = os.path.join(current_template_place, name)
    if os.path.isfile(cur_path):
            return open(cur_path).read()
    return None

def extract_name(obj, ind):
    if isinstance(obj['name'], list):
        return obj['name'][ind]
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

if __name__ == "__main__":
    self_path = os.path.dirname(os.path.realpath(__file__))
    current_template_place = self_path + '/' + current_template_place
    print('self path ' + self_path)
    main_file = os.path.join(self_path, "mdg2")
    mdg = sp.run([main_file, '-m', 'json'] + sys.argv, stdout=sp.PIPE)
    parsed = json.loads(mdg.stdout)
    for p in parsed:
        gen_file(p['file'], p['out_file'], p['data'])

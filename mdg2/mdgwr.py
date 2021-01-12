#!/usr/bin/env python3

import os
import sys
import json
import jinja2
import subprocess as sp

current_template_place = '.'

def template_loader(name):
	cur_path = os.path.join(current_template_place, name)
	if os.path.isfile(cur_path):
		return open(cur_path).read()
	return None

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

def gen_file(src, to, data):
    print("generate file")
    print("from " + src)
    print("to " + to)
    tmpl = jinja_env.from_string(open(src).read())
    result = tmpl.render(cdata = data)
    if(to == '-'):
        print(result)
    else:
        open(to, 'w').write(result)

if __name__ == "__main__":
    main_file = os.path.join(os.path.dirname(os.path.realpath(__file__)), "mdg2")
    mdg = sp.run([main_file, '-m', 'json'] + sys.argv, stdout=sp.PIPE)
    parsed = json.loads(mdg.stdout)
    for p in parsed:
        gen_file(p['file'], p['out_file'], p['data'])


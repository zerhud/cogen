import sys
import json
print("render ", template_file, " into ", output_file)

jinja_env.loader = jinja2.FileSystemLoader(os.path.dirname(template_file))
# jinja cannot load file if tmp_file contains path :(
#template = jinja_env.get_template(tmpl_file)
template = jinja_env.from_string(open(template_file).read())

tmpl_data = json.loads(template_str_data)
rendered_template = template.render(cdata = tmpl_data)

if(output_file == '-'):
	print(rendered_template)
else:
	open(output_file, 'w').write(rendered_template)


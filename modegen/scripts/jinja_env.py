import os
import jinja2

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


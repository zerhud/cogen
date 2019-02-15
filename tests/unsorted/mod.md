module notused v0.0:

string some_function();

module mod v1.0:

string zz_not_used();

list<i8> function();

interface inter {
	string name() const ;
	rec by_name(string name) const ;
	void add(rec i) mutable ;
}

record rec {
	string name;
	string value;
}

enum +auto_io e1 { one two }

interface UglyInterface {
	constructor(string param);
	string getParam() const ;
}

module client v2.3:
interface client_interface {
	string entry() const ;
	@v2.4 string new_item() const ;
}


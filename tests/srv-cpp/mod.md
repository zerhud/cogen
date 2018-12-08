module mod v1.0:

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

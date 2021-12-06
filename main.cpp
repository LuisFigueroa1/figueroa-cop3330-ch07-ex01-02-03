/*
	calculator08buggy.cpp | From: Programming -- Principles and Practice Using C++, by Bjarne Stroustrup
	We have inserted 3 bugs that the compiler will catch and 3 that it won't.
*/

//modified for making the buggy calculator work and adding the exercises 01, 02 and 03.

#include "std_lib_facilities.h"

//moved the const declaration to the top
const char let = 'L';
const char quit = 'Q';
const char print = ';';
const char number = '8';
const char name = 'a';
const char constant  = 'C';    //declaration for constant
const string declare = "let";
const string declareConst = "const";

//changed the "struct" to "class" and made it public.
class Token {
public:
	char kind;
	double value;
	string name;
	Token(char ch) :kind(ch), value(0) { }
	Token(char ch, double val) :kind(ch), value(val) { }
	//add the token for the name string n
	Token(char ch, string n) :kind(ch), name(n) { }
};

//define the private part of the class
class Token_stream {
private:
	bool full;
	Token buffer;
public:
	Token_stream(); //got rid of the initializations declared here

	Token get();
	void unget(Token t); //got rid of the initializations declared here

	void ignore(char c); //I put the char c from the function below
};

//buffer is empty
Token_stream::Token_stream()
:full(false), buffer(0){}

//unget
void Token_stream::unget(Token t){
	if (full) error("unget() buffer is full");
	buffer = t;
	full = true;
}

Token Token_stream::get()
{
	if (full) { full = false; return buffer; }

	char ch;
	cin >> ch;
	switch (ch) {
	case quit: //added this switch cases for chars declared earlier
	case print: //added this switch cases for chars declared earlier
	case '(':
	case ')':
	case '+':
	case '-':
	case '*':
	case '/':
	case '%':
	case '=':
		return Token(ch);
	case '.':
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	{	cin.unget();//added ch
	double val;
	cin >> val;
	return Token(number, val);
	}
	default:
		if (isalpha(ch)) {
			string s;
			s += ch;
			//add the underscore for ex 01
			while (cin.get(ch) && (isalpha(ch) || isdigit(ch) || ch=='_')) s += ch;
			cin.unget();
			if (s == declare) return Token(let);
			if (s == declareConst) return Token(constant);
			return Token(name, s);
		}
		error("Bad token");
	}
}

void Token_stream::ignore(char c)
{
	if (full && c == buffer.kind) {
		full = false;
		return;
	}
	full = false;

	char ch = 0; //iniitalize ch to 0
	while (cin >> ch)
		if (ch == c) return;
}

class Variable { //change struct to public class
public:
	string name;
	double value;
	//add bool declaration for variable
	bool variable;
	Variable(string n, double v, bool var = true) :name(n), value(v), variable(var) { }
};

vector<Variable> names;

double get_value(string s)
{
	for (int i = 0; i < names.size(); ++i)
		if (names[i].name == s) return names[i].value;
	error("get: undefined name ", s);
}

void set_value(string s, double d)
{
	for (int i = 0; i < names.size(); ++i) //removed "="
		if (names[i].name == s) {
			//added the case for constant value
			if(names[i].variable==false) 
				error(s," is a constant cannot be changed.");
			names[i].value = d;
			return;
		}
	error("set: undefined name ", s);
}

bool is_declared(string variable)//changed s to variable
{
	for (int i = 0; i < names.size(); ++i)
		if (names[i].name == variable) return true;
	return false;
}

//function added for nameDef
double nameDef(string s, double val, bool variable=true){
    if (is_declared(s)) error(s," declared twice");
    names.push_back(Variable(s,val,variable));
    return val;
}

Token_stream ts;

double expression();

double primary()
{
	Token t = ts.get();
	switch (t.kind) {
	case '(':
	{	double d = expression();
	t = ts.get();
	if (t.kind != ')') error("')' expected");
	return d; //return statement added
	}
	case '-':
		return -primary();
	case number:
		return t.value;
	//added case +
	case '+':
	  return primary();
		//completed case name for exercise 2
	case name:
		{
			Token next = ts.get();
			if (next.kind == '=')
			{	// handle name = expression
				double d = expression();
				set_value(t.name,d);
				return d;
			}
			else {
				ts.unget(next);
				return get_value(t.name);
			}
		}
	default:
		error("primary expected");
	}
}

double term()
{
	double left = primary();
	//moved token for get
	Token t = ts.get();

	while (true) {
		switch (t.kind) {
		case '*':
			left *= primary();
			t = ts.get();//added to get
			break;

		case '/':
		{	double d = primary();
		if (d == 0) error("divide by zero");
		left /= d;
		t = ts.get();//added to get
		break;
		}

		//added case for mod
		case '%':
		{
				int x = narrow_cast<int>(left);
				int y = narrow_cast<int>(term());
				if (y == 0) error("divide by zero");
				left = x%y;
				t = ts.get();
				break;
		}

		default:
			ts.unget(t);
			return left;
		}
	}
}

double expression()
{
	double left = term();
	//moved roken to get
	Token t = ts.get();

	while (true) {
		switch (t.kind) {
		case '+':
			left += term();
			t = ts.get();//added to get
			break;

		case '-':
			left -= term();
			t = ts.get();//added to get
			break;

		default:
			ts.unget(t);
			return left;
		}
	}
}

double declaration(Token k) //added k from drill
{
	Token t = ts.get();
	if (t.kind != name) error("name expected in declaration"); //replaced to a with name
	string vName = t.name;

//	if (is_declared(name)) error(name, " declared twice");

	Token t2 = ts.get();
	if (t2.kind != '=') error("= missing in declaration of ", vName);

	//edited to add k
	double d = expression();
	nameDef(vName, d, k.kind==let);
	return d;
}

double statement()
{
	Token t = ts.get();
	switch (t.kind) {
	case let:
	case constant: //added constant case
		return declaration(t.kind);
	default:
		ts.unget(t);
		return expression();
	}
}

void clean_up_mess()
{
	ts.ignore(print);
}

const string prompt  = "> ";
const string result  = "= ";

void calculate()
{
	while (cin) try { //changed the true condition
		cout << prompt;
		Token t = ts.get();
		while (t.kind == print) t = ts.get();
		if (t.kind == quit) return;
		ts.unget(t);
		cout << result << statement() << endl;
	}
	catch (exception& e) { //changed it to exception to have a little uniformity
		cerr << e.what() << endl;
		clean_up_mess();
	}
}

int main()

try {
	calculate();
	return 0;
}
catch (exception& e) {
	cerr << "exception: " << e.what() << endl;
	char c;
	while (cin >> c && c != ';');
	return 1;
}
catch (...) {
	cerr << "exception\n";
	char c;
	while (cin >> c && c != ';');
	return 2;
}

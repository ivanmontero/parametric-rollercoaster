#include <iostream>
#include <algorithm>
#include <queue>
#include <stack>
#include <sstream>

#include "expr.h"

# define M_PI	3.14159265358979323846
# define M_E	2.71828182845904523536

const std::string const Expr::FUNCTIONS[4] = { "sin", "cos", "tan", "ln" };
const std::string const Expr::SPECIAL_NUMBERS[2] = { "e", "pi" };
const char const Expr::PARENTHESES[2] = { '(', ')' };
// PRECEDENCE = index / 2
const char const Expr::OPERATORS[6] = { '+','-','*','/','^', 'u' }; 
const char const Expr::NUMBERS[11] = { '0','1','2','3','4','5','6','7','8','9', '.'};

Symbolic Expr::parse(std::string expr) {
	std::cout << "Original   : " << expr << std::endl;
	return to_symbolic(to_postfix(to_infix(expr)));
}

// doesn't quicky evaluate negative numbers
double Expr::eval(std::string expr) {
	return expr.find_first_not_of("0123456789.") == std::string::npos 
		? std::stod(expr) : to_double(to_postfix(to_infix(expr)));
}

double Expr::eval(Symbolic s) {
	std::stringstream ss;
	ss << s;
	std::string expr = ss.str();
	return eval(expr);
}

// -------- String preparation -> in-fix form --------
// TODO LIST:
// - Support variables (x, y, theta, etc)
// - Properly placed multiplication symbols.
//		- Support side-by-side input (Ex: 4x instead of 4*x)
//			-- Requires check of numbers and variables
//		- Place parentheses around bare functions (ex: sinx)
// 
// ---------------------------------------------------
std::queue<std::string> Expr::to_infix(std::string str) {
	// Removes whitespace
	str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
	std::queue<std::string> infix;
	for (std::string::iterator it = str.begin(); it != str.end();) {
		// Numbers - can be more than one digit
		if (in_array(NUMBERS, *it)) {
			std::string n;
			while (it != str.end() && in_array(NUMBERS, *it)) {
				// Makes sure that there isn't multiple decimal points. 
				// Multiple after first encountered disregarded.
				if (*it != '.' || n.find('.') == std::string::npos)
					n.append(std::string(1, *it));
				it++;
			}
			infix.push(n);
			it--;
		}
		else if (in_array(OPERATORS, *it) || in_array(PARENTHESES, *it)) {
			// Unary minus -> preceded by another operator, left parentheses,
			// or is the first character
			if (*it == '-' && (it == str.begin() || in_array(OPERATORS, *(it - 1)) || *(it - 1) == '('))
				infix.push("u");	// "u" represents unary minus operator
			else
				infix.push(std::string(1, *it));
		}
		else {
			// var to avoid unnessessary checks.
			bool found = false;
			// TODO: Confine to one function
			// Test if function
			for (std::string f : FUNCTIONS) {
				// Makes sure theres enough room left in str to test
				if (std::distance(it, str.end()) >= f.size()) {
					// Check for equivalence
					if (f.compare(std::string(it, it + f.size())) == 0) {
						infix.push(f);
						it += f.size() - 1;
						found = true;
						break;
					}
				}
			}
			// Test if special number
			for (std::string n : SPECIAL_NUMBERS) {
				// Makes sure theres enough room left in str to test
				if (std::distance(it, str.end()) >= n.size()) {
					// Check for equivalence
					if (n.compare(std::string(it, it + n.size())) == 0) {
						infix.push(n);
						it += n.size() - 1;
						found = true;
						break;
					}
				}
			}
			/*********************** TODO: Test if special character (e, pi, etc) ***********************/
			// Treat "unknown characters" as variables
			if (!found) {
				infix.push(std::string(1, *it));
			}
		}
		if (it != str.end())
			it++;
	}

	std::cout << "infix      : ";
	for (int i = 0; i < infix.size(); i++) {
		std::cout << infix.front() << " ";
		infix.push(infix.front());
		infix.pop();
	}
	std::cout << std::endl;

	return infix;
}

// -------- Shunting-Yard Algorithm -> post-fix form --------
// TODO LIST:
// - (better) Support variables (x, y, theta, etc)
// ASSUMPTION LIST: (Assumes previous algorithm accounts for these)
// - if the first char of string is number, rest is a number
// - No numbers right next to a function (explicit multiplication)
// - ALL functions must have parentheses
// -----------------------------------------------------------
std::queue<std::string> Expr::to_postfix(std::queue<std::string> infix) {
	std::queue<std::string> postfix;
	std::stack<std::string> ops;
	while (!infix.empty()) {
		std::string s = infix.front();
		infix.pop();
		// Numbers pushed right into the stack
		if (in_array(NUMBERS, s[0]) || in_array(SPECIAL_NUMBERS, s)) {
			postfix.push(s);
			// If function, pushed right into ops stack. Assumed that openning
			// parenthesis will follow
		}
		else if (in_array(FUNCTIONS, s)) {
			ops.push(s);
		} else if (in_array(OPERATORS, s[0])) {
			// Should NEVER encounter a function when popping
			while (!ops.empty()
				&& ops.top()[0] != '('
				&& index_of(OPERATORS, ops.top()[0]) / 2 >= index_of(OPERATORS, s[0]) / 2
				&& s[0] != '^') {
				postfix.push(ops.top());
				ops.pop();
			}
			ops.push(s);
		}
		else if (s[0] == '(') {
			ops.push(s);
		}
		else if (s[0] == ')') {
			while (ops.top()[0] != '(') {
				postfix.push(ops.top());
				ops.pop();
			}
			ops.pop();
			if (!ops.empty() && in_array(FUNCTIONS, ops.top())) {
				postfix.push(ops.top());
				ops.pop();
			}
		} else {
			postfix.push(s); // Variable
		}
	}
	while (!ops.empty()) {
		postfix.push(ops.top());
		ops.pop();
	}

	std::cout << "postfix    : ";
	for (int i = 0; i < postfix.size(); i++) {
		std::cout << postfix.front() << " ";
		postfix.push(postfix.front());
		postfix.pop();
	}
	std::cout << std::endl;
	
	return postfix;
}

// -------- post-fix evaluator -> SymbolicC++ Object Creation --------
// TODO LIST:
// - Optimize
// PROBLEMS:
// - Stack to store Symbolic object AND strings
// -------------------------------------------------------------------
Symbolic Expr::to_symbolic(std::queue<std::string> postfix) {
	std::stack<Symbolic> ss;
	while (!postfix.empty()) {
		std::string s = postfix.front();
		postfix.pop();
		if (in_array(NUMBERS, s[0])) {
			ss.push(Symbolic(std::stod(s)));
		} else if (in_array(OPERATORS, s[0])) {
			if (s[0] == 'u') {
				Symbolic t = ss.top();
				ss.pop();
				ss.push(-t);
			} else {
				Symbolic r = ss.top(), l;
				ss.pop();
				l = ss.top();
				ss.pop();
				// '+','-','*','/','^'
				if (s[0] == '+')		ss.push(l + r);
				else if (s[0] == '-')	ss.push(l - r);
				else if (s[0] == '*')	ss.push(l * r);
				else if (s[0] == '/')	ss.push(l / r);
				else if (s[0] == '^')	ss.push(l ^ r);
			}
		} else if (in_array(SPECIAL_NUMBERS, s)) {
			// "e", "pi"
			if (s.compare("e") == 0)		ss.push(SymbolicConstant::e);
			else if (s.compare("pi") == 0)	ss.push(SymbolicConstant::pi);
		} else if (in_array(FUNCTIONS, s)) {
			Symbolic n = ss.top();
			ss.pop();
			// "sin", "cos", "tan", "ln"
			if (s.compare("sin") == 0)		ss.push(sin(n));
			else if (s.compare("cos") == 0) ss.push(cos(n));
			else if (s.compare("tan") == 0) ss.push(tan(n));
			else if (s.compare("ln") == 0)  ss.push(ln(n));
		}
		else { // variables
			ss.push(Symbolic(s));
		}
	}
	std::cout << "simplified : " << ss.top() << std::endl;

	return ss.top();
}

// -------- post-fix evaluator -> double value --------
// TODO LIST:
// - Optimize
// ----------------------------------------------------
double Expr::to_double(std::queue<std::string> postfix) {
	std::stack<double> ss;
	while (!postfix.empty()) {
		std::string s = postfix.front();
		postfix.pop();
		if (in_array(NUMBERS, s[0])) {
			ss.push(std::stod(s));
		}
		else if (in_array(OPERATORS, s[0])) {
			if (s[0] == 'u') {
				double t = ss.top();
				ss.pop();
				ss.push(-t);
			} else {
				double r = ss.top(), l;
				ss.pop();
				l = ss.top();
				ss.pop();
				// '+','-','*','/','^'
				if (s[0] == '+') ss.push(l + r);
				else if (s[0] == '-') ss.push(l - r);
				else if (s[0] == '*') ss.push(l * r);
				else if (s[0] == '/') ss.push(l / r);
				else if (s[0] == '^') ss.push(pow(l, r));
			}
		} else if (in_array(SPECIAL_NUMBERS, s)) {
			// "e", "pi"
			if (s.compare("e") == 0)		ss.push(M_E);
			else if (s.compare("pi") == 0)	ss.push(M_PI);
		} else if (in_array(FUNCTIONS, s)) {
			double n = ss.top();
			ss.pop();
			// "sin", "cos", "tan", "ln"
			if (s.compare("sin") == 0)	   ss.push(sin(n));
			else if (s.compare("cos") == 0) ss.push(cos(n));
			else if (s.compare("tan") == 0) ss.push(tan(n));
			else if (s.compare("ln") == 0)  ss.push(log(n));
		}
	}
	std::cout << "simplified : " << ss.top() << std::endl;

	return ss.top();
}

template<class T, class E> bool Expr::in_array(T & arr, E & element) {
	return std::find(std::begin(arr), std::end(arr), element) != std::end(arr);
}

template<class T, class E> int Expr::index_of(T & arr, E & element) {
	return std::distance(std::begin(arr), std::find(std::begin(arr), std::end(arr), element));
}
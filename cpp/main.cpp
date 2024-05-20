/*
 * This is a simple calculator program that can do basic operations.
 * It includes a math syntax analyzer that parses and evaluates mathematical expressions.
 *
 * Created by: Electrodiux-pbh
 * Repository: https://github.com/Electrodiux-pbh/console-calculator
 */

#include<iostream>
#include<string>
#include<cmath>

// Operations

enum class Operation {
	NONE = 0,
	SUM,
	MIN,
	MUL,
	DIV,
	POW,
};

Operation getOperationFromChar(char c) {
	switch (c) {
	case '+':
		return Operation::SUM;
	case '-':
		return Operation::MIN;
	case '*':
		return Operation::MUL;
	case '/':
		return Operation::DIV;
	case '^':
		return Operation::POW;
	}
	return Operation::NONE;
}

constexpr int MIN_PRIORITY = 1;
constexpr int MAX_PRIORITY = 3;

int getPriority(Operation operation) {
	switch (operation) {
	case Operation::SUM:
	case Operation::MIN:
		return 1;
	case Operation::MUL:
	case Operation::DIV:
		return 2;
	case Operation::POW:
		return 3;
	default:
		return 0;
	}
}

// Tokens

struct Token {
	virtual double resolve() = 0; // Pure virtual function
	virtual ~Token() {} // Virtual destructor
};

struct NumberToken : Token {

	double value;

	NumberToken(double value) {
		this->value = value;
	}

	double resolve() override {
		return value;
	}

};

struct OperationToken : Token {
	Operation operation;

	std::unique_ptr<Token> a, b;

	OperationToken(Operation operation, std::unique_ptr<Token> a, std::unique_ptr<Token> b)
		: operation(operation), a(std::move(a)), b(std::move(b)) {}

	OperationToken(Operation operation, std::unique_ptr<Token> a)
		: operation(operation), a(std::move(a)), b(std::make_unique<NumberToken>(0)) {}

	double resolve() override {
		double aValue = a->resolve();
		double bValue = b->resolve();

		switch (operation) {
		case Operation::SUM:
			return aValue + bValue;
		case Operation::MIN:
			return aValue - bValue;
		case Operation::MUL:
			return aValue * bValue;
		case Operation::DIV:
			return aValue / bValue;
		case Operation::POW:
			return std::pow(aValue, bValue);
		default:
			return 0;
		}
	}
};

// Parser

constexpr double PI = 3.14159265358979323846;
constexpr double E = 2.71828182845904523536;

double parseNumber(std::string token) {
	if (token == "pi") {
		return PI;
	}
	else if (token == "e") {
		return E;
	}
	else {
		return std::stod(token);
	}
}

std::unique_ptr<Token> parseToken(std::string expression) {
	std::string right, left;

	for (int i = 0; i < expression.size(); i++) {
		char character = expression[i];

		if (character == '(') {
			int open = 1;
			int close = 0;

			for (int j = i + 1; j < expression.size(); j++) {
				char character = expression[j];

				if (character == '(') {
					open++;
				}
				else if (character == ')') {
					close++;
				}

				if (open == close) {
					std::string subExpression = expression.substr(i + 1, j - i - 1);
					std::unique_ptr<Token> token = parseToken(subExpression);

					std::string newExpression = expression.substr(0, i) + std::to_string(token->resolve()) + expression.substr(j + 1);
					return parseToken(newExpression);
				}
			}
		}
	}

	// Iterate over the priority of the operations
	// We will first resolve the operations with priority 1 and then the operations with priority 2
	// Because creating the tree the operations that are more deep (proirity 2) will be resolved first
	for (int priority = MIN_PRIORITY; priority <= MAX_PRIORITY; priority++) {
		// Iterate over the expression
		for (int i = 0; i < expression.size(); i++) {
			char character = expression[i];

			// Get the operation
			Operation operation = getOperationFromChar(character);

			if (operation == Operation::NONE) {
				continue;
			}

			int operationPriority = getPriority(operation);
			if (operationPriority != priority) {
				continue;
			}

			// Get right and left side of the operation
			right = expression.substr(i + 1);
			left = expression.substr(0, i);

			if (left == "" && operation == Operation::MIN) { // If there is a minus sign and there is no left member ignore so it process as negative number
				continue;
			}

			// Parse the right and left side of the operation
			std::unique_ptr<Token> a = parseToken(left);
			std::unique_ptr<Token> b = parseToken(right);

			return std::make_unique<OperationToken>(operation, std::move(a), std::move(b));
		}
	}

	// If the token is a number
	return std::make_unique<NumberToken>(parseNumber(expression));
}

std::unique_ptr<Token> compileExpression(std::string expression) {
	std::string expressionWithoutSpaces = expression;
	expressionWithoutSpaces.erase(std::remove(expressionWithoutSpaces.begin(), expressionWithoutSpaces.end(), ' '), expressionWithoutSpaces.end());

	return parseToken(expressionWithoutSpaces);
}

// Program functions

void help() {
	std::cout << "Program created by Electrodiux-pbh (c) https://github.com/Electrodiux-pbh/console-calculator/\n";
	std::cout << "\nCommands:\n";
	std::cout << " - (h): prints the help to the console\n";
	std::cout << " - (q): quits the program\n";
	std::cout << " - (o): execute a operation\n";
	std::cout << "\nAvalaible operations:\n";
	std::cout << " - Addition (+)\n";
	std::cout << " - Substraction (-)\n";
	std::cout << " - Multiplication (*)\n";
	std::cout << " - Divition (/)\n";
	std::cout << " - Power (^)\n";
	std::cout << "\nConstants:\n";
	std::cout << " - pi = 3.14159265358979323846\n";
	std::cout << " - e = 2.71828182845904523536\n";
}

void unrecognizedAction() {
	std::cout << "Unrecognized action, type h for help" << std::endl;
}

int main() {
	std::cout << "Welcome to calculator, type an acction to do (type h for help)" << std::endl;

	while (true) { // Keep the program alive indefenetly
		std::string action;

		std::getline(std::cin, action);

		if (action == "h") {
			help();
			continue;
		}
		else if (action == "q") {
			break; // Break loop to exit the program 
		}
		else if (action == "o") {
			std::cout << "Enter a operation: ";

			std::string expression_str;
			std::getline(std::cin, expression_str);

			std::cout << std::endl;

			auto expression = compileExpression(expression_str);
			double result = expression->resolve();

			std::cout << expression_str << " = " << result << std::endl;
		}
		else {
			unrecognizedAction();
		}
	}

	return 0;
}

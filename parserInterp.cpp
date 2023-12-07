/* Implementation of Recursive-Descent Parser
	for a Simple Pasacal-Like Language
 * parser.cpp
 * Programming Assignment 2
 * Fall 2023
*/

#include "parserInterp.h"
#include <vector>
#include <queue>
//checks if the variable has been assigned
map<string, bool> defVar;
//stores the type of the variable
map<string, Token> SymTable;
//stores the value of the variable
map<string, Value> TempsResults;
//used by writeLn to keep track of value obects from exprList
std::queue <Value> * ValQue;
namespace Parser {
	bool pushed_back = false;
	LexItem	pushed_token;

	static LexItem GetNextToken(istream& in, int& line) {
		if( pushed_back ) {
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
	}

	static void PushBackToken(LexItem & t) {
		if( pushed_back ) {
			abort();
		}
		pushed_back = true;
		pushed_token = t;	
	}

}

static int error_count = 0;

int ErrCount()
{
    return error_count;
}

void ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}

//Prog ::= PROGRAM IDENT ; DeclPart CompoundStmt
bool Prog(istream& in, int& line) {
	LexItem t;
	t = Parser::GetNextToken(in, line);

	if (t != PROGRAM) {
		ParseError(line, "Missing PROGRAM Keyword");
		return false;
	}
	t = Parser:: GetNextToken(in, line);

	if (t != IDENT) {
		ParseError(line, "Missing Program Name.");
		return false;
	}
	t = Parser:: GetNextToken(in, line);

	if (t != SEMICOL) {
		ParseError(line, "Missing Semicolon in Program.");
		return false;
	}
	bool ex = DeclPart(in, line);

	if (!ex) {
		ParseError(line, "Missing DeclPart");
		return false;
	}

	ex = CompoundStmt(in, line);

	if (!ex) {
		ParseError(line, "Missing CompoundStmt");
		return false;
	}

	return ex;

}

// DeclPart ::= VAR DeclStmt; { DeclStmt ; }
bool DeclPart(istream& in, int& line) {
	LexItem tok = Parser::GetNextToken(in, line);

	if (tok != VAR) {
		ParseError(line, "Missing VAR");
		return false;
	}
	bool ex = DeclStmt(in, line);

	if (!ex) {
		ParseError(line, "Incorrect Declaration Section");
		return false;
	}
	while (true) {
		tok = Parser::GetNextToken(in, line);

		if (tok != SEMICOL) {
			ParseError(line, "Missing SEMICOL");
			return false;
		}
		tok = Parser::GetNextToken(in, line);
		Parser::PushBackToken(tok);
		if (tok != IDENT && tok != SEMICOL) {
			break;
		} else {
			if (DeclStmt(in, line) == false) {
				break;
			}
		}
	}

	return true;
}

//DeclStmt ::= IDENT {, IDENT } : Type [:= Expr]
bool DeclStmt(istream& in, int& line) {
	LexItem tok = Parser::GetNextToken(in, line);

	if (tok != IDENT) {
		ParseError(line, "Missing Var name");
		return false;
	}
	string varName = tok.GetLexeme();
	if (defVar.find(varName) != defVar.end()) {
		ParseError(line, "Variable Redefinition");
		return false;
	}
	
	defVar[varName] = false;
	tok = Parser::GetNextToken(in, line);
	vector<string> variables;
	variables.push_back(varName);

	while (true) {
		if (tok != COMMA) {
			if (tok != COLON) {
				ParseError(line, "Missing comma in declaration statement");
				ParseError(line, "Incorrect identifiers list in Declaration Statement");
				return false;
			}
			break;
		}
		tok = Parser::GetNextToken(in, line);

		if (tok != IDENT) {
			ParseError(line, "Missing Var name");
			return false;
		}
		varName = tok.GetLexeme();
		if (defVar.find(varName) != defVar.end()) {
			ParseError(line, "Variable Redefinition");
			return false;
		}
		
		defVar[varName] = false;
		tok = Parser::GetNextToken(in, line);

		variables.push_back(varName);
	}
	
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	if (tok != COLON) {
		ParseError(line, "Missing COLON");
		return false;
	}
	tok = Parser::GetNextToken(in, line);

	if (tok != INTEGER && tok != REAL && tok != BOOLEAN && tok != STRING) {
		ParseError(line, "Incorrect declaration type");
		return false;
	}

	for (auto i = variables.begin(); i != variables.end(); ++i){
        SymTable[*i] = tok.GetToken();
	}

	tok = Parser::GetNextToken(in, line);
	bool status;
	if (tok == ASSOP) {
		Value retVal;
		status = Expr(in, line, retVal);
		if (!status) {
			ParseError(line, "Missing Assignment Expression");
			return false;
		}
		for (auto i = variables.begin(); i != variables.end(); ++i){
			defVar[*i] = true;
			TempsResults[*i] = retVal;
		}
		defVar[varName] = true;
	} else {
		Parser::PushBackToken(tok);
	}
	return true;
}

//Stmt ::= SimpleStmt | StructuredStmt
bool Stmt(istream& in, int& line) {
	LexItem tok = Parser::GetNextToken(in, line);

	Parser::PushBackToken(tok);
	// check simple
	bool status = false;
	if (tok ==IDENT || tok == WRITELN || tok == WRITE){
		status = SimpleStmt(in, line);
		if (!status) {
			ParseError(line, "Missing SimpleStmt");
			return false;
		}
	//check structured
	}else if (tok == IF || tok == BEGIN) {
		status = StructuredStmt(in, line);
		if (!status) {
			ParseError(line, "Missing StructuredStmt");
			return false;
		}
	} else {
		/*
		if (tok == DOT) {
			line+= 1;
			return true;
		}
		*/
		return false;
	}
	return true;
}

//StructuredStmt ::= IfStmt | CompoundStmt
bool StructuredStmt(istream& in, int& line) {
	LexItem tok = Parser::GetNextToken(in, line);
	Parser::PushBackToken(tok);
	bool status = false;

	if (tok == IF) {
		status = IfStmt(in, line);
		if (!status) {
			ParseError(line, "Error in IfStmt");
			return false;
		}
	} else if (tok == BEGIN) {
		status = CompoundStmt(in, line);
		if (!status) {
			ParseError(line, "Error in CompoundStmt");
			return false;
		}
	} else {
		return false;
	}

	return true;
	
}

//CompoundStmt ::= BEGIN Stmt {; Stmt } END
bool CompoundStmt(istream& in, int& line) {
	LexItem tok = Parser::GetNextToken(in, line);

	if (tok != BEGIN) {
		ParseError(line, "Missing BEGIN");
		return false;
	}

	while (true) {
		bool status = Stmt(in, line);
		if (!status) {
			ParseError(line, "Error in  Stmt");
			return false;
		}
		tok = Parser::GetNextToken(in, line);
		if (tok != SEMICOL) {
			break;
		}
	}

	
	if (tok != END) {
		ParseError(line, "Missing END");
		return false;
	}
	return true;
}

//SimpleStmt ::= AssignStmt | WriteLnStmt | WriteStmt
bool SimpleStmt(istream& in, int& line) {
	LexItem tok = Parser::GetNextToken(in, line);

	//Parser::PushBackToken(tok);
	bool status = false;
	if (tok == IDENT) {
		Parser::PushBackToken(tok);
		status = AssignStmt(in, line);
		if (!status) {
			ParseError(line, "Error in AssignStmt");
			return false;
		}
	} else if (tok == WRITELN) {
		status = WriteLnStmt(in, line);
		if (!status) {
			ParseError(line, "Error in WriteLnStmt");
			return false;
		}
	} else if (tok == WRITE) {
		status = WriteStmt(in, line);
		if (!status) {
			ParseError(line, "Error in WriteStmt");
			return false;
		}
	} else {
		ParseError(line, "Missing SimpleStmt");
		Parser::PushBackToken(tok);
		return false;
	}
	return true;
}

//WriteLnStmt ::= writeln (ExprList) 
bool WriteLnStmt(istream& in, int& line) {
	LexItem t;
	//cout << "in WriteStmt" << endl;
	ValQue = new queue<Value>;
	t = Parser::GetNextToken(in, line);

	if( t != LPAREN ) {
		
		ParseError(line, "Missing Left Parenthesis");
		return false;
	}
	
	bool ex = ExprList(in, line);
	
	if( !ex ) {
		ParseError(line, "Missing expression list for WriteLn statement");
		return false;
	}
	
	t = Parser::GetNextToken(in, line);
	if(t != RPAREN ) {
		
		ParseError(line, "Missing Right Parenthesis");
		return false;
	}
	while (!(*ValQue).empty()) {
		cout << (*ValQue).front();
		ValQue->pop();
	}
	cout << endl;
	//Evaluate: print out the list of expressions values

	return ex;
}//End of WriteLnStmt

//WriteStmt ::= WRITE (ExprList)
bool WriteStmt(istream& in, int& line) {
	LexItem tok = Parser::GetNextToken(in, line);

	if (tok != LPAREN) {
		ParseError(line, "Missing Left Parenthesis");
		return false;
	}

	bool ex = ExprList(in, line);
	
	if( !ex ) {
		ParseError(line, "Missing expression list for WriteLn statement");
		return false;
	}
	
	tok = Parser::GetNextToken(in, line);

	if(tok != RPAREN ) {
		
		ParseError(line, "Missing Right Parenthesis");
		return false;
	}
	//Evaluate: print out the list of expressions values

	return ex;
}

//IfStmt ::= IF Expr THEN Stmt [ ELSE Stmt ]
bool IfStmt(istream& in, int& line) {
	LexItem tok = Parser::GetNextToken(in, line);

	if (tok != IF) {
		ParseError(line, "Missing IF");
		return false;
	}
	Value retVal;
	bool status = Expr(in, line, retVal);

	if (!status) {
		ParseError(line, "Error in IfStmt-Expr");
		return false;
	}
	// checks if the evaluation is a boolean
	if (!retVal.IsBool()) {
		ParseError(line, "Run-Time Error-Non boolean type in IfStmt");
		return false;
	}
	if (retVal.GetBool() == true) {
		tok = Parser::GetNextToken(in, line);

		if (tok != THEN) {
			ParseError(line, "Missing THEN");
			return false;
		}

		status = Stmt(in, line);

		if (!status) {
			ParseError(line, "Error in IfStmt-Stmt");
			return false;
		}
		tok = Parser::GetNextToken(in, line);
		if (tok == ELSE) {
			tok = Parser::GetNextToken(in, line);
			while (tok != SEMICOL) {
				//if theres an else clause it burns through all the tokens until it reaches the end of the line
				if (tok == BEGIN) {
					while (tok != END) {
						tok = Parser::GetNextToken(in, line);
					}
				}
				tok = Parser::GetNextToken(in, line);
			}
			
		}
		cout << tok << endl;
		cout << line << endl;
		Parser::PushBackToken(tok);
		
	} else {
		// else conditional
		tok = Parser::GetNextToken(in, line);
		while (tok != END && tok != SEMICOL) {
			//burns through the then statement until it finds an ELSE or end of line
			tok = Parser::GetNextToken(in, line);
		}
		tok = Parser::GetNextToken(in, line);
		if (tok != ELSE) {
			Parser::PushBackToken(tok);
		} else {
			status = Stmt(in, line);
			if (!status) {
				ParseError(line, "Error in IfStmt-ELSE");
				return false;
			}
		}
	}
	return true;
}

//AssignStmt ::= Var := Expr
//Var ::= IDENT
bool AssignStmt(istream& in, int& line) {
	LexItem tok = Parser::GetNextToken(in, line);
	string varName = tok.GetLexeme();
	bool status = Var(in, line, tok);

	if (!status){ 
		ParseError(line, "Error in Var");
		return false;
	}
	Token type = SymTable[varName];

	tok = Parser::GetNextToken(in, line);
	if (tok != ASSOP) {
		ParseError(line, "Missing ASSOP");
		return false;
	}
	Value retVal;
	status = Expr(in, line, retVal);

	if (!status){ 
		ParseError(line, "Missing Expr");
		return false;
	}

	switch (type) {
	case INTEGER:
		if (retVal.IsReal()) {
			TempsResults[varName] = Value((int) retVal.GetReal());
		} else if (retVal.IsInt()) {
			TempsResults[varName] = retVal;
		} else {
			type = ERR;
		}
		break;
	case REAL:
		if (retVal.IsReal()) {
			TempsResults[varName] = retVal;
		} else if (retVal.IsInt()) {
			TempsResults[varName] = Value((float) retVal.GetInt());
		} else {
			type = ERR;
		}
		break;
	case BOOLEAN:
		if (retVal.IsBool()) {
			TempsResults[varName] = retVal;
		} else {
			type = ERR;
		}
		break;
	case STRING:
		if (retVal.IsString()) {
			TempsResults[varName] = retVal;
		} else {
			type = ERR;
		}
		break;
	default:
		break;

	}
	if (type == ERR) {
		ParseError(line, "Runtime Error-Type mismatch when assigning to variable "+varName);
		return false;
	}
	//TempsResults[varName] = retVal;
	defVar[varName] = true;
	return status;
}
//Var ::= IDENT
bool Var(istream& in, int& line, LexItem & idtok) {
	string varName = idtok.GetLexeme();

	if (defVar.find(varName) == defVar.end()) {
		ParseError(line, "Variable Not Declared");
		return false;
	}
	return true;
}

//ExprList:= Expr {,Expr}
bool ExprList(istream& in, int& line) {
	bool status = false;
	//cout << "in ExprList and before calling Expr" << endl;
	Value retVal;
	status = Expr(in, line, retVal);
	if(!status){
		ParseError(line, "Missing Expression");
		return false;
	}
	ValQue->push(retVal);
	LexItem tok = Parser::GetNextToken(in, line);
	
	if (tok == COMMA) {
		//cout << "before calling ExprList" << endl;
		status = ExprList(in, line);
		//cout << "after calling ExprList" << endl;
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else{
		Parser::PushBackToken(tok);
		return true;
	}
	return status;
}//ExprList

//Expr ::= LogOrExpr ::= LogAndExpr { OR LogAndExpr }
bool Expr(istream& in, int& line, Value & retVal){
	bool status = LogANDExpr(in, line, retVal);
	Value val = retVal;
	if (!status) { 
		ParseError(line, "Missing LogANDExpr");
		return false;
	}
	LexItem tok = Parser::GetNextToken(in, line);
	while (tok == OR) {
		status = LogANDExpr(in, line, retVal);
		if (!status) { 
			ParseError(line, "Missing LogANDExpr");
			return false;
		}
		val = val || retVal;
		if (val.IsErr()) {
			ParseError(line, "Run-Time Error-Illegal Mixed Type Operands in Expr");
			return false;
		}
		tok = Parser::GetNextToken(in, line);
		
	}
	retVal = val;
	Parser::PushBackToken(tok);
	return true;
}

//LogAndExpr ::= RelExpr {AND RelExpr }
bool LogANDExpr(istream& in, int& line, Value & retVal) {
	Value val1, val2;
	bool status = RelExpr(in, line, val1);
	if (!status) { 
		ParseError(line, "Missing RelExpr");
		return false;
	}
	retVal = val1;
	LexItem tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while (tok == AND) {
		status = RelExpr(in, line, val2);

		if (!status) { 
			ParseError(line, "Missing RelExpr");
			return false;
		}
		retVal = retVal && val2;
		if (retVal.IsErr()) {
			ParseError(line, "Run-Time Error-Illegal Mixed Type Operands");
			return false;
		}
		tok = Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}
	}
	Parser::PushBackToken(tok);
	return true;
}

//RelExpr ::= SimpleExpr [ ( = | < | > ) SimpleExpr ]
bool RelExpr(istream& in, int& line, Value & retVal) {
	
	bool status = SimpleExpr(in, line, retVal);
	if (!status) {
		ParseError(line, "Missing SimpleExpr");
		return false;
	}
	Value val = retVal;
	LexItem tok = Parser::GetNextToken(in, line);

	if (tok == EQ || tok == LTHAN || tok == GTHAN) {
		status = SimpleExpr(in, line, retVal);
		if (!status) {
			ParseError(line, "Missing SimpleExpr");
			return false;
		}
		if (tok == EQ) {
			val = val == retVal;
		} else if (tok == LTHAN) {
			val = val < retVal;
		} else {
			val = val > retVal;
		}
		if (val.IsErr()) {
			ParseError(line, "Run-Time Error-Illegal Mixed Type Operands in RelExpr");
			return false;
		}
		retVal = val;
	} else {
		Parser::PushBackToken(tok);
	}

	return true;
}

//SimpleExpr :: Term { ( + | - ) Term 
bool SimpleExpr(istream& in, int& line, Value & retVal) {
	bool status = Term(in, line, retVal);
	if (!status) {
		ParseError(line, "Missing Term");
		return false;
	}
	Value val = retVal;
	LexItem tok = Parser::GetNextToken(in, line);
	while (tok == PLUS || tok == MINUS) {
		status = Term(in, line, retVal);
		if (!status) {
			ParseError(line, "Missing Term");
			return false;
		}
		switch (tok.GetToken()) {
		case PLUS:
			val = val + retVal;
			break;
		case MINUS:
			val = val - retVal;
			break;
		default:
			break;
		}
		if (val.IsErr()) {
			ParseError(line, "Run-Time Error-Illegal Mixed Type Operands in SimpleExpr");
			return false;
		}

		tok = Parser::GetNextToken(in, line);
		
	}
	retVal = val;
	Parser::PushBackToken(tok);

	return true;
}

//Term ::= SFactor { ( * | / | DIV | MOD ) SFactor }
bool Term(istream& in, int& line, Value & retVal) {
	bool status = SFactor(in, line, retVal);
	if (!status) {
		ParseError(line, "Missing SFactor");
		return false;
	}
	Value val1 = retVal;

	LexItem tok = Parser::GetNextToken(in, line);

	while (tok == MULT || tok == IDIV || tok == DIV || tok == MOD) {
		Token tokType = tok.GetToken();
		status = SFactor(in, line, retVal);
		if (!status) {
			ParseError(line, "Missing SFactor");
			return false;
		}

		switch (tokType) {
			case MULT:
				val1 = val1 * retVal;
				break;
			case IDIV:
				if ((retVal.IsInt() && retVal.GetInt() == 0) || (retVal.IsReal() && retVal.GetReal() == 0)) {
					ParseError(line, "Run-Time Error-Illegal Division by zero");
					return false;
				}
				val1 = val1.idiv(retVal);
				break;
			case DIV:
				if ((retVal.IsInt() && retVal.GetInt() == 0) || (retVal.IsReal() && retVal.GetReal() == 0)) {
					ParseError(line, "Run-Time Error-Illegal Division by zero");
					return false;
				}
				val1 = val1 / retVal;
				break;
			case MOD:
				val1 = val1 % retVal;
				break;
			default:
				break;
		}
		if (val1.IsErr()) {
			ParseError(line, "Run-Time Error-Illegal Mixed Type Operands in Term");
			return false;
		}
		tok = Parser::GetNextToken(in, line);
	}
	retVal = val1;
	Parser::PushBackToken(tok);
	return status;
}

//SFactor ::= [( - | + | NOT )] Factor
bool SFactor(istream& in, int& line, Value & retVal) {
	LexItem tok = Parser::GetNextToken(in, line);

	int sign = 0;
	//1 is negative
	if (tok == MINUS) {
		sign = 1;
	} else if (tok == PLUS) {
		sign = 2;
	} else if (tok == NOT) {
		sign = 3;
	} else {
		Parser::PushBackToken(tok);
	}

	bool status = Factor(in, line, retVal, sign);

	if (!status) {
		ParseError(line, "Missing Factor");
		return false;
	}
	return true;
}

// Factor ::= IDENT | ICONST | RCONST | SCONST | BCONST | (Expr)
bool Factor(istream& in, int& line, Value & retVal, int sign) {
	LexItem tok = Parser::GetNextToken(in, line);
	
	if (tok == LPAREN) {
		bool status = Expr(in, line, retVal);
		if (!status) {
			ParseError(line, "Missing Expr");
			return false;
		}
		tok = Parser::GetNextToken(in, line);

		if (tok != RPAREN) {
			ParseError(line, "Missing R	PAREN");
			return false;
		}
		return status;
	}
	Token type = tok.GetToken();
	string lexeme = tok.GetLexeme();
	while (true) {
		switch (type) {
			case IDENT:
				//checks to see if the variable was ever initalized
				if (TempsResults.find(lexeme) == TempsResults.end()) {
					ParseError(line, "Runtime Error-Uninitalized Variable");
					return false;
				} else {
					Value val = TempsResults[tok.GetLexeme()];
					//assigning another variables value to a different variable
					if (val.IsBool()) {
						type = BCONST;
						lexeme = val.GetBool() ? "true" : "false";
					} else if (val.IsInt()) {
						type = ICONST;
						lexeme = to_string(val.GetInt());
					} else if (val.IsReal()) {
						type = RCONST;
						lexeme = to_string(val.GetReal());
					} else {
						type = SCONST;
						lexeme = val.GetString();
					}
				}
				break;
			case ICONST:
				//converts string lexeme to int
				if (sign == 3) {
					ParseError(line, "Runtime Error-Illegal Operand for not operator");
					return false;
				}
				if (sign ==1) {
					// negative value
					retVal = Value((-1)*(std::stoi(lexeme)));
				} else {
					retVal = Value(std::stoi(lexeme));
				}
				return true;
				break;
			case RCONST:
				//converts string lexeme to double
				if (sign == 3) {
					ParseError(line, "Runtime Error-Illegal Operand for not operator");
					return false;
				}
				if (sign == 1) {
					//negative value
					retVal = Value((-1)*(std::stof(lexeme)));
				} else {
					retVal = Value(std::stof(lexeme));
				}
				return true;
				break;
			case SCONST:
				if (sign != 0) {
					ParseError(line, "Runtime Error-Illegal Operand for sign operator");
					return false;
				}
				retVal = Value(lexeme);
				return true;
				break;
			case BCONST:
				//converts string lexeme to bool
				if (sign == 1 || sign == 2) {
					ParseError(line, "Runtime Error-Illegal Operand for sign operator");
					return false;
				}
				
				if (lexeme == "false") {
					if (sign == 3) {
						//not false
						retVal = Value(true);
					} else {
						retVal = Value(false);
					}
				} else {
					if (sign == 3) {
						//not true
						retVal = Value(false);
					} else {
						retVal = Value(true);
					}
				}
				return true;
				break;
			default:
				ParseError(line, "Missing IDENT, ICONST, RCONST, SCONST, BCONST, or LPAREN");
				return false;
		}
	}
}
/* Implementation of Recursive-Descent Parser
	for a Simple Pasacal-Like Language
 * parser.cpp
 * Programming Assignment 2
 * Fall 2023
*/

#include "parserInt.h"
#include <vector>

map<string, bool> defVar;
map<string, Token> SymTable;

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
		status = Expr(in, line);
		if (!status) {
			ParseError(line, "Missing Assignment Expression");
			return false;
		}
		for (auto i = variables.begin(); i != variables.end(); ++i){
			defVar[*i] = true;
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
			Parser::PushBackToken(tok);
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
	bool status = Expr(in, line);

	if (!status) {
		ParseError(line, "Error in IfStmt-Expr");
		return false;
	}

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

	if (tok != ELSE) {
		Parser::PushBackToken(tok);
		return true;
	} else {
		status = Stmt(in, line);
		if (!status) {
			ParseError(line, "Error in IfStmt-Stmt");
			return false;
		}
		return true;
	}
}

//AssignStmt ::= Var := Expr
//Var ::= IDENT
bool AssignStmt(istream& in, int& line) {
	//IDENT token already used
	LexItem tok = Parser::GetNextToken(in, line);
	Parser::PushBackToken(tok);
	string varName = tok.GetLexeme();
	bool status = Var(in, line);

	if (!status){ 
		ParseError(line, "Error in Var");
		return false;
	}
	//Token type = SymTable[varName];

	tok = Parser::GetNextToken(in, line);
	if (tok != ASSOP) {
		ParseError(line, "Missing ASSOP");
		return false;
	}
	
	status = Expr(in, line);

	if (!status){ 
		ParseError(line, "Missing Expr");
		return false;
	}
	defVar[varName] = true;
	return status;
}

bool Var(istream& in, int& line) {
	LexItem tok = Parser::GetNextToken(in, line);
	string varName = tok.GetLexeme();

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

	status = Expr(in, line);
	if(!status){
		ParseError(line, "Missing Expression");
		return false;
	}
	
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
bool Expr(istream& in, int& line){

	while (true) {
		bool status = LogANDExpr(in, line);
		if (!status) { 
			ParseError(line, "Missing LogANDExpr");
			return false;
		}
		LexItem tok = Parser::GetNextToken(in, line);
		if (tok != OR) {
			Parser::PushBackToken(tok);
			break;
		}
	}
	return true;
}

//LogAndExpr ::= RelExpr {AND RelExpr }
bool LogANDExpr(istream& in, int& line) {
	while (true) {
		bool status = RelExpr(in, line);
	
		if (!status) { 
			ParseError(line, "Missing RelExpr");
			return false;
		}
		LexItem tok = Parser::GetNextToken(in, line);
		if (tok != AND) {
			Parser::PushBackToken(tok);
			break;
		}
	}
	return true;
}

//RelExpr ::= SimpleExpr [ ( = | < | > ) SimpleExpr ]
bool RelExpr(istream& in, int& line) {
	
	bool status = SimpleExpr(in, line);
	if (!status) {
		ParseError(line, "Missing SimpleExpr");
		return false;
	}

	LexItem tok = Parser::GetNextToken(in, line);

	if (tok == EQ || tok == LTHAN || tok == GTHAN) {
		status = SimpleExpr(in, line);
		if (!status) {
			ParseError(line, "Missing SimpleExpr");
			return false;
		}
	} else {
		Parser::PushBackToken(tok);
	}

	return true;
}

//SimpleExpr :: Term { ( + | - ) Term 
bool SimpleExpr(istream& in, int& line) {
	while (true) {
		bool status = Term(in, line);
		if (!status) {
			ParseError(line, "Missing Term");
			return false;
		}
		LexItem tok = Parser::GetNextToken(in, line);
		if (tok != PLUS && tok != MINUS) {
			Parser::PushBackToken(tok);
			break;
		}
	}

	return true;
}

//Term ::= SFactor { ( * | / | DIV | MOD ) SFactor }
bool Term(istream& in, int& line) {
	while (true) {
		bool status = SFactor(in, line);
		if (!status) {
			ParseError(line, "Missing SFactor");
			return false;
		}
		LexItem tok = Parser::GetNextToken(in, line);
		if (tok != MULT && tok != IDIV && tok != DIV && tok != MOD) {
			Parser::PushBackToken(tok);
			break;
		}
	}
	return true;
}

//SFactor ::= [( - | + | NOT )] Factor
bool SFactor(istream& in, int& line) {
	LexItem tok = Parser::GetNextToken(in, line);

	int sign = 0;

	if (tok == MINUS) {
		sign = 1;
	} else if (tok == PLUS) {
		sign = 0;
	} else if (tok == NOT) {
		sign = 2;
	} else {
		Parser::PushBackToken(tok);
	}

	bool status = Factor(in, line, sign);

	if (!status) {
		ParseError(line, "Missing Factor");
		return false;
	}
	return true;
}

// Factor ::= IDENT | ICONST | RCONST | SCONST | BCONST | (Expr)
bool Factor(istream& in, int& line, int sign) {
	LexItem tok = Parser::GetNextToken(in, line);

	if (tok != IDENT && tok != ICONST && tok != RCONST && tok!= SCONST && tok != BCONST && tok != LPAREN) {
		ParseError(line, "Missing IDENT, ICONST, RCONST, SCONST, BCONST, or LPAREN");
		return false;
	}
	if (tok == LPAREN) {
		bool status = Expr(in, line);
		if (!status) {
			ParseError(line, "Missing Expr");
			return false;
		}
		tok = Parser::GetNextToken(in, line);

		if (tok != RPAREN) {
			ParseError(line, "Missing R	PAREN");
			return false;
		}
	}
	return true;
}
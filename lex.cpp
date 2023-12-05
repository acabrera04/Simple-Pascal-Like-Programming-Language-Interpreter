#include "lex.h"
#include <string>
#include <map>
#include <algorithm>
#include <set>

using namespace std;

ostream& operator<<(ostream& out, const LexItem& tok){
	const string tokenArray[] = {
        "IF", "ELSE", "WRITELN", "WRITE", "INTEGER", "REAL",
	"BOOLEAN", "STRING", "BEGIN", "END", "VAR", "THEN", "PROGRAM",

	// identifiers
	"IDENT", "TRUE", "FALSE",

	// an integer, real, and string constant
	"ICONST", "RCONST", "SCONST", "BCONST",

	// the arithmetic operators, logic operators, relational operators
	"PLUS", "MINUS", "MULT", "DIV", "IDIV", "MOD", "ASSOP", "EQ", 
	"GTHAN", "LTHAN", "AND", "OR", "NOT", 
	//Delimiters
	"COMMA", "SEMICOL", "LPAREN", "RPAREN", "DOT", "COLON",
	// any error returns this token
	"ERR",

	// when completed (EOF), return this token
	"DONE",
    };

	if (tok == IDENT || tok == SCONST || tok == ICONST || tok == RCONST || tok == BCONST || tok == ERR) {
		out << tokenArray[tok.GetToken()] << ": \"" << tok.GetLexeme() << "\"" << endl;
		
	} else {
		out << tokenArray[tok.GetToken()] << endl;
	}

	return out;
    
}

LexItem id_or_kw(const string& lexeme, int linenum){
    map <string, Token> StrToTok = {
		{"program", PROGRAM},
		{"writeln", WRITELN},
		{"write", WRITE },
		{ "if", IF },
		{ "else", ELSE },	
		{ "IDENT", IDENT },
		{"var", VAR},
		{"begin", BEGIN},
		{"end", END},
		{"integer", INTEGER},
		{"real", REAL},
		{"string", STRING},
		{"boolean", BOOLEAN},
		
		{ "ICONST", ICONST },
		{ "RCONST", RCONST },
		{ "SCONST", SCONST  },
		{ "BCONST", BCONST },
			
		{ "+", PLUS },
		{ "-" , MINUS },
		{ "*" , MULT  },
		{ "/" , DIV },
		{ "div" , IDIV },
		{ ":=", ASSOP },
		{ "=", EQ  },
		{ ">" , GTHAN  },
		{ "<", LTHAN },
		{ "mod", MOD},
		{ "and", AND},
		{"then", THEN},
		{ "or", OR},
		{ "not", NOT},
				            
		{ "COMMA", COMMA  },
		{ "LPAREN", LPAREN },
		{ "RPAREN", RPAREN },
			
		{ "SEMICOL", SEMICOL  },
		{ "DOT", DOT },
		
		{ "ERR",ERR  },


		{ "DONE", DONE },
};

    if (lexeme == "true" || lexeme == "false") {
        LexItem temp(BCONST, lexeme, linenum);
        return temp;
    }
    
    if (StrToTok.find(lexeme) != StrToTok.end()) {
        LexItem temp(StrToTok[lexeme], lexeme, linenum);
        return temp;
    }
    LexItem temp(IDENT, lexeme, linenum);
    return temp;
}

LexItem getNextToken(istream& in, int& linenum) {
    enum TokState{START, INID, ININT, INSTRING, INREAL, ASSOP, COMM} lexState = START;
	string lexeme;
	char ch;
	set<char> delimiter = {',', ';', '(', ')', ':', '.', '+', '-', '*', '/', '=', '<', '>'};
	while (in.get(ch)) {
		//cout << "char: " << ch;
		//cout << " state: " << lexState;
		//cout << " lexeme: " << lexeme;
		//cout << endl;

		switch (lexState) {
			case START:
				if (ch == '\n') {
					linenum += 1;
				} else if (isspace(ch)) {
					lexState = START;
				} else if (ch == '.') {
					lexeme += ch;
					return LexItem (DOT, lexeme, linenum);
				} else if (ch == ';') {
					lexeme += ch;
					return LexItem(SEMICOL, lexeme, linenum);
				} else if (ch == '(') {
					lexeme += ch;
					return LexItem(LPAREN, lexeme, linenum);
				} else if (ch == ')') {
					lexeme += ch;
					return LexItem(RPAREN, lexeme, linenum);
				}  else if (ch == ':') {
					lexeme += ch;
					lexState = ASSOP;
				} else if (ch == ',') {
					lexeme += ch;
					return LexItem(COMMA, lexeme, linenum);
				} else if (ch == '\"' || ch == '?') {
					lexeme += ch;
					return LexItem (ERR, lexeme, linenum);
				} else if (ch == '{') {
					lexState = COMM;
				} else if (delimiter.find(ch) != delimiter.end()) {
					lexeme += ch;
					return id_or_kw(lexeme, linenum);
				} else if (isdigit(ch)) {
					lexState = ININT;
					lexeme += ch;
				} else if (ch == '\'') {
					lexState = INSTRING;
					lexeme += ch;
				} else {
					lexState = INID;
					lexeme += ch;
				}
				break;
			case ININT:
				if (ch == '.') {
					lexState = INREAL;
					lexeme += ch;
				} else if (isdigit(ch)) {
					lexeme += ch;
				} else {
					in.putback(ch);
					return LexItem (ICONST, lexeme, linenum);
				}
				break;
			case INREAL:
				if (ch == '.') {
					lexeme += ch;
					return LexItem (ERR, lexeme, linenum);
				} else if (isdigit(ch)) {
					lexeme += ch;
				} else {
					in.putback(ch);
					return LexItem (RCONST, lexeme, linenum);
				}
				break;
			case INSTRING:
				if (ch == '\'') {
					lexeme.erase(0,1);
					return LexItem (SCONST, lexeme, linenum);
				} else if (ch == '\"') {
					lexeme += ch;
					return LexItem (ERR, lexeme, linenum);
				} else if (ch == '\n') {
					return LexItem (ERR, lexeme, linenum);
				} else {
					lexeme += ch;					
				}
				break;
			case INID:
				if ((ch == '?' || ch == '\"') || isspace(ch) || delimiter.find(ch) != delimiter.end()){
					in.putback(ch);
					return id_or_kw(lexeme, linenum);
				} else {
					lexeme += ch;
				}
				break;
			case ASSOP:
				if (ch == '=') {
					lexeme += ch;
					return id_or_kw(lexeme, linenum);
				} else {
					return LexItem(COLON, lexeme, linenum);
				}
				break;
			case COMM:
				if (ch == '\n') {
					linenum++;
					break;
				} else if (ch == '}') {
					lexState = START;
					break;
				}
		}
	}
	
	LexItem temp(DONE, "", linenum);
    return temp;
}
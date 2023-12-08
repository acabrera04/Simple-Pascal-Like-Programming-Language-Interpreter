# Simple-Pascal-Like-Programming-Language-Interpreter
An interpreter written in C++ to handle a simple pascal like programming language.

It uses the following EBNF:
1. Prog ::= PROGRAM IDENT ; DeclPart CompoundStmt .
2. DeclPart ::= VAR DeclStmt; { DeclStmt ; }
3. DeclStmt ::= IDENT {, IDENT } : Type [:= Expr]
4. Type ::= INTEGER | REAL | BOOLEAN | STRING
5. Stmt ::= SimpleStmt | StructuredStmt
6. SimpleStmt ::= AssignStmt | WriteLnStmt | WriteStmt
7. StructuredStmt ::= IfStmt | CompoundStmt
8. CompoundStmt ::= BEGIN Stmt {; Stmt } END
9. WriteLnStmt ::= WRITELN (ExprList)
10. WriteStmt ::= WRITE (ExprList)
11. IfStmt ::= IF Expr THEN Stmt [ ELSE Stmt ]
12. AssignStmt ::= Var := Expr
13. Var ::= IDENT
14. ExprList ::= Expr { , Expr }
15. Expr ::= LogOrExpr ::= LogAndExpr { OR LogAndExpr }
16. LogAndExpr ::= RelExpr {AND RelExpr }
17. RelExpr ::= SimpleExpr [ ( = | < | > ) SimpleExpr ]
18. SimpleExpr :: Term { ( + | - ) Term }
19. Term ::= SFactor { ( * | / | DIV | MOD ) SFactor }
20. SFactor ::= [( - | + | NOT )] Factor
21. Factor ::= IDENT | ICONST | RCONST | SCONST | BCONST | (Expr)

Table of Operators Precedence

| Precedence | Operator | Description | Associativity |
| ---- | ---- | ---- | ---- |
| 1 |  Unary +, -, not| Unary plus, minus, and not (complement) | Right-to-Left |
| 2 | *, /, div, mod | Multiplication, Division, integer division, and modulus | Left-to-Right |
| 3 | +, - | Addition, and Subtraction, | Left-to-Right |
| 4 | <, >, =  | Less than, greater than, and equality | (No Cascading) |
| 5 | and | Logical Anding | Left-to-Right |
| 6 | or | Logical Oring | Left-to-Right |

The language has four types: integer, real, boolean, and string

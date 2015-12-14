#ifndef _WORDANALYZE_H_
#define _WORDANALYZE_H_
#include <iostream>
#include <fstream>
#pragma warning(disable:4996)
#include <sstream>
#include <stdio.h>
#include <string>

namespace MyPL0Compiler{
	#define NRW        13     // number of reserved words
	#define NSYM       11     // maximum number of symbols in array ssym and csym
	#define MAXIDLEN   10     // length of identifiers
	enum symtype
	{
		SYM_NULL,
		SYM_IDENTIFIER,
		SYM_NUMBER,
		SYM_PLUS,
		SYM_MINUS,
		SYM_TIMES,
		SYM_SLASH,
		SYM_ODD,
		SYM_EQU,
		SYM_NEQ,
		SYM_LES,
		SYM_LEQ,
		SYM_GTR,
		SYM_GEQ,
		SYM_LPAREN,
		SYM_RPAREN,
		SYM_COMMA,
		SYM_SEMICOLON,
		SYM_PERIOD,
		SYM_BECOMES,
		SYM_BEGIN,
		SYM_END,
		SYM_IF,
		SYM_THEN,
		SYM_WHILE,
		SYM_DO,
		SYM_CALL,
		SYM_CONST,
		SYM_VAR,
		SYM_PROCEDURE,
		SYM_READ,
		SYM_WRITE,
		SYM_X1
	};

	enum idtype
	{
		ID_CONSTANT, ID_VARIABLE, ID_PROCEDURE
	};



	extern char* word[NRW + 1];

	extern int wsym[NRW + 1];


	extern int ssym[NSYM + 1];


	extern char csym[NSYM + 1];


	extern std::ifstream myfile1;//code txt

	extern class WordAnalyze{
	public :
		char ch = ' ';         // last character read
		int  sym = 0;        // last symbol read
		char id[MAXIDLEN + 1]; // last identifier read
		int  num = 0;        // last number read
		int  cc = 0;         // character count
		int  ll = 0;         // line length
		char line[80];
		int  cx = 0;         // index of current instruction to be generated.
		WordAnalyze(){}
		~WordAnalyze(){}
		void test(){ sym++; }
		void getch();
		void getsym();
	};
}

#endif
#include "WordAnalyze.h"

namespace MyPL0Compiler{

	char * word[NRW + 1] = 
	{
		"", /* place holder */
		"begin", "call", "const", "do", "end", "if",
		"odd", "procedure", "then", "var", "while", "read", "write"
	};
	int wsym[NRW + 1] =
	{
		SYM_NULL, SYM_BEGIN, SYM_CALL, SYM_CONST, SYM_DO, SYM_END,
		SYM_IF, SYM_ODD, SYM_PROCEDURE, SYM_THEN, SYM_VAR, SYM_WHILE,SYM_READ,
		SYM_WRITE
	};

	int ssym[NSYM + 1] =
	{
		SYM_NULL, SYM_PLUS, SYM_MINUS, SYM_TIMES, SYM_SLASH,
		SYM_LPAREN, SYM_RPAREN, SYM_EQU, SYM_COMMA, SYM_PERIOD, SYM_SEMICOLON,
		SYM_X1
	};

	char csym[NSYM + 1] =
	{
		' ', '+', '-', '*', '/', '(', ')', '=', ',', '.', ';', '#'
	};

	std::ifstream myfile1("d:/code.txt");//code txt
	int amount = 0;
	void WordAnalyze::getch()
	{
		if (cc == ll)
		{
			ll = cc = 0;
			while (!MyPL0Compiler::myfile1.eof() && (ch = MyPL0Compiler::myfile1.get()) != '\n')
			{
				line[++ll] = ch;
			} // while
			line[++ll] = ' ';
		}
		ch = line[++cc];
	} // getch


	void WordAnalyze::getsym()
	{
		int i, k;
		char a[MAXIDLEN + 1];

		while (ch == ' ')
			getch();

		if (isalpha(ch))
		{ // symbol is a reserved word or an identifier.
			k = 0;
			do
			{
				if (k < MAXIDLEN)
					a[k++] = ch;
				getch();
			} while (isalpha(ch) || isdigit(ch));
			a[k] = 0;
			strcpy(id, a);
			word[0] = id;
			i = NRW;
			while (strcmp(id, word[i--]));
			if (++i){
				sym = wsym[i]; // symbol is a reserved word
			}
			else
				sym = SYM_IDENTIFIER;   // symbol is an identifier
		}
		else if (isdigit(ch))
		{ // symbol is a number.
			k = num = 0;
			sym = SYM_NUMBER;
			do
			{
				num = num * 10 + ch - '0';
				k++;
				getch();
			} while (isdigit(ch));
		}
		else if (ch == ':')
		{
			getch();
			if (ch == '=')
			{
				sym = SYM_BECOMES; // :=
				getch();
			}
			else
			{
				sym = SYM_NULL;       // illegal?
			}
		}
		else if (ch == '>')
		{
			getch();
			if (ch == '=')
			{
				sym = SYM_GEQ;     // >=
				getch();
			}
			else
			{
				sym = SYM_GTR;     // >
			}
		}
		else if (ch == '<')
		{
			getch();
			if (ch == '=')
			{
				sym = SYM_LEQ;     // <=
				getch();
			}
			else if (ch == '>')
			{
				sym = SYM_NEQ;     // <>
				getch();
			}
			else
			{
				sym = SYM_LES;     // <
			}
		}
		else
		{ // other tokens
			i = NSYM;
			csym[0] = ch;
			while (csym[i--] != ch);
			if (++i)
			{
				sym = ssym[i];
				getch();
			}
		}
	} // getsym

}

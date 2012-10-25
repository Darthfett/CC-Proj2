%{
/*
 * grammar.y
 *
 * Pascal grammar in Yacc format, based originally on BNF given
 * in "Standard Pascal -- User Reference Manual", by Doug Cooper.
 * This in turn is the BNF given by the ANSI and ISO Pascal standards,
 * and so, is PUBLIC DOMAIN. The grammar is for ISO Level 0 Pascal.
 * The grammar has been massaged somewhat to make it LALR.
 */

#include "shared.h"
#include "rulefuncs.h"
#include "usrdef.h"
#include <assert.h>

  // struct myText text;
  int yylex(void);
  void yyerror(const char *error);

  extern char *yytext;          /* yacc text variable */
  extern int line_number;       /* Holds the current line number; specified
				   in the lexer */
  struct program_t *program;    /* points to our program */
%}

%token AND ARRAY ASSIGNMENT CLASS COLON COMMA DIGSEQ
%token DO DOT DOTDOT ELSE END EQUAL EXTENDS FUNCTION
%token GE GT IDENTIFIER IF LBRAC LE LPAREN LT MINUS MOD NEW NOT
%token NOTEQUAL OF OR PBEGIN PLUS PRINT PROGRAM RBRAC
%token RPAREN SEMICOLON SLASH STAR THEN
%token VAR WHILE

%type <tden> type_denoter
%type <id> result_type
%type <id> identifier
%type <idl> identifier_list
%type <fdes> function_designator
%type <apl> actual_parameter_list
%type <apl> params
%type <ap> actual_parameter
%type <vd> variable_declaration
%type <vdl> variable_declaration_list
%type <r> range
%type <un> unsigned_integer
%type <fpsl> formal_parameter_section_list
%type <fps> formal_parameter_section
%type <fps> value_parameter_specification
%type <fps> variable_parameter_specification
%type <va> variable_access
%type <as> assignment_statement
%type <os> object_instantiation
%type <ps> print_statement
%type <e> expression
%type <s> statement
%type <ss> compound_statement
%type <ss> statement_sequence
%type <ss> statement_part
%type <is> if_statement
%type <ws> while_statement
%type <e> boolean_expression
%type <iv> indexed_variable
%type <ad> attribute_designator
%type <md> method_designator
%type <iel> index_expression_list
%type <e> index_expression
%type <se> simple_expression
%type <t> term
%type <f> factor
%type <i> sign
%type <p> primary
%type <un> unsigned_constant
%type <un> unsigned_number
%type <at> array_type
%type <cb> class_block
%type <vdl> variable_declaration_part
%type <fdl> func_declaration_list
%type <funcd> function_declaration
%type <fb> function_block
%type <fh> function_heading
%type <id> function_identification
%type <fpsl> formal_parameter_list
%type <cl> class_list
%type <ci> class_identification
%type <program> program
%type <ph> program_heading
%type <op> relop
%type <op> addop
%type <op> mulop

%union {
  struct type_denoter_t *tden;
  char *id;
  struct identifier_list_t *idl;
  struct function_designator_t *fdes;
  struct actual_parameter_list_t *apl;
  struct actual_parameter_t *ap;
  struct variable_declaration_list_t *vdl;
  struct variable_declaration_t *vd;
  struct range_t *r;
  struct unsigned_number_t *un;
  struct formal_parameter_section_list_t *fpsl;
  struct formal_parameter_section_t *fps;
  struct variable_access_t *va;
  struct assignment_statement_t *as;
  struct object_instantiation_t *os;
  struct print_statement_t *ps;
  struct expression_t *e;
  struct statement_t *s;
  struct statement_sequence_t *ss;
  struct if_statement_t *is;
  struct while_statement_t *ws;
  struct indexed_variable_t *iv;
  struct attribute_designator_t *ad;
  struct method_designator_t *md;
  struct index_expression_list_t *iel;
  struct simple_expression_t *se;
  struct term_t *t;
  struct factor_t *f;
  int *i;
  struct primary_t *p;
  struct array_type_t *at;
  struct class_block_t *cb;
  struct func_declaration_list_t *fdl;
  struct function_declaration_t *funcd;
  struct function_block_t *fb;
  struct function_heading_t *fh;
  struct class_identification_t *ci;
  struct class_list_t *cl;
  struct program_t *program;
  struct program_heading_t *ph;
  int op;
}



%%

program : program_heading semicolon class_list DOT
	{
	
	printf("program : program_heading semicolon class_list DOT \n");
	$$ = (struct program_t*) malloc(sizeof(struct program_t));

	program = $$;
	$$->ph = $1;
	$$->cl = $3;
	}
 ;

program_heading : PROGRAM identifier
	{
	printf("program_heading : PROGRAM identifier \n");
	$$ = (struct program_heading_t *) malloc(sizeof(struct program_heading_t));
	$$->id = $2;
	}
 | PROGRAM identifier LPAREN identifier_list RPAREN
	{
	// printf("my text = %s\n\n",text.id);
	printf("PROGRAM identifier LPAREN identifier_list RPAREN \n");

	$$ = (struct program_heading_t *) malloc(sizeof(struct program_heading_t));
	$$->id = $2;
	$$->il = $4;
	}
 ;

identifier_list : identifier_list comma identifier
        {
	printf("identifier_list : identifier_list comma identifier \n%s",yytext);
	// ?

	//$$->next = $1; 
	// 	OR 
	// $1->next = $$
	
	//$$->id = $3;
        }
 | identifier
        {
	printf("identifier_list : identifier \n");
	$$ = (struct identifier_list_t*) malloc(sizeof(struct identifier_list_t));
	//$$->next = NULL;  //OR just leave blank?
	//$$->id = $1;
	$$->next = NULL;
	$$->id = $1;
        }
 ;

class_list : class_list class_identification PBEGIN class_block END
	{
	printf("class_list : class_list class_identification PBEGIN class_block END \n");
	$$ = (struct class_list_t*) malloc(sizeof(struct class_list_t));
	
	$$->next = $1;
	$$->ci = $2;
	$$->cb = $4;
	}
 | class_identification PBEGIN class_block END
	{
	printf("class_list : class_identification PBEGIN class_block END \n");
	$$ = (struct class_list_t*) malloc(sizeof(struct class_list_t));

	$$->next = NULL;
	$$->ci = $1;
	$$->cb = $3;
	}
 ;

class_identification : CLASS identifier
	{
	printf("class_identification : CLASS identifier \n");

	$$ = (struct class_identification_t*) malloc(sizeof(struct class_identification_t));
	$$->id = $2;
	$$->extend = NULL;
	$$->line_number = line_number;
	}
| CLASS identifier EXTENDS identifier
	{
	printf("class_identification : CLASS identifier EXTENDS identifier \n");

	$$ = (struct class_identification_t*) malloc(sizeof(struct class_identification_t));
	$$->id = $2;
	$$->extend = $4;
	$$->line_number = line_number;
	}
;

class_block : variable_declaration_part func_declaration_list
	{
	printf("class_block : variable_declaration_part func_declaration_list \n");
	$$ = (struct class_block_t*) malloc(sizeof(struct class_block_t));

	$$->vdl = $1;
	$$->fdl = $2;
	}
 ;

type_denoter : array_type
	{
	printf("type_denoter : array_type \n");
	// 1 - array_type
	// 2 - class_type
	// 3 - base_type
	$$->type = 1;
	
	// $$->name = ?;
	$$->data.at = $1;

	}
 | identifier
	{
	printf("type_denoter : identifier \n");
	// $$->type = ?;
	}
 ;

array_type : ARRAY LBRAC range RBRAC OF type_denoter
	{
	printf("array_type : ARRAY LBRAC range RBRAC OF type_denoter \n");
	$$ = (struct array_type_t*) malloc(sizeof(struct array_type_t));
	$$->r = $3;
	$$->td = $6;
	}
 ;

range : unsigned_integer DOTDOT unsigned_integer
	{
	printf("range : unsigned_integer DOTDOT unsigned_integer \n");
	$$ = (struct range_t*) malloc(sizeof(struct range_t));
	$$->min = $1;
	$$->max = $3;
	// TODO: verify min <= max ?
	}
 ;

variable_declaration_part : VAR variable_declaration_list semicolon
	{
	printf("variable_declaration_part : VAR variable_declaration_list semicolon \n");
	// $$->vd = ?
	// $$->next = ?
	}
 |
	{
	printf("variable_declaration_part :  \n");
	// $$->vd = ?
	// $$->next = ?
	$$->vd = NULL;
	$$->next = NULL;
	}
 ;

variable_declaration_list : variable_declaration_list semicolon variable_declaration
	{
	printf("variable_declaration_list : variable_declaration_list semicolon variable_declaration \n");
	// $$->next = ?
	$$->next = $1;

	$$->vd = $3;
	}
 | variable_declaration
	{
	printf("variable_declaration_list : variable_declaration \n");
	// $$->next = ?
	$$->next = NULL;
	$$->vd = $1;
	}

 ;

variable_declaration : identifier_list COLON type_denoter
	{
	printf("variable_declaration : identifier_list COLON type_denoter \n");
	$$->il = $1;
	$$->tden = $3;
	$$->line_number = line_number;
	}
 ;

func_declaration_list : func_declaration_list semicolon function_declaration
	{
	printf("func_declaration_list : func_declaration_list semicolon function_declaration \n");
	$$ = (struct func_declaration_list_t*) malloc(sizeof(struct func_declaration_list_t));
	// $$->next = ?
	$$->next = $1;
	$$->fd = $3;
	}
 | function_declaration
	{
	printf("func_declaration_list : function_declaration \n");
	$$ = (struct func_declaration_list_t*) malloc(sizeof(struct func_declaration_list_t));
	// $$->next = ?
	$$->next = NULL;
	$$->fd = $1;
	}
 |
	{
	printf("func_declaration_list :  \n");
	$$ = (struct func_declaration_list_t*) malloc(sizeof(struct func_declaration_list_t));

	$$->next = NULL;
	$$->fd = NULL;
	}
 ;

formal_parameter_list : LPAREN formal_parameter_section_list RPAREN 
	{
	printf("formal_parameter_list : LPAREN formal_parameter_section_list RPAREN  \n");
	}
;
formal_parameter_section_list : formal_parameter_section_list semicolon formal_parameter_section
	{
	printf("formal_parameter_section_list : formal_parameter_section_list semicolon formal_parameter_section \n");
	$$->next = $1;
	$$->fps = $3;
	}
 | formal_parameter_section
	{
	printf("formal_parameter_section_list : formal_parameter_section \n");
	$$->next = NULL;
	$$->fps = $1;
	}
 ;

formal_parameter_section : value_parameter_specification
	{
	printf("formal_parameter_section : value_parameter_specification \n");
	
	}
 | variable_parameter_specification
 	{
	printf("formal_parameter_section : variable_parameter_specification \n");
 	}
 ;

value_parameter_specification : identifier_list COLON identifier
	{
	printf("value_parameter_specification : identifier_list COLON identifier \n");
	// The previous rule has nothing that needs to be done?

	// $$->next = ?
	$$->id = $3;
	}
 ;

variable_parameter_specification : VAR identifier_list COLON identifier
	{
	printf("variable_parameter_specification : VAR identifier_list COLON identifier \n");

	}
 ;

function_declaration : function_identification semicolon function_block
	{
	printf("function_declaration : function_identification semicolon function_block \n");

	}
 | function_heading semicolon function_block
	{
	printf("function_declaration : function_heading semicolon function_block \n");

	}
 ;

function_heading : FUNCTION identifier COLON result_type
	{
	printf("function_heading : FUNCTION identifier COLON result_type \n");

	}
 | FUNCTION identifier formal_parameter_list COLON result_type
	{
	printf("function_heading : FUNCTION identifier formal_parameter_list COLON result_type \n");

	}
 ;

result_type : identifier
	{
	printf("result_type : identifier \n");
	}
 ;

function_identification : FUNCTION identifier
	{
	printf("function_identification : FUNCTION identifier \n");

	}
;

function_block : 
  variable_declaration_part
  statement_part
	{
	printf("function_block : variable_declaration_part statement_part \n");

	}
;

statement_part : compound_statement
	{
	printf("statement_part : compound_statement \n");
	}
 ;

compound_statement : PBEGIN statement_sequence END
	{
	printf("compound_statement : PBEGIN statement_sequence END \n");

	}
 ;

statement_sequence : statement
	{
	printf("statement_sequence : statement \n");

	}
 | statement_sequence semicolon statement
	{
	printf("statement_sequence : statement_sequence semicolon statement \n");

	}
 ;

statement : assignment_statement
	{
	printf("statement : assignment_statement \n");

	}
 | compound_statement
	{
	printf("statement : compound_statement \n");

	}
 | if_statement
	{
	printf("statement : if_statement \n");

	}
 | while_statement
	{
	printf("statement : while_statement \n");

	}
 | print_statement
        {
	printf("statement : print_statement \n");

        }
 ;

while_statement : WHILE boolean_expression DO statement
	{
	printf("while_statement : WHILE boolean_expression DO statement \n");

	}
 ;

if_statement : IF boolean_expression THEN statement ELSE statement
	{
	printf("if_statement : IF boolean_expression THEN statement ELSE statement \n");

	}
 ;

assignment_statement : variable_access ASSIGNMENT expression
	{
	printf("assignment_statement : variable_access ASSIGNMENT expression \n");

	}
 | variable_access ASSIGNMENT object_instantiation
	{
	printf("assignment_statement : variable_access ASSIGNMENT object_instantiation \n");

	}
 ;

object_instantiation: NEW identifier
	{
	printf("object_instantiation: NEW identifier \n");

	}
 | NEW identifier params
	{
	printf("object_instantiation: NEW identifier params \n");

	}
;

print_statement : PRINT variable_access
        {
	printf("print_statement : PRINT variable_access \n");

        }
;

variable_access : identifier
	{
	printf("variable_access : identifier \n");

	}
 | indexed_variable
	{
	printf("variable_access : indexed_variable \n");

	}
 | attribute_designator
	{
	printf("variable_access : attribute_designator \n");

	}
 | method_designator
	{
	printf("variable_access : method_designator \n");

	}
 ;

indexed_variable : variable_access LBRAC index_expression_list RBRAC
	{
	printf("indexed_variable : variable_access LBRAC index_expression_list RBRAC \n");

	}
 ;

index_expression_list : index_expression_list comma index_expression
	{
	printf("index_expression_list : index_expression_list comma index_expression \n");

	}
 | index_expression
	{
	printf("index_expression_list : index_expression \n");

	}
 ;

index_expression : expression
	{
	printf("index_expression : expression \n");

	} ;

attribute_designator : variable_access DOT identifier
	{
	printf("attribute_designator : variable_access DOT identifier \n");

	}
;

method_designator: variable_access DOT function_designator
	{
	printf("method_designator: variable_access DOT function_designator \n");

	}
 ;


params : LPAREN actual_parameter_list RPAREN 
	{
	printf("params : LPAREN actual_parameter_list RPAREN  \n");

	}
 ;

actual_parameter_list : actual_parameter_list comma actual_parameter
	{
	printf("actual_parameter_list : actual_parameter_list comma actual_parameter \n");

	}
 | actual_parameter 
	{
	printf("actual_parameter_list : actual_parameter \n");

	}
 ;

actual_parameter : expression
	{
	printf("actual_parameter : expression \n");

	}
 | expression COLON expression
	{
	printf("actual_parameter : expression COLON expression \n");

	}
 | expression COLON expression COLON expression
	{
	printf("actual_parameter : expression COLON expression COLON expression \n");

	}
 ;

boolean_expression : expression
	{
	printf("boolean_expression : expression \n");

	} ;

expression : simple_expression
	{
	printf("expression : simple_expression \n");

	}
 | simple_expression relop simple_expression
	{
	printf("expression : simple_expression relop simple_expression \n");

	}
 ;

simple_expression : term
	{
	printf("simple_expression : term \n");

	}
 | simple_expression addop term
	{
	printf("simple_expression : simple_expression addop term \n");

	}
 ;

term : factor
	{
	printf("term : factor \n");

	}
 | term mulop factor
	{
	printf("term : term mulop factor \n");

	}
 ;

sign : PLUS
	{
	printf("sign : PLUS \n");

	}
 | MINUS
	{
	printf("sign : MINUS \n");

	}
 ;

factor : sign factor
	{

	}
 | primary 
	{

	}
 ;

primary : variable_access
	{

	}
 | unsigned_constant
	{

	}
 | function_designator
	{

	}
 | LPAREN expression RPAREN
	{

	}
 | NOT primary
	{

	}
 ;

unsigned_constant : unsigned_number
 ;

unsigned_number : unsigned_integer ;

unsigned_integer : DIGSEQ
	{

	}
 ;

/* functions with no params will be handled by plain identifier */
function_designator : identifier params
	{

	}
 ;

addop: PLUS
	{

	}
 | MINUS
	{

	}
 | OR
	{

	}
 ;

mulop : STAR
	{

	}
 | SLASH
	{

	}
 | MOD
	{

	}
 | AND
	{

	}
 ;

relop : EQUAL
	{

	}
 | NOTEQUAL
	{

	}
 | LT
	{

	}
 | GT
	{

	}
 | LE
	{

	}
 | GE
	{

	}
 ;

identifier : IDENTIFIER
	{
	$$ = (char*) malloc((strlen(yytext) + 1));
	strcpy($$, yytext);
	}
 ;

semicolon : SEMICOLON
	{

	}
 ;

comma : COMMA
	{

	}
 ;

%%

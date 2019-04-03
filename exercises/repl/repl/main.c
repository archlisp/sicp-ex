#define _CRT_SECURE_NO_DEPRECATE
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sexp.h"

typedef enum
{
  done
} Continues;

typedef struct
{
  char* Variable;
  Sexp* Value;
  struct Binding* NextBinding;
} Binding;

typedef struct
{
  Binding* Bindings;
  struct Frame* NextFrame;
} Frame;

/* =====Main===== */
int main();
int intpower(int base, int exponent);
void EvalTest(char* raw, Sexp expectedVal);
void ParseTest(char* raw, Sexp expectedParse);

/* =====Parsing===== */
Sexp Parse(char* raw, int startIndex, int stopIndex);
Sexp ParseNumber(char* raw, int startIndex, int stopIndex);
Sexp ParseString(char* raw, int startIndex, int stopIndex);
Sexp ParseSymbol(char* raw, int startIndex, int stopIndex);

/* =====Scheme Procedures===== */
int numberp(Sexp s);
int stringp(Sexp s);

/* =====4.1===== */
Sexp lookup_variable_value(Sexp var, Frame env);
Frame setup_environment();
int self_evaluatingp(Sexp exp);
int variablep(Sexp s);

/* =====5.32===== */
Sexp start(Sexp e);

/* =====Main===== */
int main()
{
  ParseTest("42", MakeNumber(42));
  ParseTest("501", MakeNumber(501));
  ParseTest("-11", MakeNumber(-11));
  ParseTest("\"abc\"", MakeString("abc", 0, 2));
  ParseTest("xyz", MakeSymbol("xyz", 0, 2));

  // 4.1 Tests
  EvalTest("5", MakeNumber(5));
  EvalTest("\"abc\"", MakeString("abc", 0, 2));

  /*
  (define-test (eval-one ''(+ 2 2)) '(+ 2 2))
  (define-test (eval-one '(begin (define x 5)
                                  (set! x 6)
                                  x))
               6)
  (define-test (eval-one '(begin (define x 5) x)) 5)
  (define-test (eval-one '(begin (define (x) 5) (x))) 5)
  (define-test (eval-one '(if 0 1 2)) 1)
  (define-test (eval-one '(if true 1 2)) 1)
  (define-test (eval-one '(if false 1 2)) 2)
  (define-test (eval-one '(begin
                             (define plus-one (lambda (x) (+ x 1)))
                             (plus-one 1)))
                2)
  (define-test (eval-one '(begin 1 2 3 4)) 4)
  (define-test (eval-one '(cond [else 0])) 0)
  (define-test (eval-one '(cond [true 0] [false 1] [else 2])) 0)
  (define-test (eval-one '(cond [false 0] [true 1] [else 2])) 1)
  (define-test (eval-one '(cond [false 0] [false 1] [else 2])) 2)
  (define-test (eval-one '(+ 2 2)) 4)
  */

  return 0;
}

int intpower(int base, int exponent)
{
  int result = 1;
  for (int i = 0; i < exponent; i++)
  {
    result *= base;
  }
  return result;
}

void EvalTest(char* raw, Sexp expectedVal)
{
  Sexp parsed = Parse(raw, 0, strlen(raw) - 1);
  if (parsed.Type == Error)
  {
    printf("Fail\n");
    return;
  }

  Sexp result = start(parsed);
  if (AreEqual(result, expectedVal))
  {
    printf("Pass\n");
  }
  else
  {
    printf("Fail\n");
  }
}

void ParseTest(char* raw, Sexp expectedParse)
{
  Sexp parsed = Parse(raw, 0, strlen(raw) - 1);
  if (AreEqual(parsed, expectedParse))
  {
    printf("Pass\n");
  }
  else
  {
    printf("Fail\n");
  }
}

/* =====Parsing===== */

Sexp Parse(char* raw, int startIndex, int stopIndex)
{
  for (int i = startIndex; i <= stopIndex; i++)
  {
    char c = raw[i];
    if (c == ' ' || c == '\r' || c == '\n') 
    {
      continue;
    }
    else if (c == '-' || c == '0' || c == '1' || c == '2' || c == '3' || c == '4' || c == '5' || c == '6' || c == '7' || c == '8' || c == '9')
    {
      return ParseNumber(raw, i, stopIndex);
    }
    else if (c == '"')
    {
      return ParseString(raw, i, stopIndex);
    }
    else if (c == '(')
    {
      // Pair (?)
    }
    else
    {
      return ParseSymbol(raw, i, stopIndex);
    }
  }

  return MakeError("not implemented", -1, -1);
}

Sexp ParseNumber(char* raw, int startIndex, int stopIndex)
{
  int neg = raw[startIndex] == '-';
  if (neg)
  {
    startIndex++;
  }
  int place = intpower(10, stopIndex - startIndex);
  int value = 0;

  for (int i = startIndex; i <= stopIndex; i++)
  {
    char c = raw[i];

    int digit;
    if (c == '0') { digit = 0; }
    else if (c == '1') { digit = 1; }
    else if (c == '2') { digit = 2; }
    else if (c == '3') { digit = 3; }
    else if (c == '4') { digit = 4; }
    else if (c == '5') { digit = 5; }
    else if (c == '6') { digit = 6; }
    else if (c == '7') { digit = 7; }
    else if (c == '8') { digit = 8; }
    else if (c == '9') { digit = 9; }
    else
    {
      return MakeError("non digit char", -1, -1);
    }

    value += place * digit;

    place /= 10;
  }

  if (neg)
  {
    value = -1 * value;
  }

  return MakeNumber(value);
}

Sexp ParseString(char* raw, int startIndex, int stopIndex)
{
  int openQuoteIndex = raw[startIndex] == '"' ? startIndex : -1;
  if (openQuoteIndex == -1)
  {
    return MakeError("exepected expression to begin with double quote", -1, -1);
  }
  int firstCharIndex = startIndex + 1;

  int closeQuoteIndex = -1;
  for (int i = firstCharIndex; i <= stopIndex; i++)
  {
    if (raw[i] == '"')
    {
      closeQuoteIndex = i;
      break;
    }
  }
  if (closeQuoteIndex == -1)
  {
    return MakeError("can't find matching double quote", -1, -1);
  }
  int lastCharIndex = closeQuoteIndex - 1;

  return MakeString(raw, firstCharIndex, lastCharIndex);
}

Sexp ParseSymbol(char* raw, int startIndex, int stopIndex)
{
  int firstCharIndex = startIndex;
  int lastCharIndex = stopIndex;

  for (int i = startIndex; i <= stopIndex; i++)
  {
    if (raw[i] == ' ')
    {
      lastCharIndex = i - 1;
      break;
    }
  }

  return MakeSymbol(raw, firstCharIndex, lastCharIndex);
}

/* =====Scheme Procedures===== */

int numberp(Sexp s)
{
  return s.Type == Number;
}

int stringp(Sexp s)
{
  return s.Type == String;
}

/* =====4.1===== */
int self_evaluatingp(Sexp s)
{
  return numberp(s) || stringp(s);
}

Frame setup_environment()
{
  Frame ret;
  ret.Bindings = NULL;
  ret.NextFrame = NULL;
  return ret;
}

Sexp lookup_variable_value(Sexp var, Frame env)
{
  if (var.Type != Symbol)
  {
    return MakeError("lookup_variable_value -- can only lookup a symbol", -1, -1);
  }

  char* name = GetStringValue(var);

  Binding* binding = env.Bindings;
  while (binding != NULL)
  {
    if (strcmp(name, binding->Variable) == 0)
    {
      return *(binding->Value);
    }
    binding = binding->NextBinding;
  }

  if (env.NextFrame == NULL)
  {
    return MakeError("lookup_variable_value -- var not defined", -1, -1);
  }

  Frame* next = env.NextFrame;
  return lookup_variable_value(var, *next);
}

int variablep(Sexp s)
{
  return s.Type == Symbol;
}

/* =====5.32===== */
Sexp start(Sexp e)
{
  // "Registers"
  Continues compapp = done;
  Continues cont = done;
  Frame env = setup_environment();
  Sexp exp = e;
  Sexp val = { .Type = Error };

// (assign compapp (label compound_apply))
// (branch (label external_entry))
 
  regular_eval:
    cont = done;

  eval_dispatch:
    if (self_evaluatingp(exp))
      goto ev_self_eval;
    if (variablep(exp))
      goto ev_variable;
//   (test (op quoted?) (reg exp))
//   (branch (label ev_quoted))
//   (test (op assignment?) (reg exp))
//   (branch (label ev_assignment))
//   (test (op definition?) (reg exp))
//   (branch (label ev_definition))
//   (test (op if?) (reg exp))
//   (branch (label ev_if))
//   (test (op lambda?) (reg exp))
//   (branch (label ev_lambda))
//   (test (op begin?) (reg exp))
//   (branch (label ev_begin))
//   (test (op cond?) (reg exp))
//   (branch (label ev_cond))
//   (test (op is_and?) (reg exp))
//   (branch (label ev_and))
//   (test (op is_or?) (reg exp))
//   (branch (label ev_or))
//   (test (op is_let?) (reg exp))
//   (branch (label ev_let))
//   (test (op is_let_star?) (reg exp))
//   (branch (label ev_let_star))
//   (test (op is_compile?) (reg exp))
//   (branch (label ev_compile))
//   (test (op symbol_application?) (reg exp))
//   (branch (label ev_symbol_application) (reg exp))
//   (test (op application?) (reg exp))
//   (branch (label ev_application))
//   (goto (label unknown_expression_type))

  ev_self_eval:
    val = exp;
    goto continue_;

  ev_variable:
    val = lookup_variable_value(exp, env);
    goto continue_;

  ev_quoted:
//   (assign val (op text_of_quotation) (reg exp))
    goto continue_;

  ev_lambda:
//   (assign unev (op lambda_parameters) (reg exp))
//   (assign exp (op lambda_body) (reg exp))
//   (assign val (op make_procedure) (reg unev) (reg exp) (reg env))
    goto continue_;

  ev_symbol_application:
//   (save continue)
//   (assign unev (op operands) (reg exp))
//   (assign exp (op operator) (reg exp))
//   (assign continue (label ev_appl_did_operator_no_restore))
    goto eval_dispatch;

  ev_application:
//   (save continue)
//   (save env)
//   (assign unev (op operands) (reg exp))
//   (save unev)
//   (assign exp (op operator) (reg exp))
//   (assign continue (label ev_appl_did_operator))
//   (goto (label eval_dispatch))
  ev_appl_did_operator:
//   (restore unev)                  ; the operands
//   (restore env)
  ev_appl_did_operator_no_restore:
//   (assign argl (op empty_arglist))
//   (assign proc (reg val))         ; the operator
//   (test (op no_operands?) (reg unev))
//   (branch (label apply_dispatch))
//   (save proc)
  ev_appl_operand_loop:
//   (save argl)
//   (assign exp (op first_operand) (reg unev))
//   (test (op last_operand?) (reg unev))
//   (branch (label ev_appl_last_arg))
//   (save env)
//   (save unev)
//   (assign continue (label ev_appl_accumulate_arg))
//   (goto (label eval_dispatch))
  ev_appl_accumulate_arg:
//   (restore unev)
//   (restore env)
//   (restore argl)
//   (assign argl (op adjoin_arg) (reg val) (reg argl))
//   (assign unev (op rest_operands) (reg unev))
//   (goto (label ev_appl_operand_loop))
  ev_appl_last_arg:
//   (assign continue (label ev_appl_accum_last_arg))
//   (goto (label eval_dispatch))
  ev_appl_accum_last_arg:
//   (restore argl)
//   (assign argl (op adjoin_arg) (reg val) (reg argl))
//   (restore proc)
//   (goto (label apply_dispatch))
// 
  apply_dispatch:
//   (test (op primitive_procedure?) (reg proc))
//   (branch (label primitive_apply))
//   (test (op compound_procedure?) (reg proc))
//   (branch (label compound_apply))
//   (test (op compiled_procedure?) (reg proc)) ; added for 5.47
//   (branch (label compiled_apply)) ; added for 5.47
//   (goto (label unknown_procedure_type))
  primitive_apply:
//   (assign val (op apply_primitive_procedure) (reg proc) (reg argl))
//   (restore continue)
    goto continue_;
  compound_apply:
//   (assign unev (op procedure_parameters) (reg proc))
//   (assign env (op procedure_environment) (reg proc))
//   (assign env (op extend_environment) (reg unev) (reg argl) (reg env))
//   (assign unev (op procedure_body) (reg proc))
    goto ev_sequence;
// compiled_apply:
//   (restore continue)
//   (assign val (op compiled_procedure_entry) (reg proc))
//   (goto (reg val))

  ev_begin:
//   (assign unev (op begin_actions) (reg exp))
//   (save continue)
    goto ev_sequence;
  ev_sequence:
//   (assign exp (op first_exp) (reg unev))
//   (test (op last_exp?) (reg unev))
//   (branch (label ev_sequence_last_exp))
//   (save unev)
//   (save env)
//   (assign continue (label ev_sequence_continue))
    goto eval_dispatch;
  ev_sequence_continue:
//   (restore env)
//   (restore unev)
//   (assign unev (op rest_exps) (reg unev))
    goto ev_sequence;
  ev_sequence_last_exp:
//   (restore continue)
    goto eval_dispatch;

  ev_if:
//   (save exp)                    ; save expression for later
//   (save env)
//   (save continue)
//   (assign continue (label ev_if_decide))
//   (assign exp (op if_predicate) (reg exp))
    goto eval_dispatch;
  ev_if_decide:
//   (restore continue)
//   (restore env)
//   (restore exp)
//   (test (op true?) (reg val))
//   (branch (label ev_if_consequent))
  ev_if_alternative:
//   (assign exp (op if_alternative) (reg exp))
    goto eval_dispatch;
  ev_if_consequent:
//   (assign exp (op if_consequent) (reg exp))
    goto eval_dispatch;
// 
  ev_assignment:
//   (assign unev (op assignment_variable) (reg exp))
//   (save unev)                   ; save variable for later
//   (assign exp (op assignment_value) (reg exp))
//   (save env)
//   (save continue)
//   (assign continue (label ev_assignment_1))
    goto eval_dispatch;
  ev_assignment_1:
//   (restore continue)
//   (restore env)
//   (restore unev)
//   (perform (op set_variable_value!) (reg unev) (reg val) (reg env))
//   (assign val (const ok))
    goto continue_;

  ev_definition:
//   (assign unev (op definition_variable) (reg exp))
//   (save unev)                   ; save variable for later
//   (assign exp (op definition_value) (reg exp))
//   (save env)
//   (save continue)
//   (assign continue (label ev_definition_1))
    goto eval_dispatch;
  ev_definition_1:
//   (restore continue)
//   (restore env)
//   (restore unev)
//   (perform (op define_variable!) (reg unev) (reg val) (reg env))
//   (assign val (const ok))
    goto continue_;

  ev_and:
// (assign exp (op and_clauses) (reg exp))
// (assign val (op eval_and) (reg exp) (reg env))
    goto continue_;

  ev_or:
// (assign exp (op or_clauses) (reg exp))
// (assign val (op eval_or) (reg exp) (reg env))
    goto continue_;

  ev_let:
// (assign exp (op let_>combination) (reg exp))
    goto eval_dispatch;

  ev_let_star:
// (assign exp (op let_star_>nested_lets) (reg exp))
    goto eval_dispatch;

  ev_cond:
// (assign exp (op cond_clauses) (reg exp))   ; exp is clauses
  ev_cond_loop:
// (assign val (op car) (reg exp))            ; val is 1st clause
// (test (op cond_else_clause?) (reg val))    ; if 1st clause is else
// (branch (label ev_cond_evaluate_actions))  ;   eval 1st clause actions
// (save continue)
// (save exp)
// (assign continue (label ev_cond_after_pred))
// (assign exp (op cond_predicate) (reg val))
    goto eval_dispatch;
  ev_cond_after_pred:
// (restore exp)                              ; exp is clauses
// (restore continue)
// (test (op true?) (reg val))                ; if the predicate is true
// (branch (label ev_cond_evaluate_actions))  ;  eval 1st clause actions
// (assign exp (op cdr) (reg exp))
    goto ev_cond_loop;

  ev_cond_evaluate_actions:
// (assign exp (op car) (reg exp))                ; exp is clause
// (test (op cond_is_arrow?) (reg exp))
// (branch (label ev_cond_arrow_actions))

  ev_cond_regular_actions:
// (assign exp (op cond_actions) (reg exp))       ; exp is actions of the clause
// (assign exp (op cons) (const begin) (reg exp)) ; exp is actions prepended with "begin"
    goto eval_dispatch;

  ev_cond_arrow_actions:
// (assign val (op cons) (reg val) (const ())) ; val is (predicate_value)
// (assign exp (op cond_arrow_proc) (reg exp)) ; exp is the proc of the cond_arrow
// (assign exp (op cons) (reg exp) (reg val)) ; exp is (proc predicate_value)
    goto eval_dispatch;

  unknown_expression_type:
//   (perform (op error) (const "unknown expression type") (reg exp))

  unknown_procedure_type:
//   (perform (op error) (const "unknown procedure type") (reg proc))

// external_entry:
//   (assign continue (label regular_eval))
//   (goto (reg val))

// ev_compile:
//   (assign val (op compile_exp) (reg exp))
//   (assign val (op compile) (reg val))
//   (goto (reg val))

  done:
    return val;

  // NEW -- Avoid using labels as values
  continue_:
    switch (cont)
    {
      case done: goto done;
    }
}

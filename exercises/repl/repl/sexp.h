#pragma once

typedef enum
{
  Bool,
  Error,
  Number,
  Pair,
  String,
  Symbol
} SexpTypes;

typedef struct
{
  SexpTypes Type;
  void* Value;
} Sexp;

typedef struct
{
  Sexp* Car;
  Sexp* Cdr;
} Cell;

int AreEqual(Sexp a, Sexp b);
int GetBoolValue(Sexp s);
char* GetErrorValue(Sexp s);
int GetNumberValue(Sexp s);
Cell* GetPairValue(Sexp s);
char* GetStringValue(Sexp s);
char* GetSymbolValue(Sexp s);
Sexp MakeBool(int value);
Sexp MakeError(char* raw, int startIndex, int stopIndex);
Sexp MakeNumber(int value);
Sexp MakePair(Cell* value);
Sexp MakeString(char* raw, int startIndex, int stopIndex);
Sexp MakeSymbol(char* raw, int startIndex, int stopIndex);

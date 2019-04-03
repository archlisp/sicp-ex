#include "sexp.h"

/* Infrastructure */

void* PackInt(int value)
{
  int* ret = malloc(sizeof(int));
  *ret = value;
  return ret;
}

void* PackCharPtr(char* raw, int startIndex, int stopIndex)
{
  int len = stopIndex - startIndex + 1;
  char* value = malloc((len + 1) * sizeof(char));

  int i = 0;
  for (; i < len; i++)
  {
    value[i] = raw[i + startIndex];
  }
  value[i] = '\0';

  return value;
}

static int UnpackInt(Sexp s) 
{
  return *((int*)s.Value); 
}

static char* UnpackCharPtr(Sexp s)
{
  return (char*)s.Value; 
}

static Cell* UnpackCellPtr(Sexp s)
{
  return (char*)s.Value;
}

/* Implementation */

int AreEqual(Sexp a, Sexp b)
{
  if ((a.Type == Bool && b.Type == Bool) || 
      (a.Type == Number && b.Type == Number))
  {
    return UnpackInt(a) == UnpackInt(b);
  }
  else if ((a.Type == Error && b.Type == Error) || 
           (a.Type == String && b.Type == String) ||
           (a.Type == Symbol && b.Type == Symbol))
  {
    return strcmp(UnpackCharPtr(a), UnpackCharPtr(b)) == 0;
  }
  else if (a.Type == Pair && b.Type == Pair)
  {
    return 0; // TODO: Fix
  }

  return 0;
}

int GetBoolValue(Sexp s)
{
  return UnpackInt(s);
}

char* GetErrorValue(Sexp s) 
{
  return UnpackCharPtr(s);
}

int GetNumberValue(Sexp s)
{
  return UnpackInt(s);
}

Cell* GetPairValue(Sexp s)
{
  return UnpackCellPtr(s);
}

char* GetStringValue(Sexp s)
{
  return UnpackCharPtr(s);
}

char* GetSymbolValue(Sexp s)
{
  return UnpackCharPtr(s);
}

Sexp MakeBool(int value)
{
  Sexp ret =
  {
    .Type = Bool,
    .Value = PackInt(value)
  };
  return ret;
}

Sexp MakeError(char* raw, int startIndex, int stopIndex)
{
  Sexp ret =
  {
    .Type = Error,
    .Value = PackCharPtr(raw, startIndex, stopIndex)
  };
  return ret;
}

Sexp MakeNumber(int value)
{
  Sexp ret =
  {
    .Type = Number,
    .Value = PackInt(value)
  };
  return ret;
}

Sexp MakePair(Cell* value)
{
  Sexp ret =
  {
    .Type = Pair,
    .Value = value
  };
  return ret;
}

Sexp MakeString(char* raw, int startIndex, int stopIndex)
{
  Sexp ret =
  {
    .Type = String,
    .Value = PackCharPtr(raw, startIndex, stopIndex)
  };
  return ret;
}

Sexp MakeSymbol(char* raw, int startIndex, int stopIndex)
{
  Sexp ret =
  {
    .Type = Symbol,
    .Value = PackCharPtr(raw, startIndex, stopIndex)
  };
  return ret;
}

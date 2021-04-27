// closeleg.h
// Siegfried Nijssen, snijssen@liacs.nl, feb 2004.
#ifndef CLOSELEG_H
#define CLOSELEG_H
#include <vector>
#include "legoccurrence.h"



struct CloseTuple {
  EdgeLabel label;
  int from;
  int to;
  friend bool operator< ( CloseTuple &a, CloseTuple &b ) { return a.from < b.from || ( a.from == b.from && ( a.to < b.to || ( a.to == b.to && a.label < b.label ) ) ); }
  friend bool operator> ( CloseTuple &a, CloseTuple &b ) { return a.from > b.from || ( a.from == b.from && ( a.to > b.to || ( a.to == b.to && a.label > b.label ) ) ); }
  friend ostream &operator<< ( ostream &stream, CloseTuple &tuple ) { 
    stream << (int) tuple.from << " " << tuple.to << " " << (int) tuple.label << endl;
    return stream;
  }
  CloseTuple(){};
  CloseTuple(EdgeLabel l, int f, int t):label(l), from(f), to(t){};
};

struct CloseLegOccurrence {
  Tid tid;
  OccurrenceId occurrenceid;

  CloseLegOccurrence ( Tid tid, OccurrenceId occurrenceid ): tid ( tid ), occurrenceid ( occurrenceid ) { }
  CloseLegOccurrence () { }

  CloseLegOccurrence(const CloseLegOccurrence& obj){
	  tid = obj.tid;
	  occurrenceid = obj.occurrenceid;
  }
};

struct CloseLegOccurrences {
  Frequency frequency;
  vector<CloseLegOccurrence> elements;
  CloseLegOccurrences () : frequency ( 0 ) { }


  CloseLegOccurrences(const CloseLegOccurrences& obj){
	  frequency = obj.frequency;
	  elements.assign(obj.elements.begin(), obj.elements.end());
  }



  CloseLegOccurrences& operator = (const CloseLegOccurrences& obj){
	  if (this == &obj)
		  return *this;
	  frequency = obj.frequency;
	  elements.assign(obj.elements.begin(), obj.elements.end());
	  return *this;
  }
};

typedef CloseLegOccurrences *CloseLegOccurrencesPtr;

struct CloseLeg {
  bool copy;
  CloseTuple tuple;
  CloseLegOccurrences occurrences;
  CloseLeg (): copy ( true ) { }
};

typedef CloseLeg *CloseLegPtr;

class Leg;
typedef Leg *LegPtr;



#endif

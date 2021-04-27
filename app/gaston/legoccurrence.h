// legoccurrence.h
// Siegfried Nijssen, snijssen@liacs.nl, jan 2004.
#ifndef LEGOCCURRENCE_H
#define LEGOCCURRENCE_H
#include <iostream>
#include <vector>
#include <list>
#include "misc.h"

using namespace std;

typedef unsigned int OccurrenceId;

extern TimeTracker tt1,tt2,tt3,tt4,tt5;

struct LegOccurrences;

struct LegOccurrence {
  Tid tid;
  OccurrenceId occurrenceid;
  NodeId tonodeid, fromnodeid; // by wenwen: the nth time occurrence
  vector<int> parent_tonodeid;

  LegOccurrence ( Tid tid, OccurrenceId occurrenceid, NodeId tonodeid, NodeId fromnodeid, LegOccurrence & parent)://todo change all
    tid ( tid ), occurrenceid ( occurrenceid ), tonodeid ( tonodeid ), fromnodeid ( fromnodeid ) {
	//tt1.start();
	  parent_tonodeid = parent.parent_tonodeid;
	  parent_tonodeid.push_back(parent.fromnodeid);
	 //tt1.stop();
  }

  LegOccurrence ( Tid tid, OccurrenceId occurrenceid, NodeId tonodeid, NodeId fromnodeid ):
    tid ( tid ), occurrenceid ( occurrenceid ), tonodeid ( tonodeid ), fromnodeid ( fromnodeid ) { }
  LegOccurrence () {}

  friend ostream &operator<< ( ostream &stream, LegOccurrence &occ );

  LegOccurrence(const LegOccurrence& obj){
	  tid = obj.tid;
	  occurrenceid = obj.occurrenceid;
	  tonodeid= obj.tonodeid;
	  fromnodeid = obj.fromnodeid;
	  parent_tonodeid = obj.parent_tonodeid;
  }
};
typedef LegOccurrences *LegOccurrencesPtr;

struct LegOccurrences {
  vector<LegOccurrence> elements;
  //LegOccurrencesPtr parent;
  //vector<vector<LegOccurrence> > parent; // by wenwen : parent pointer may have released in this pattern
  int number;
  Frequency selfjoin; // by wenwen : what is a sefjoin
  short unsigned int maxdegree;
  Frequency frequency;
  LegOccurrences () : frequency ( 0 ), selfjoin ( 0 ) { }


  LegOccurrences(const LegOccurrences& obj){
	  elements = obj.elements;
	  //parent = obj.parent;
	  number = obj.number;
	  selfjoin = obj.selfjoin;
	  maxdegree = obj.maxdegree;
	  frequency = obj.frequency;

  }

  LegOccurrences(vector<LegOccurrence>& els, int num){
          elements = els;
          number = num;
  }

  LegOccurrences& operator = (const LegOccurrences& obj){
	  if(this == &obj)
		  return *this;
	  elements = obj.elements;
	  //parent = obj.parent;
	  number = obj.number;
	  selfjoin = obj.selfjoin;
	  maxdegree = obj.maxdegree;
	  frequency = obj.frequency;
	  return *this;
  }



  void parentPushBack(LegOccurrences& p){
	  tt1.start();
	  //parent = p.parent;
	  //parent.emplace_back(p.elements, p.number);
	  //for
	  /*
	  parent.resize(p.parent.size()+1);
	  LegOccurrences& tmp = parent.back();
	  tmp.elements = p.elements;
	  tmp.number = p.number;
	  */
	  cout << "tt1:" << tt1.stop() << endl;
  }
};

ostream &operator<< ( ostream &stream, vector<LegOccurrence> &occs );


// returns the join if this join is frequent. The returned array may be swapped.
LegOccurrencesPtr join ( LegOccurrences &legoccsdata1, NodeId connectingnode, LegOccurrences &legoccsdata2 );
LegOccurrencesPtr join ( LegOccurrences &legoccsdata );

void initLegStatics ();

void extend ( LegOccurrences &legoccurrencesdata ); // fills the global arrays above
void extend ( LegOccurrences &legoccurrencesdata, EdgeLabel minlabel, EdgeLabel neglect );

void sanityCheck ( LegOccurrencesPtr legoccurrencesptr );

#endif

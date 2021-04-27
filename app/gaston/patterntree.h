// patterntree.h
// Siegfried Nijssen, snijssen@liacs.nl, jan 2004.
#ifndef PATTERNTREE_H
#define PATTERNTREE_H
#include <iostream>
#include <vector>
#include "misc.h"
#include "database.h"
#include "legoccurrence.h"
#include "path.h"
#include "closeleg.h"

using namespace std;

struct Tuple {
  Depth depth;
  EdgeLabel label;
  NodeId connectingnode;
  
  Tuple ( Depth depth, EdgeLabel label, NodeId connectingnode ): 
    depth ( depth ), label ( label ), connectingnode ( connectingnode ) { }
  Tuple () { }

  Tuple(const Tuple& obj){
	  depth = obj.depth;
	  label = obj.label;
	  connectingnode = obj.connectingnode;
  }

  friend bool operator< ( Tuple &a, Tuple &b ) { return a.depth > b.depth || ( a.depth == b.depth && a.label < b.label ); }
  friend bool operator== ( Tuple &a, Tuple &b ) { return a.depth == b.depth && a.label == b.label; }
  friend ostream &operator<< ( ostream &stream, Tuple &tuple );
};

struct Leg {
  Tuple tuple;
  LegOccurrences occurrences;
};

typedef Leg *LegPtr;

class PatternTree {
  public:
	~PatternTree () {
	  for ( int i = 0; i < legs.size (); i++ )
	    delete legs[i];
	  for ( int i = 0; i < closelegs.size (); i++ )
	    delete closelegs[i];
	}
    void checkIfIndeedNormal () {
      int i = 0, j = nextpathstart;
      bool equal = true;
      for ( ; i <= maxdepth && equal; i++, j++ )
        equal = ( treetuples[i] == treetuples[j] );
      if ( !equal && treetuples[i-1].label > treetuples[j-1].label ) {
        cout << "NOT NORMAL: first path is higher than second" << endl;
      }
      for ( i = maxdepth + 1; i < nextpathstart; i++ ) {
        if ( treetuples[i].depth == maxdepth &&
             treetuples[i].label < treetuples[treetuples[i].depth].label )
          cout << "NOT NORMAL: lower path than possible (1)" << endl;
      }
      for ( i = nextpathstart + maxdepth + 1; i < treetuples.size (); i++ ) {
        if ( treetuples[i].depth == 0 ) {
          j = i; i = nextpathstart;
          equal = true;
          for ( ; j < treetuples.size () && equal; i++, j++ )
            equal = ( treetuples[i] == treetuples[j] );
          if ( !equal && treetuples[j-1].depth == maxdepth && treetuples[i-1].label > treetuples[j-1].label ) {
            cout << "NOT NORMAL: third path is lower than the second" << endl;
          }
          break;
        }
        if ( treetuples[i].depth == maxdepth &&
             treetuples[i].label < treetuples[treetuples[i].depth + nextpathstart].label )
          cout << "NOT NORMAL: lower path than possible (2)" << endl;
      }
    }

    vector<Tuple> treetuples;
    vector<NodeId> rightmostindexes;
    vector<short> rootpathrelations;
    unsigned int nextprefixindex;
    unsigned int rootpathstart;
    unsigned int nextpathstart;
    unsigned int maxdepth;
    int symmetric; // 0 == not symmetric, 1 == symmetric, even length path, 2 == symmetric, odd length path
    int secondpathleg;
    vector<LegPtr> legs; // pointers used to avoid copy-constructor during a resize of the vector
    vector<CloseLegPtr> closelegs;
    friend ostream &operator<< ( ostream &stream, PatternTree &patterntree );
#ifdef GRAPH_OUTPUT
    friend void fillMatrix ( int **A, int &nextnode, int rootnode, NodeLabel rootlabel, 
                  int startpos, int endpos, PatternTree &patterntree );
    NodeLabel tree1rootlabel, tree2rootlabel;
    EdgeLabel rootpathlabel;
#endif
};

#define NONEXTPREFIX ((unsigned int) -1)

#endif

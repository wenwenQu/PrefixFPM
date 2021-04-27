// path.h
// Siegfried Nijssen, snijssen@liacs.nl, jan 2004.
#ifndef PATH_H
#define PATH_H
#include <iostream>
#include <vector>
#include "misc.h"
#include "database.h"
#include "legoccurrence.h"
#include "closeleg.h"
//#include "patterntree.h"

using namespace std;

struct PathTuple {
  Depth depth;
  NodeId connectingnode;
  EdgeLabel edgelabel;
  NodeLabel nodelabel;
};

struct PathLeg {
  PathTuple tuple;
  LegOccurrences occurrences;
};

typedef PathLeg *PathLegPtr;

class Path {
  public:
	~Path () {
	  for ( int i = 0; i < legs.size (); i++ )
	    delete legs[i];
	  for ( int i = 0; i < closelegs.size (); i++ )
	    delete closelegs[i];
	}
    void expand ();
    friend class PatternTree;
    // ADDED
    bool isnormal ( EdgeLabel edgelabel ) {
      // symplistic quadratic algorithm
      int nodelabelssize = nodelabels.size (), step, add, start;

      edgelabels.push_back ( edgelabel );

      // if we would program it better, we would use the 'totalsymmetry' variable here;
      // however, to be quick & easy, we used a different coding here...
      int t = nodelabelssize - 1, r = 0;
      int symmetry;
      do {
        symmetry = nodelabels[t] - nodelabels[r];
        int nt = ( t + nodelabelssize - 1 ) % nodelabelssize;
        if ( !symmetry )
          symmetry = edgelabels[nt] - edgelabels[r];
        r = ( r + 1 ) % nodelabelssize;
        t = nt;
      }
      while ( symmetry == 0 && t != nodelabelssize - 1 );

      if ( symmetry < 0 ) {
        step = -1 + nodelabelssize ;
        add = -1 + nodelabelssize ;
        start = nodelabelssize - 1;
      }
      else {
        step = 1 + nodelabelssize;
        add = nodelabelssize ;
        start = 0;
      }
      for ( int i = 0; i < nodelabelssize; i++ ) {
        // starting positions for the new path
        int k = start, l = i, p;
        do {
          if ( nodelabels[l] < nodelabels[k] ) {
            edgelabels.pop_back ();
            return false;
          }
          if ( nodelabels[l] > nodelabels[k] )
            break;
          p = ( k + add ) % nodelabelssize;
          l = ( l + nodelabelssize - 1 ) % nodelabelssize;
          if ( edgelabels[l] < edgelabels[p] ) {
            edgelabels.pop_back ();
            return false;
          }
          if ( edgelabels[l] > edgelabels[p] )
            break;
          k = ( k + step ) % nodelabelssize;
        }
        while ( k != start );

        k = start, l = i;
        do {
          if ( nodelabels[l] < nodelabels[k] ) {
            edgelabels.pop_back ();
            return false;
          }
          if ( nodelabels[l] > nodelabels[k] )
            break;
          p = ( k + add ) % nodelabelssize;
          if ( edgelabels[l] < edgelabels[p] ) {
            edgelabels.pop_back ();
            return false;
          }
          if ( edgelabels[l] > edgelabels[p] )
            break;
          l = ( l + 1 ) % nodelabelssize;
          k = ( k + step ) % nodelabelssize;
        }
        while ( k != start );

      }
      edgelabels.pop_back ();
      return true;
    }
    void expand2 ();
    vector<PathLegPtr> legs; // pointers used to avoid copy-constructor during a resize of the vector
    vector<CloseLegPtr> closelegs; //??? what's closelegs?
    vector<NodeLabel> nodelabels;
    vector<EdgeLabel> edgelabels;
    int frontsymmetry; // which is lower, the front or front reverse?
    int backsymmetry; // which is lower, the back or back reverse?
    int totalsymmetry; // which is lower, from left to right, or the reverse?

    friend ostream &operator<< ( ostream &stream, Path &path );
};

ostream &operator<< ( ostream &stream, Path &path ) {
  stream << /* database.nodelabels[ */ (int) path.nodelabels[0] /* ].inputlabel; */ << " ";
  for ( int i = 0; i < path.edgelabels.size (); i++ ) {
    //stream << (char) ( path.edgelabels[i] + 'A' ) << path.nodelabels[i+1];
    stream << /*database.edgelabels[database.edgelabelsindexes[*/ (int) path.edgelabels[i] /*]].inputedgelabel */ << " " <<  /* database.nodelabels[ */ (int) path.nodelabels[i+1] /* ].inputlabel */ << " ";
  }
  return stream;
}

#endif

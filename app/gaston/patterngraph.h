// patterngraph.h
// Siegfried Nijssen, snijssen@liacs.nl, jan 2004.
#ifndef PATTERNGRAPH_H
#define PATTERNGRAPH_H
#include <vector>
#include "closeleg.h"

using namespace std;

class PatternGraph {
  public:
    vector<CloseLegPtr> closelegs;
    vector<CloseTuple> closetuples;
    vector<CloseLegPtr> closelegssource; //change vector* to vector by wenwen
    vector<Tuple> treetuples; // add to check is normal by wenwen
    int legindex;
    Frequency frequency;

    ~PatternGraph () {
      for ( int i = 0; i < closelegs.size (); i++ )
        delete closelegs[i];
      for ( int i = 0; i < closelegssource.size (); i++ )
        delete closelegssource[i];;
    }
};

#endif

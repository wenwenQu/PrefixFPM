// misc.h
// Siegfried Nijssen, snijssen@liacs.nl, jan 2004.
#ifndef MISC_H
#define MISC_H
#include <vector>
#include <stdio.h>
using namespace std;

typedef unsigned int EdgeLabel; // combined node-edge label of the input file.
typedef unsigned int NodeLabel;
typedef unsigned int NodeId;
typedef unsigned int Depth; // unsigned int is more efficient than short, but requires more memory...
typedef unsigned int Tid;
typedef unsigned int Frequency;

extern Frequency minfreq;

#define NOTID ((Tid) -1)
#define NOEDGELABEL ((EdgeLabel) -1)
#define MAXEDGELABEL NOEDGELABEL
#define NONODELABEL ((NodeLabel) -1)
#define NODEPTH ((Depth) -1)
#define NOLEG (-1)
#define NONODE ((NodeId) -1)

// this macro can be used when push_back-ing large structures. In stead of first allocating a local
// variable and pushing this, one first pushes and defines a reference to the space in the vector.
// This avoids re-allocation.
#define vector_push_back(_type,_vector,_var) (_vector).push_back ( _type () ); _type &_var = (_vector).back ();

// can be used to obtain a type when inserting into a map
#define map_insert_pair(_type) typedef decltype(_type) T##_type; pair<T##_type::iterator,bool>

#define store(a,b) { if ( (b).elements.capacity () - (b).elements.size () > (b).elements.size () / 2 ) (a) = (b); else swap ( (a), (b) ); }


void puti ( FILE *f, int i );



inline void setmax ( short unsigned int &a, short unsigned int b ) { if ( b > a ) a = b; }

class Statistics {
  public:
    vector<unsigned int> frequenttreenumbers;
    vector<unsigned int> frequentpathnumbers;
    vector<unsigned int> frequentgraphnumbers;

    void print () {
      int total = 0, total2 = 0, total3 = 0;
      for ( int i = 0; i < frequenttreenumbers.size (); i++ ) {
        cout << "Frequent " << i + 2
             << " cyclic graphs: " << frequentgraphnumbers[i]
             << " real trees: " << frequenttreenumbers[i]
             << " paths: " << frequentpathnumbers[i]
             << " total: " << frequentgraphnumbers[i] + frequenttreenumbers[i] + frequentpathnumbers[i] << endl;
        total += frequentgraphnumbers[i];
        total2 += frequenttreenumbers[i];
        total3 += frequentpathnumbers[i];
      }
      cout << "TOTAL:" << endl
           << "Frequent cyclic graphs: " << total << " real trees: " << total2 << " paths: " << total3 << " total: " << total + total2 + total3 << endl;
    }
};


Frequency minfreq = 1;
Statistics statistics;
bool dooutput = false;
int phase = 3;
int maxsize = ( 1 << ( sizeof(NodeId)*8 ) ) - 1; // safe default for the largest allowed pattern
atomic<int> ID(0);

void puti ( FILE *f, int i ) {
  char array[100];
  int k = 0;
  do {
    array[k] = ( i % 10 ) + '0';
    i /= 10;
    k++;
  }
  while ( i != 0 );
  do {
    k--;
    putc ( array[k], f );
  } while ( k );
}

#endif

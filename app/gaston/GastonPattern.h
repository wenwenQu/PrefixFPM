/*
 * GastonPattern.h
 *
 *  Created on: 2021Äê3ÔÂ15ÈÕ
 *      Author: Administrator
 */

#ifndef APP_GASTON_GASTONPATTERN_H_
#define APP_GASTON_GASTONPATTERN_H_
#include "GastonTrans.h"

extern TimeTracker tt1,tt2,tt3,tt4,tt5;
class GastonPattern:public Pattern{
public:
	int stage;
	Path* path;
	PatternTree* patterntree;
	PatternGraph* patterngraph;
	GraphState graphstate;
	ostream* ostr;
	//closeleg.cpp
	CloseLegOccurrences closelegoccurrences;

	GastonPattern(){
		path = 0;
		patterntree = 0;
		patterngraph = 0;
	}

	~GastonPattern(){
		if (path!=0)
			delete path;
		if (patterntree!=0)
			delete patterntree;
		if (patterngraph !=0)
			delete patterngraph;
	}

	CloseLegOccurrencesPtr join ( LegOccurrences &legoccsdata, CloseLegOccurrences &closelegoccsdata ) {
	  Frequency frequency = 0;
	  Tid lasttid = NOTID;
	  vector<CloseLegOccurrence> &closelegoccs = closelegoccsdata.elements;
	  vector<LegOccurrence> &legoccs = legoccsdata.elements;

	  closelegoccurrences.elements.resize ( 0 );

	  unsigned int legoccssize = legoccs.size (), closelegoccssize = closelegoccs.size ();
	  OccurrenceId j = 0, k = 0;
	  int comp;

	  while ( true ) {
	    comp = legoccs[j].occurrenceid - closelegoccs[k].occurrenceid;
	    if  ( comp < 0 ) {
	      j++;
	      if ( j == legoccssize )
	        break;
	    }
	    else {
	      if ( comp == 0 ) {
	        closelegoccurrences.elements.emplace_back( legoccs[j].tid, j );
	        if ( legoccs[j].tid != lasttid ) {
	          lasttid = legoccs[j].tid;
	          frequency++;
	        }
	        j++;
	        if ( j == legoccssize )
	          break;
	      }
	      else {
	        k++;
	        if ( k == closelegoccssize )
	          break;
	      }
	    }
	  }

	  if ( frequency >= minfreq ) {
	    closelegoccurrences.frequency = frequency;
	    return &closelegoccurrences;
	  }
	  else
	    return NULL;
	}

	CloseLegOccurrencesPtr join ( CloseLegOccurrences &closelegoccsdata1, CloseLegOccurrences &closelegoccsdata2 ) {
	  Frequency frequency = 0;
	  Tid lasttid = NOTID;
	  vector<CloseLegOccurrence> &closelegoccs1 = closelegoccsdata1.elements,
	                             &closelegoccs2 = closelegoccsdata2.elements;

	  unsigned int closelegoccs1size = closelegoccs1.size (), closelegoccs2size = closelegoccs2.size ();
	  closelegoccurrences.elements.resize ( 0 );
	  OccurrenceId j = 0, k = 0;
	  int comp;

	  while ( true ) {
	    comp = closelegoccs1[j].occurrenceid - closelegoccs2[k].occurrenceid;
	    if ( comp < 0 ) {
	      j++;
	      if ( j == closelegoccs1size )
	        break;
	    }
	    else {
	      if ( comp == 0 ) {
	        closelegoccurrences.elements.emplace_back ( closelegoccs1[j].tid, closelegoccs1[j].occurrenceid );
	        if ( closelegoccs1[j].tid != lasttid ) {
	          lasttid = closelegoccs1[j].tid;
	          frequency++;
	        }
	        j++;
	        if ( j == closelegoccs1size )
	          break;
	      }
	      k++;
	      if ( k == closelegoccs2size )
	        break;
	    }
	  }

	  if ( frequency >= minfreq ) {
	    closelegoccurrences.frequency = frequency;
	    return &closelegoccurrences;
	  }
	  else
	    return NULL;
	}
/*
	void swap(CloseLegOccurrences& a, CloseLegOccurrences& b){
		CloseLegOccurrences tmp(a);
		a = b;
		b = tmp;

	}

	void swap(LegOccurrences& a, LegOccurrences& b){
		LegOccurrences tmp(a);
		a = b;
		b = tmp;

	}
*/
	void addCloseExtensions ( vector<CloseLegPtr> &targetcloselegs, int number ) {
	  if ( closelegsoccsused ) {
	    for ( int i = 1; i < candidatecloselegsoccs.size (); i++ )
	      if ( candidatecloselegsoccsused[i] ) {
	        vector<CloseLegOccurrences> &edgelabeloccs = candidatecloselegsoccs[i];
	        for ( EdgeLabel j = 0; j < edgelabeloccs.size (); j++ ) {
	          if ( edgelabeloccs[j].frequency >= minfreq ) {
	            CloseLegPtr closelegptr = new CloseLeg;
	            closelegptr->tuple.label = j;
	            closelegptr->tuple.to = i;
	            closelegptr->tuple.from = number;
	            swap ( closelegptr->occurrences, edgelabeloccs[j] );
	            targetcloselegs.push_back ( closelegptr );
	          }
	        }
	      }
	  }
	}

	void addCloseExtensions ( vector<CloseLegPtr> &targetcloselegs, vector<CloseLegPtr> &sourcecloselegs, LegOccurrences &sourceoccs ) {
	  for ( int i = 0; i < sourcecloselegs.size (); i++ ) {
	    CloseLegOccurrencesPtr closelegoccurrencesptr = join ( sourceoccs, sourcecloselegs[i]->occurrences );
	    if ( closelegoccurrencesptr ) {
	      CloseLegPtr closelegptr = new CloseLeg;
	      closelegptr->tuple = sourcecloselegs[i]->tuple;
	      swap ( closelegptr->occurrences, *closelegoccurrencesptr );
	      targetcloselegs.push_back ( closelegptr );
	    }
	  }
	}



	//legoccurrence.cpp
	vector<LegOccurrences> candidatelegsoccurrences; // for each frequent possible edge, the occurrences found, used by extend
	LegOccurrences legoccurrences;
	bool closelegsoccsused;
	vector<vector< CloseLegOccurrences> > candidatecloselegsoccs;
	vector<bool> candidatecloselegsoccsused;


	void initLegStatics () {
	  candidatecloselegsoccs.reserve ( 200 ); // should be larger than the largest structure that contains a cycle
	  candidatelegsoccurrences.resize ( database.frequentEdgeLabelSize () );
	}


	friend ostream &operator<< ( ostream &stream, LegOccurrence &occ ) {
	  stream << "[" << occ.tid << "," << occ.occurrenceid << "," << occ.tonodeid << "," << occ.fromnodeid << "]";
	  return stream;
	}

	friend ostream &operator<< ( ostream &stream, vector<LegOccurrence> &occs ) {
	  Tid lasttid = NOTID;
	  Frequency frequency = 0;
	  for ( int i = 0; i < occs.size (); i++ ) {
	    //stream << occs[i];
	    if ( occs[i].tid != lasttid ) {
	      stream << occs[i].tid << " ";
	      lasttid = occs[i].tid;
	      frequency++;
	    }
	  }
	  stream << endl << " (" << frequency << ")" << endl;

	  return stream;
	}

	// This function is on the critical path. Its efficiency is MOST important.
	LegOccurrencesPtr join ( LegOccurrences &legoccsdata1, NodeId connectingnode, LegOccurrences &legoccsdata2 ) {
	  if ( graphstate.getNodeDegree ( connectingnode ) == graphstate.getNodeMaxDegree ( connectingnode ) )
	    return NULL;

	  Frequency frequency = 0;
	  Tid lasttid = NOTID;
	  vector<LegOccurrence> &legoccs1 = legoccsdata1.elements, &legoccs2 = legoccsdata2.elements;
	  legoccurrences.elements.resize ( 0 );
	  legoccurrences.maxdegree = 0;
	  legoccurrences.selfjoin = 0;
	  //legoccurrences.elements.reserve ( legoccs1.size () * 2 ); // increased memory usage, and speed!
	  OccurrenceId j = 0, k = 0, l, m;
	  unsigned int legoccs1size = legoccs1.size (), legoccs2size = legoccs2.size (); // this increases speed CONSIDERABLY!
	  Tid lastself = NOTID;

	  do {
	    while ( j < legoccs1size && legoccs1[j].occurrenceid < legoccs2[k].occurrenceid ) {
	      j++;
	    }
	    if ( j < legoccs1size ) {
	      LegOccurrence &jlegocc = legoccs1[j];
	      while ( k < legoccs2size && legoccs2[k].occurrenceid < jlegocc.occurrenceid ) {
	        k++;
	      }
	      if ( k < legoccs2size ) {
	        if ( legoccs2[k].occurrenceid == jlegocc.occurrenceid ) {
	          m = j;
	          do {
	            j++;
	          }
	          while ( j < legoccs1size && legoccs1[j].occurrenceid == jlegocc.occurrenceid );
	          l = k;
	          do {
	            k++;
	          }
	          while ( k < legoccs2size && legoccs2[k].occurrenceid == jlegocc.occurrenceid );
		  bool add = false;
	          for ( OccurrenceId m2 = m; m2 < j; m2++ ) {
	            int d = 0;
	            for ( OccurrenceId l2 = l; l2 < k; l2++ ) {
		      NodeId tonodeid = legoccs2[l2].tonodeid;
	              if ( legoccs1[m2].tonodeid !=  tonodeid ) {
	                legoccurrences.elements.emplace_back ( jlegocc.tid, m2, tonodeid, legoccs2[l2].fromnodeid,  legoccs1[m2]);
	                setmax ( legoccurrences.maxdegree, database.trees[jlegocc.tid]->nodes[tonodeid].edges.size () );
			add = true;
			d++;
	              }
	            }
		    if ( d > 1 && jlegocc.tid != lastself ) {
		      lastself = jlegocc.tid;
		      legoccurrences.selfjoin++;
		    }
		  }

		  if ( jlegocc.tid != lasttid && add ) {
	            lasttid = jlegocc.tid;
		    frequency++;
		  }

	          if ( k == legoccs2size )
	            break;
	        }
	      }
	      else
	        break;
	    }
	    else
	      break;
	  }
	  while ( true );

	  if ( frequency >= minfreq ) {
	    //legoccurrences.parentPushBack(legoccsdata1);
	    legoccurrences.number = legoccsdata1.number + 1;
	    legoccurrences.frequency = frequency;
	    return &legoccurrences;
	  }
	  else
	    return NULL;
	}

	LegOccurrencesPtr join ( LegOccurrences &legoccsdata ) {
	  if ( legoccsdata.selfjoin < minfreq )
	    return NULL;
	  legoccurrences.elements.resize ( 0 );
	  vector<LegOccurrence> &legoccs = legoccsdata.elements;
	  legoccurrences.maxdegree = 0;
	  legoccurrences.selfjoin = 0;
	  Tid lastself = NOTID;

	  OccurrenceId j = 0, k, l, m;
	  do {
	    k = j;
	    LegOccurrence &legocc = legoccs[k];
	    do {
	      j++;
	    }
	    while ( j < legoccs.size () &&
	            legoccs[j].occurrenceid == legocc.occurrenceid );
	    for ( l = k; l < j; l++ )
	      for ( m = k; m < j; m++ )
	        if ( l != m ) {
	          legoccurrences.elements.emplace_back ( legocc.tid, l, legoccs[m].tonodeid, legoccs[m].fromnodeid, legoccs[l] );
	          setmax ( legoccurrences.maxdegree, database.trees[legocc.tid]->nodes[legoccs[m].tonodeid].edges.size () );
	        }
	    if ( ( j - k > 2 ) && legocc.tid != lastself ) {
	      lastself = legocc.tid;
	      legoccurrences.selfjoin++;
	    }
	  }
	  while ( j < legoccs.size () );

	    // no need to check that we are frequent, we must be frequent
	  //legoccurrences.parentPushBack(legoccsdata);
	  legoccurrences.number = legoccsdata.number + 1;
	  legoccurrences.frequency = legoccsdata.selfjoin;
	    // we compute the self-join frequency exactly while building the
	    // previous list. It is therefore not necessary to recompute it.
	  return &legoccurrences;
	}

	inline int nocycle ( DatabaseTreePtr tree, DatabaseTreeNode &node, NodeId tonode, OccurrenceId occurrenceid, LegOccurrences* legoccurrencesdata ) {
	//tt2.start();
	  if ( !tree->nodes[tonode].incycle )
	    return 0;
	  if ( !node.incycle )
	    return 0;
	  int i;
	  vector<int>& parent_tonodeid = legoccurrencesdata->elements[occurrenceid].parent_tonodeid;
	  //int num = legoccurrencesdata->number;
	  if ( legoccurrencesdata->elements[occurrenceid].tonodeid == tonode ){
		  return parent_tonodeid.size()+1;
	  }
	  //cout << num << " " << parent_tonodeid.size() << endl;
	  //occurrenceid = legoccurrencesdata->elements[occurrenceid].occurrenceid;
	  //list<LegOccurrences>::iterator i = parentOccurrencesdata.end();
	  //for(i--; i!=parentOccurrencesdata.begin(); i--){
	  for(i = parent_tonodeid.size()-1; i>=0 ; i--){
		  if ( parent_tonodeid[i] == tonode ){
			  return i+1;
		  }
		  //occurrenceid = parentOccurrencesdata[i].elements[occurrenceid].occurrenceid;

		  /*
		  if((*i).elements[occurrenceid].tonodeid == tonode)
			  return (*i).number;
		  occurrenceid = (*i).elements[occurrenceid].occurrenceid;
		  */
	  }
	//tt2.stop();
	  return 0;
	}

	void candidateCloseLegsAllocate ( int number, int maxnumber ) {
	  if ( !closelegsoccsused ) {
	    int oldsize = candidatecloselegsoccs.size ();
	    candidatecloselegsoccs.resize ( maxnumber );
	    for ( int k = oldsize; k < candidatecloselegsoccs.size (); k++ ) {
	      candidatecloselegsoccs[k].resize ( database.frequentEdgeLabelSize () );
	    }
	    candidatecloselegsoccsused.resize ( 0 );
	    candidatecloselegsoccsused.resize ( maxnumber, false );
	    closelegsoccsused = true;
	  }
	  if ( !candidatecloselegsoccsused[number] ) {
	    candidatecloselegsoccsused[number] = true;
	    vector<CloseLegOccurrences> &candidateedgelabeloccs = candidatecloselegsoccs[number];
	    for ( int k = 0; k < candidateedgelabeloccs.size (); k++ ) {
	      candidateedgelabeloccs[k].elements.resize ( 0 );
	      candidateedgelabeloccs[k].frequency = 0;
	    }
	  }
	}

	void extend ( LegOccurrences &legoccurrencesdata ) {
	  // we're trying hard to avoid repeated destructor/constructor calls for complex types like vectors.
	  // better reuse previously allocated memory, if possible!
	  vector<LegOccurrence> &legoccurrences = legoccurrencesdata.elements;
	  Tid lastself[candidatelegsoccurrences.size ()];
	  for ( int i = 0; i < candidatelegsoccurrences.size (); i++ ) {
	    candidatelegsoccurrences[i].elements.resize ( 0 );
	    //candidatelegsoccurrences[i].elements.reserve ( legoccurrences.size () ); // increases memory usage, but also speed!

	    //candidatelegsoccurrences[i].parentPushBack(legoccurrencesdata);
	    candidatelegsoccurrences[i].number = legoccurrencesdata.number + 1;
	    candidatelegsoccurrences[i].maxdegree = 0;
	    candidatelegsoccurrences[i].frequency = 0;
	    candidatelegsoccurrences[i].selfjoin = 0;
	    lastself[i] = NOTID;
	  }

	  closelegsoccsused = false; // we are lazy with the initialization of close leg arrays, as we may not need them at all in
	                             // many cases

	  for ( OccurrenceId i = 0; i < legoccurrences.size (); i++ ) {
	    LegOccurrence &legocc = legoccurrences[i];
	    DatabaseTreePtr tree = database.trees[legocc.tid];
	    DatabaseTreeNode &node = tree->nodes[legocc.tonodeid];
	    for ( int j = 0; j < node.edges.size (); j++ ) {
	      if ( node.edges[j].tonode != legocc.fromnodeid ) {
	  	EdgeLabel edgelabel = node.edges[j].edgelabel;
		int number = nocycle ( tree, node, node.edges[j].tonode, i, &legoccurrencesdata );
		//int number = 0;
	     if ( number == 0 ) {
	          vector<LegOccurrence> &candidatelegsoccs = candidatelegsoccurrences[edgelabel].elements;
	          if ( candidatelegsoccs.empty () )
	  	    candidatelegsoccurrences[edgelabel].frequency++;
		  else {
		    if ( candidatelegsoccs.back ().tid != legocc.tid )
	    	      candidatelegsoccurrences[edgelabel].frequency++;
		    if ( candidatelegsoccs.back ().occurrenceid == i &&
		         lastself[edgelabel] != legocc.tid ) {
	              lastself[edgelabel] = legocc.tid;
		      candidatelegsoccurrences[edgelabel].selfjoin++;
		    }
	          }
	          candidatelegsoccs.emplace_back ( legocc.tid, i, node.edges[j].tonode, legocc.tonodeid, legocc);
	          setmax ( candidatelegsoccurrences[edgelabel].maxdegree, database.trees[legocc.tid]->nodes[node.edges[j].tonode].edges.size () );
	        }
	        else if ( number - 1 != graphstate.nodes.back().edges[0].tonode ) {
	          candidateCloseLegsAllocate ( number, legoccurrencesdata.number+1 );

	          vector<CloseLegOccurrence> &candidatelegsoccs = candidatecloselegsoccs[number][edgelabel].elements;
	          if ( !candidatelegsoccs.size () || candidatelegsoccs.back ().tid != legocc.tid )
		    candidatecloselegsoccs[number][edgelabel].frequency++;
	          candidatelegsoccs.emplace_back ( legocc.tid, i );
	            setmax ( candidatelegsoccurrences[edgelabel].maxdegree, database.trees[legocc.tid]->nodes[node.edges[j].tonode].edges.size () );
	        }
	      }
	    }
	  }
	}

	void extend ( LegOccurrences &legoccurrencesdata, EdgeLabel minlabel, EdgeLabel neglect ) {
	  // we're trying hard to avoid repeated destructor/constructor calls for complex types like vectors.
	  // better reuse previously allocated memory, if possible!
	  vector<LegOccurrence> &legoccurrences = legoccurrencesdata.elements;
	  int lastself[candidatelegsoccurrences.size ()];

	  for ( int i = 0; i < candidatelegsoccurrences.size (); i++ ) {
	    candidatelegsoccurrences[i].elements.resize ( 0 );
	    //candidatelegsoccurrences[i].parentPushBack(legoccurrencesdata);
	    candidatelegsoccurrences[i].number = legoccurrencesdata.number + 1;
	    candidatelegsoccurrences[i].maxdegree = 0;
	    candidatelegsoccurrences[i].selfjoin = 0;
	    lastself[i] = NOTID;
	    candidatelegsoccurrences[i].frequency = 0;
	  }

	  closelegsoccsused = false; // we are lazy with the initialization of close leg arrays, as we may not need them at all in
	                             // many cases
	  for ( OccurrenceId i = 0; i < legoccurrences.size (); i++ ) {
	    LegOccurrence &legocc = legoccurrences[i];
	    DatabaseTreePtr tree = database.trees[legocc.tid];
	    DatabaseTreeNode &node = tree->nodes[legocc.tonodeid];
	    for ( int j = 0; j < node.edges.size (); j++ ) {
	      if ( node.edges[j].tonode != legocc.fromnodeid ) {
		EdgeLabel edgelabel = node.edges[j].edgelabel;
	        int number = nocycle ( tree, node, node.edges[j].tonode, i, &legoccurrencesdata );
	        if ( number == 0 ) {
		  if ( edgelabel >= minlabel && edgelabel != neglect ) {
	            vector<LegOccurrence> &candidatelegsoccs = candidatelegsoccurrences[edgelabel].elements;
	            if ( candidatelegsoccs.empty () )
	  	      candidatelegsoccurrences[edgelabel].frequency++;
		    else {
		      if ( candidatelegsoccs.back ().tid != legocc.tid )
	  	        candidatelegsoccurrences[edgelabel].frequency++;
		      if ( candidatelegsoccs.back ().occurrenceid == i &&
	                lastself[edgelabel] != legocc.tid ) {
	                lastself[edgelabel] = legocc.tid;
	                candidatelegsoccurrences[edgelabel].selfjoin++;
	              }
	            }
	            candidatelegsoccs.emplace_back ( legocc.tid, i, node.edges[j].tonode, legocc.tonodeid, legocc );
		    setmax ( candidatelegsoccurrences[edgelabel].maxdegree, database.trees[legocc.tid]->nodes[node.edges[j].tonode].edges.size () );
		  }
	        }
	        else if ( number - 1 != graphstate.nodes.back().edges[0].tonode ) {
	          candidateCloseLegsAllocate ( number, legoccurrencesdata.number+1 );

	          vector<CloseLegOccurrence> &candidatelegsoccs = candidatecloselegsoccs[number][edgelabel].elements;
	          if ( !candidatelegsoccs.size () || candidatelegsoccs.back ().tid != legocc.tid )
		    candidatecloselegsoccs[number][edgelabel].frequency++;
	          candidatelegsoccs.emplace_back ( legocc.tid, i );
	          setmax ( candidatelegsoccurrences[edgelabel].maxdegree, database.trees[legocc.tid]->nodes[node.edges[j].tonode].edges.size () );
	        }
	      }
	    }
	  }
	}


	//path.cpp

	//create path pattern from start node
	void createPath(NodeLabel startnodelabel){
		  path = new Path;
		  graphstate.insertStartNode ( startnodelabel );

		  path->nodelabels.push_back ( startnodelabel );
		  path->frontsymmetry = path->backsymmetry = path->totalsymmetry = 0;

		  cout << "Root " << database.nodelabels[startnodelabel].inputlabel << endl;

		  DatabaseNodeLabel &databasenodelabel = database.nodelabels[startnodelabel];

		  vector<EdgeLabel> edgelabelorder ( database.edgelabelsindexes.size () );
		  EdgeLabel j = 0;

		  vector<EdgeLabel> frequentedgelabels;
		  for ( int i = 0; i < databasenodelabel.frequentedgelabels.size (); i++ )
		    frequentedgelabels.push_back ( database.edgelabels[databasenodelabel.frequentedgelabels[i]].edgelabel );
		/*  for ( int i = 0; i < databasenodelabel.edgelabelfrequencies.size (); i++ )
		    if ( databasenodelabel.edgelabelfrequencies[i] >= minfreq )
		      frequentedgelabels.push_back ( database.edgelabels[i].edgelabel );*/
		  sort ( frequentedgelabels.begin (), frequentedgelabels.end () );
		  Tid lastself[frequentedgelabels.size ()];

		  for ( int i = 0; i < frequentedgelabels.size (); i++ ) {
		    edgelabelorder[frequentedgelabels[i]] = j;
		    j++;

		    PathLegPtr leg = new PathLeg;
		    path->legs.push_back ( leg );
		    leg->tuple.depth = 0;
		    leg->tuple.edgelabel = frequentedgelabels[i];
		    leg->tuple.connectingnode = 0;
		    //leg->occurrences.parentPushBack(databasenodelabel.occurrences);
		    leg->occurrences.number = 2;
		    leg->occurrences.maxdegree = 0;
		    leg->occurrences.selfjoin = 0;
		    lastself[i] = NOTID;
		    DatabaseEdgeLabel &databaseedgelabel = database.edgelabels[database.edgelabelsindexes[frequentedgelabels[i]]];
		    leg->occurrences.frequency = databaseedgelabel.frequency;
		    if ( databaseedgelabel.fromnodelabel == startnodelabel )
		      leg->tuple.nodelabel = databaseedgelabel.tonodelabel;
		    else
		      leg->tuple.nodelabel = databaseedgelabel.fromnodelabel;
		  }

		  for ( unsigned int i = 0; i < databasenodelabel.occurrences.elements.size (); i++ ) {
		    DatabaseTree &tree = * (database.trees[databasenodelabel.occurrences.elements[i].tid]);
		    DatabaseTreeNode &datanode = tree.nodes[databasenodelabel.occurrences.elements[i].tonodeid];
		    for ( int j = 0; j < datanode.edges.size (); j++ ) {
		      EdgeLabel edgelabel = edgelabelorder[datanode.edges[j].edgelabel];
		      PathLeg &leg = * (path->legs[edgelabel] );
		      if ( !leg.occurrences.elements.empty () &&
		           leg.occurrences.elements.back ().occurrenceid == i &&
		           lastself[edgelabel] != tree.tid ) {
		        leg.occurrences.selfjoin++;
		        lastself[edgelabel] = tree.tid;
		      }
		      vector_push_back ( LegOccurrence, leg.occurrences.elements, legoccurrence );
		      //added by wenwen
		      legoccurrence.parent_tonodeid.push_back(databasenodelabel.occurrences.elements[i].tonodeid);
		      legoccurrence.tid = tree.tid;
		      legoccurrence.occurrenceid = i;
		      legoccurrence.tonodeid = datanode.edges[j].tonode;
		      legoccurrence.fromnodeid = databasenodelabel.occurrences.elements[i].tonodeid;
		    }
		  }

	}

	//create path pattern from and existed path and a new node
	void createPath ( Path &parentpath, unsigned int legindex ) {
	  path = new Path;
	  PathLeg &leg = (*parentpath.legs[legindex]);
	  int positionshift;

	  OUTPUT(parentpath.legs[legindex]->occurrences.frequency,  ostr);

	  // fill in normalisation information, it seems a lot of code, but in fact it's just a lot
	  // of code to efficiently perform one walk through the edge/nodelabels arrays.

	  path->nodelabels.resize ( parentpath.nodelabels.size () + 1 );
	  path->edgelabels.resize ( parentpath.edgelabels.size () + 1 );
	  addCloseExtensions (path->closelegs, parentpath.closelegs, leg.occurrences );
	  if ( parentpath.nodelabels.size () == 1 ) {
		path->totalsymmetry = parentpath.nodelabels[0] - leg.tuple.nodelabel;
		path->frontsymmetry = path->backsymmetry = 0;
		path->nodelabels[1] = leg.tuple.nodelabel;
		path->edgelabels[0] = leg.tuple.edgelabel;
		path->nodelabels[0] = parentpath.nodelabels[0];
	    positionshift = 0;
	  }
	  else if ( leg.tuple.depth == 0 ) {
	    positionshift = 1;
	    path->nodelabels[0] = leg.tuple.nodelabel;
	    path->edgelabels[0] = leg.tuple.edgelabel;

	    path->backsymmetry = parentpath.totalsymmetry;
	    path->frontsymmetry = leg.tuple.nodelabel - parentpath.nodelabels[parentpath.nodelabels.size () - 2];
	    path->totalsymmetry = leg.tuple.nodelabel - parentpath.nodelabels.back ();
	    if ( !path->totalsymmetry )
	    	path->totalsymmetry = leg.tuple.edgelabel - parentpath.edgelabels.back ();

	    int i = 0;
	    // we can prepend only before strings of length 2
	    if ( parentpath.nodelabels.size () > 2 ) {
	      if ( !path->frontsymmetry )
	    	  path->frontsymmetry = leg.tuple.edgelabel - parentpath.edgelabels[parentpath.nodelabels.size () - 3];

	      while ( !path->frontsymmetry && i < parentpath.edgelabels.size () / 2 ) {
	    	path->nodelabels[i + 1] = parentpath.nodelabels[i];
	    	path->edgelabels[i + 1] = parentpath.edgelabels[i];

	    	path->frontsymmetry = parentpath.nodelabels[i] - parentpath.nodelabels[parentpath.nodelabels.size () - i - 3];
	        if ( !path->frontsymmetry && parentpath.nodelabels.size () > 3 )
	        	path->frontsymmetry = parentpath.edgelabels[i] - parentpath.edgelabels[parentpath.nodelabels.size () - i - 4];

		if ( !path->totalsymmetry ) {
			path->totalsymmetry = parentpath.nodelabels[i] - parentpath.nodelabels[parentpath.nodelabels.size () - i - 2];
		  if ( !path->totalsymmetry )
			  path->totalsymmetry = parentpath.edgelabels[i] - parentpath.edgelabels[parentpath.nodelabels.size () - i - 3];
		}

	        i++;
	      }
	    }
	    for ( ; !path->totalsymmetry && i < parentpath.edgelabels.size () / 2; i++ ) {
	      path->nodelabels[i + 1] = parentpath.nodelabels[i];
	      path->edgelabels[i + 1] = parentpath.edgelabels[i];

	      path->totalsymmetry = parentpath.nodelabels[i] - parentpath.nodelabels[parentpath.nodelabels.size () - i - 2];
	      if ( !path->totalsymmetry && parentpath.nodelabels.size () > 3 )
	    	  path->totalsymmetry = parentpath.edgelabels[i] - parentpath.edgelabels[parentpath.nodelabels.size () - i - 3];
	    }
	    for ( ;i < parentpath.edgelabels.size (); i++ ) {
	      path->nodelabels[i + 1] = parentpath.nodelabels[i];
	      path->edgelabels[i + 1] = parentpath.edgelabels[i];
	    }

	    path->nodelabels[i + 1] = parentpath.nodelabels[i];

	    // build OccurrenceLists
	    extend ( leg.occurrences );
	    for ( int i = 0; i < candidatelegsoccurrences.size (); i++ ) {
	      if ( candidatelegsoccurrences[i].frequency >= minfreq ) {
	        PathLegPtr leg2 = new PathLeg;
	        path->legs.push_back ( leg2 );
	        leg2->tuple.edgelabel = i;
	        leg2->tuple.connectingnode = graphstate.lastNode ();
	        DatabaseEdgeLabel &databaseedgelabel = database.edgelabels[database.edgelabelsindexes[i]];
	        if ( databaseedgelabel.fromnodelabel == leg.tuple.nodelabel )
	          leg2->tuple.nodelabel = databaseedgelabel.tonodelabel;
	        else
	          leg2->tuple.nodelabel = databaseedgelabel.fromnodelabel;
	        leg2->tuple.depth = 0;
	        store ( leg2->occurrences, candidatelegsoccurrences[i] ); // avoid copying
	      }
	    }
	  }
	  else  {
	    positionshift = 0;

	    path->frontsymmetry = parentpath.totalsymmetry;
	    path->backsymmetry = parentpath.nodelabels[1] - leg.tuple.nodelabel;
	    path->totalsymmetry = parentpath.nodelabels[0] - leg.tuple.nodelabel;
	    if ( !path->totalsymmetry )
	    	path->totalsymmetry = parentpath.edgelabels[0] - leg.tuple.edgelabel;
	    int i = 0;
	    if ( parentpath.nodelabels.size () > 2 ) {
	      if ( !path->backsymmetry )
	    	path->backsymmetry = parentpath.edgelabels[1] - leg.tuple.edgelabel;

	        while ( !path->backsymmetry && i < parentpath.edgelabels.size () / 2 ) {
	    	path->nodelabels[i] = parentpath.nodelabels[i];
	    	path->edgelabels[i] = parentpath.edgelabels[i];

	    	path->backsymmetry = parentpath.nodelabels[i + 2] - parentpath.nodelabels[parentpath.nodelabels.size () - i - 1];
	    	if ( !path->backsymmetry && parentpath.nodelabels.size () > 3 )
	    		path->backsymmetry = parentpath.edgelabels[i + 2] - parentpath.edgelabels[parentpath.nodelabels.size () - i - 2];

	    	if ( !path->totalsymmetry ) {
	    		path->totalsymmetry = parentpath.nodelabels[i + 1] - parentpath.nodelabels[parentpath.nodelabels.size () - i - 1];
	    		if ( !path->totalsymmetry && parentpath.nodelabels.size () > 3 )
	    			path->totalsymmetry = parentpath.edgelabels[i + 1] - parentpath.edgelabels[parentpath.nodelabels.size () - i - 2];
	    	}
	    		i++;
	      }
	    }
	    for ( ; !path->totalsymmetry && i < parentpath.edgelabels.size () / 2; i++ ) {
	      path->nodelabels[i] = parentpath.nodelabels[i];
	      path->edgelabels[i] = parentpath.edgelabels[i];
	      path->totalsymmetry = parentpath.nodelabels[i + 1] - parentpath.nodelabels[parentpath.nodelabels.size () - i - 1];
	      if ( !path->totalsymmetry && i < parentpath.edgelabels.size () - 1 )
	    	  path->totalsymmetry = parentpath.edgelabels[i + 1] - parentpath.edgelabels[parentpath.nodelabels.size () - i - 2];
	    }
	    for ( ; i < parentpath.edgelabels.size (); i++ ) {
	      path->nodelabels[i] = parentpath.nodelabels[i];
	      path->edgelabels[i] = parentpath.edgelabels[i];
	    }

	    path->nodelabels[i] = parentpath.nodelabels[i];
	    path->edgelabels[i] = leg.tuple.edgelabel;
	    path->nodelabels[i+1] = leg.tuple.nodelabel;
	  }

	  int i = 0;
	  LegOccurrencesPtr legoccurrencesptr;

	  for ( ; i < legindex; i++ ) {
	    PathLeg &leg2 = (*parentpath.legs[i]);

	    if ( legoccurrencesptr = join ( leg.occurrences, leg2.tuple.connectingnode, leg2.occurrences ) ) {
	      PathLegPtr leg3 = new PathLeg;
	      path->legs.push_back ( leg3 );
	      leg3->tuple.connectingnode = leg2.tuple.connectingnode;
	      leg3->tuple.edgelabel = leg2.tuple.edgelabel;
	      leg3->tuple.nodelabel = leg2.tuple.nodelabel;
	      leg3->tuple.depth = leg2.tuple.depth + positionshift;
	      store ( leg3->occurrences, *legoccurrencesptr );
	    }
	  }
	  if ( legoccurrencesptr = join ( leg.occurrences ) ) {
	    PathLegPtr leg3 = new PathLeg;
	    path->legs.push_back ( leg3 );
	    leg3->tuple.connectingnode = leg.tuple.connectingnode;
	    leg3->tuple.edgelabel = leg.tuple.edgelabel;
	    leg3->tuple.nodelabel = leg.tuple.nodelabel;
	    leg3->tuple.depth = leg.tuple.depth + positionshift;
	    store ( leg3->occurrences, *legoccurrencesptr );
	  }

	  for ( i++; i < parentpath.legs.size (); i++ ) {
	    PathLeg &leg2 = (*parentpath.legs[i]);
	    if ( legoccurrencesptr = join ( leg.occurrences, leg2.tuple.connectingnode, leg2.occurrences ) ) {
	      PathLegPtr leg3 = new PathLeg;
	      path->legs.push_back ( leg3 );
	      leg3->tuple.connectingnode = leg2.tuple.connectingnode;
	      leg3->tuple.edgelabel = leg2.tuple.edgelabel;
	      leg3->tuple.nodelabel = leg2.tuple.nodelabel;
	      leg3->tuple.depth = leg2.tuple.depth + positionshift;
	      store ( leg3->occurrences, *legoccurrencesptr );
	    }
	  }

	  if ( positionshift ) {
	    addCloseExtensions ( path->closelegs, leg.occurrences.number ); // stored separately
	    return;
	  }
	  extend ( leg.occurrences );

	  for ( int i = 0; i < candidatelegsoccurrences.size (); i++ ) {
	    if ( candidatelegsoccurrences[i].frequency >= minfreq ) {
	      PathLegPtr leg2 = new PathLeg;
	      path->legs.push_back ( leg2 );
	      leg2->tuple.edgelabel = i;
	      leg2->tuple.connectingnode = graphstate.lastNode ();
	      DatabaseEdgeLabel &databaseedgelabel = database.edgelabels[database.edgelabelsindexes[i]];
	      if ( databaseedgelabel.fromnodelabel == leg.tuple.nodelabel )
	        leg2->tuple.nodelabel = databaseedgelabel.tonodelabel;
	      else
	        leg2->tuple.nodelabel = databaseedgelabel.fromnodelabel;
	      leg2->tuple.depth = leg.tuple.depth + 1;
	      store ( leg2->occurrences, candidatelegsoccurrences[i] ); // avoid copying
	    }
	  }
	  addCloseExtensions ( path->closelegs, leg.occurrences.number );

	}


	//for tree

    // this function assumes that the extension tuple is already added at the back of the queue,
    // and the equivalency information has been filled in.
    void addExtensionLegs ( Tuple &tuple, LegOccurrences &legoccurrences ) {
      if ( legoccurrences.maxdegree == 1 )
        return;
      if ( tuple.depth == patterntree->maxdepth ) {
        extend ( legoccurrences, MAXEDGELABEL, NONODE );
        addCloseExtensions ( patterntree->closelegs, legoccurrences.number );
        return;
      }
      EdgeLabel minlabel = NOEDGELABEL, neglect, pathlowestlabel = patterntree->treetuples[tuple.depth + 1 + patterntree->rootpathstart].label;

      if ( patterntree->nextprefixindex != NONEXTPREFIX ) {
        if ( patterntree->treetuples[patterntree->nextprefixindex].depth <= tuple.depth ) {
          // heuristic saving
          extend ( legoccurrences, MAXEDGELABEL, NONODE );
          addCloseExtensions ( patterntree->closelegs, legoccurrences.number );
          return;
        }
        minlabel = patterntree->treetuples[patterntree->nextprefixindex].label;
        if ( minlabel == pathlowestlabel )
          minlabel = NOEDGELABEL;
        else
          neglect = pathlowestlabel;
      }
      if ( tuple.depth == patterntree->maxdepth - 1 ) {
        if ( patterntree->rootpathrelations.back () > 0 ) {
          // heuristic saving
          extend ( legoccurrences, MAXEDGELABEL, NONODE );
          addCloseExtensions ( patterntree->closelegs, legoccurrences.number );
          return;
        }
        if ( patterntree->rootpathrelations.back () == 0 )
          if ( minlabel != NOEDGELABEL ) {
            neglect = NOEDGELABEL;
    	if ( pathlowestlabel > minlabel )
              minlabel = pathlowestlabel;
          }
          else {
            neglect = NOEDGELABEL;
            minlabel = pathlowestlabel;
          }
          // else we have a restriction, and that restriction is at least as high
          // as the label on the path.
      }

      if ( minlabel != NOEDGELABEL )
        extend ( legoccurrences, minlabel, neglect );
      else
        extend ( legoccurrences );

      if ( candidatelegsoccurrences[pathlowestlabel].frequency >= minfreq )
        // this is the first possible extension, as we force this label to be the lowest!
        addLeg ( graphstate.lastNode (), tuple.depth + 1, pathlowestlabel, candidatelegsoccurrences[pathlowestlabel] );

      for ( int i = 0; i < candidatelegsoccurrences.size (); i++ ) {
        if ( candidatelegsoccurrences[i].frequency >= minfreq && i != pathlowestlabel )
          addLeg ( graphstate.lastNode (), tuple.depth + 1, i, candidatelegsoccurrences[i] );
      }

      addCloseExtensions ( patterntree->closelegs, legoccurrences.number );
    }
    inline void addLeg ( NodeId connectingnode, const int depth, const EdgeLabel edgelabel, LegOccurrences &legoccurrences ) {
      LegPtr leg = new Leg;
      leg->tuple.depth = depth;
      leg->tuple.label = edgelabel;
      leg->tuple.connectingnode = connectingnode;
      store ( leg->occurrences, legoccurrences );
      patterntree->legs.push_back ( leg );
    }

    void addLeftLegs ( Path &path, PathLeg &leg, int &i, Depth olddepth, EdgeLabel lowestlabel, int leftend, int edgesize2 ) {
      // the order of the other extensions is almost correct - except that we must move
      // extensions at depth d which are for the label of the path at depth d to the front of
      // all legs of depth d...

      for ( ; i < path.legs.size () && path.legs[i]->tuple.depth <= leftend; i++ ) {
        if ( path.legs[i]->tuple.depth != olddepth ) {
          // when we encounter a new depth, we look whether there is an extension tuple at that
          // depth with the same label as the node on the path; this tuple has to be moved to the front; ...
          olddepth = path.legs[i]->tuple.depth;
          lowestlabel = path.edgelabels[olddepth - 1];
          int i2 = i;
          while ( i2 < path.legs.size () && path.legs[i2]->tuple.depth == olddepth ) {
            if ( path.legs[i2]->tuple.edgelabel == lowestlabel ) {
              LegOccurrencesPtr legoccurrencesptr = join ( leg.occurrences, path.legs[i2]->tuple.connectingnode, path.legs[i2]->occurrences );
              if ( legoccurrencesptr )
                addLeg ( path.legs[i2]->tuple.connectingnode, edgesize2 - olddepth, path.legs[i2]->tuple.edgelabel, *legoccurrencesptr );
              break;
            }
            i2++;
          }
        }
        // skip lowest label tuples, as they have already been moved to the front...
        if ( path.legs[i]->tuple.edgelabel != lowestlabel ) {
          LegOccurrencesPtr legoccurrencesptr = join ( leg.occurrences, path.legs[i]->tuple.connectingnode, path.legs[i]->occurrences );
          if ( legoccurrencesptr )
            addLeg ( path.legs[i]->tuple.connectingnode, edgesize2 - olddepth, path.legs[i]->tuple.edgelabel, *legoccurrencesptr );
        }
      }
    }

    int addLeftLegs ( Path &path, PathLeg &leg, Tuple &tuple, unsigned int legindex, int leftend, int edgesize2 ) {
      int i;

      LegOccurrencesPtr legoccurrencesptr = join ( leg.occurrences );
      if ( legoccurrencesptr )
        addLeg ( leg.tuple.connectingnode, tuple.depth, tuple.label, *legoccurrencesptr );

      // the easy part - the extensions of the left side of the original path

      if ( patterntree->rootpathrelations.back () == 0 && tuple.depth != patterntree->maxdepth ) {
        // we force the label on the path to be the lowest label, so all labels which are actually
        // lower must be added as they are higher here, deepest nodes are an exception;
        // no path may be lower than the current path

        for ( i = legindex - 1; i >= 0 && path.legs[i]->tuple.depth == leg.tuple.depth; i-- );
        EdgeLabel lowestlabel = path.edgelabels[leg.tuple.depth - 1];
        for ( i++; i < legindex; i++ ) {
          if ( path.legs[i]->tuple.edgelabel != lowestlabel ) {
            legoccurrencesptr = join ( leg.occurrences, path.legs[i]->tuple.connectingnode, path.legs[i]->occurrences );
            if ( legoccurrencesptr )
              addLeg ( path.legs[i]->tuple.connectingnode, tuple.depth, path.legs[i]->tuple.edgelabel, *legoccurrencesptr );
          }
        }
      }

      // the order of the other extensions is almost correct - except that we must move
      // extensions at depth d which are for the label of the path at depth d to the front of
      // all legs of depth d...

      i = legindex + 1;
      addLeftLegs ( path, leg, i, leg.tuple.depth, path.edgelabels[leg.tuple.depth - 1], leftend, edgesize2 );

      return i;
    }


    void addRightLegs ( Path &path, PathLeg &leg, int &i, Depth olddepth, EdgeLabel lowestlabel, int rightstart, int nodesize2 ) {
      int i2 = i + 1, k;
      LegOccurrencesPtr legoccurrencesptr;

      while ( i >= 0 && path.legs[i]->tuple.depth >= rightstart ) {
        // we encounter a new depth, do all the extensions of the previous depths...
        if ( path.legs[i]->tuple.depth != olddepth ) {
          for ( k = i + 1; k < i2; k++ ) {
            if ( path.legs[k]->tuple.edgelabel != lowestlabel ) {
              legoccurrencesptr = join ( leg.occurrences, path.legs[k]->tuple.connectingnode, path.legs[k]->occurrences );
              if ( legoccurrencesptr )
                addLeg ( path.legs[k]->tuple.connectingnode, path.legs[k]->tuple.depth - nodesize2, path.legs[k]->tuple.edgelabel, *legoccurrencesptr );
            }
          }
          i2 = i + 1;
          olddepth = path.legs[i]->tuple.depth;
          lowestlabel = path.edgelabels[olddepth];
        }
        // for the current depth, we encounter the label that is on the second path,
        // this extension must be moved to the front, so before the other tuples are
        // added by the code above
        if ( path.legs[i]->tuple.edgelabel == lowestlabel ) {
          legoccurrencesptr = join ( leg.occurrences, path.legs[i]->tuple.connectingnode, path.legs[i]->occurrences );
          if ( legoccurrencesptr )
            addLeg ( path.legs[i]->tuple.connectingnode, path.legs[i]->tuple.depth - nodesize2, path.legs[i]->tuple.edgelabel, *legoccurrencesptr );
        }
        i--;
      }
      // some tuples may not have been checked yet
      for ( k = i + 1; k < i2; k++ ) {
        if ( path.legs[k]->tuple.edgelabel != lowestlabel ) {
          legoccurrencesptr = join ( leg.occurrences, path.legs[k]->tuple.connectingnode, path.legs[k]->occurrences );
          if ( legoccurrencesptr )
            addLeg ( path.legs[k]->tuple.connectingnode, path.legs[k]->tuple.depth - nodesize2, path.legs[k]->tuple.edgelabel, *legoccurrencesptr );
        }
      }
    }

    int addRightLegs ( Path &path, PathLeg &leg, Tuple &tuple, unsigned int legindex, int rightstart, int nodesize2 ) {
      int i;
      LegOccurrencesPtr legoccurrencesptr = join ( leg.occurrences );
      if ( legoccurrencesptr )
        addLeg ( leg.tuple.connectingnode, tuple.depth, tuple.label, *legoccurrencesptr );

      // other extensions at the right path

      i = legindex - 1;
      while ( i >= 0 && path.legs[i]->tuple.depth == leg.tuple.depth )
        i--;

      // brothers of the new leg
      if ( patterntree->rootpathrelations.back () == 0 && tuple.depth != patterntree->maxdepth )
        for ( int j = i + 1; j < legindex; j++ ) {
          LegOccurrencesPtr legoccurrencesptr = join ( leg.occurrences, path.legs[j]->tuple.connectingnode, path.legs[j]->occurrences );
          if ( legoccurrencesptr )
            addLeg ( path.legs[j]->tuple.connectingnode, tuple.depth, path.legs[j]->tuple.edgelabel, *legoccurrencesptr );
        }
      EdgeLabel lowestlabel = path.edgelabels[leg.tuple.depth];
      for ( int j = legindex + 1; j < path.legs.size () && path.legs[j]->tuple.depth == leg.tuple.depth; j++ ) {
        if ( path.legs[j]->tuple.edgelabel != lowestlabel ) {
          LegOccurrencesPtr legoccurrencesptr = join ( leg.occurrences, path.legs[j]->tuple.connectingnode, path.legs[j]->occurrences );
          if ( legoccurrencesptr )
            addLeg ( path.legs[j]->tuple.connectingnode, tuple.depth, path.legs[j]->tuple.edgelabel, *legoccurrencesptr );
        }
      }

      // candidates at a lower depth
      if ( i >= 0 )
        addRightLegs ( path, leg, i, leg.tuple.depth, lowestlabel, rightstart, nodesize2 );

      return i;
    }


    void createTree ( Path &path, unsigned int legindex ) {
      patterntree = new PatternTree;
      PathLeg &leg = (*path.legs[legindex]);

      OUTPUT(path.legs[legindex]->occurrences.frequency, ostr);

      patterntree->maxdepth = path.edgelabels.size () / 2 - 1;
      int leftwalk, leftstart, rightwalk, rightstart;
      LegOccurrencesPtr legoccurrencesptr;

      addCloseExtensions ( patterntree->closelegs, path.closelegs, leg.occurrences );

      int nodesize2 = path.nodelabels.size () / 2;
      int edgesize2 = path.edgelabels.size () / 2;
      rightstart = rightwalk = nodesize2;
      leftstart = leftwalk = edgesize2 - 1;
      if ( path.totalsymmetry == 0 )
    	  patterntree->symmetric = path.nodelabels.size () % 2 + 1;
      else
    	  patterntree->symmetric = 0;
      if ( path.totalsymmetry ||
           leg.tuple.depth * 2 == path.edgelabels.size () ) {
        while ( leftwalk >= 0 &&
                path.edgelabels[leftwalk] == path.edgelabels[rightwalk] )
          leftwalk--, rightwalk++;
          // now there is one very nasty case: A-B-A-B-A-B
          // this path is not symmetric, but the labels on the edges are!
          // So we can find that leftwalk == -1.
          // In this case, we assume that the left part is the first path,
          // furthermore the position of the extension determines to which path
          // it is added
        graphstate.nasty = ( leftwalk == -1 );
        if ( leftwalk == -1 || path.edgelabels[leftwalk] < path.edgelabels[rightwalk] ) {
          // left part of the path should be the first path in the tree

          patterntree->treetuples.reserve ( path.edgelabels.size () + 1 );
          int i, j;
          for ( i = leftstart, j = 0; i >= 0; i--, j++ ) {
            vector_push_back ( Tuple, patterntree->treetuples, tuple );
            tuple.label = path.edgelabels[i];
            tuple.depth = j;
          }

          int leftend = nodesize2 - 1;

          if ( leg.tuple.depth <= leftend ) {
            // extension is an extension of the first path

            // add extension tuple
            vector_push_back ( Tuple, patterntree->treetuples, tuple );
            tuple.label = leg.tuple.edgelabel;
            tuple.depth = edgesize2 - leg.tuple.depth;

            patterntree->rootpathrelations.reserve ( tuple.depth );
            patterntree->rightmostindexes.reserve ( tuple.depth );
            for ( i = 0; i < tuple.depth; i++ ) {
            	patterntree->rootpathrelations.push_back ( 0 );
            	patterntree->rightmostindexes.push_back ( i );
            }
            patterntree->rightmostindexes.push_back ( patterntree->treetuples.size () - 1 );
            if ( tuple.label == patterntree->treetuples[tuple.depth].label ) {
            	patterntree->nextprefixindex = tuple.depth + 1;
            	patterntree->rootpathrelations.push_back ( 0 );
            }
            else {
            	patterntree->nextprefixindex = NONEXTPREFIX;
            	patterntree->rootpathrelations.push_back ( patterntree->treetuples[tuple.depth].label - tuple.label );
            }

            patterntree->rootpathstart = 0;
            patterntree->nextpathstart = patterntree->treetuples.size ();

            // fill in possible extensions below the new node
            addExtensionLegs ( tuple, leg.occurrences );

            // add second path
            for ( i = rightstart, j = 0; i < path.edgelabels.size (); i++, j++ ) {
              vector_push_back ( Tuple, patterntree->treetuples, tuple );
              tuple.label = path.edgelabels[i];
              tuple.depth = j;
            }

            // here, we have determined the expansion tuple sequence of the tree, and the normal
            // form information. Now we have to fill in the legs at other positions than below
            // the new node. This is complicated as the occurrence list order of the path
            // has to be changed...

            i = addLeftLegs ( path, leg, tuple, legindex, leftend, edgesize2 );

            patterntree->secondpathleg = patterntree->legs.size ();


            // a difficult part - the extensions of the right side of the original path; here the labels
            // are in correct order, but the depth order is incorrect (more precisely, reversed)

            Depth olddepth = NODEPTH;
            int j2 = path.legs.size ();
            int j = j2 - 1, k;

            if ( j >= i && path.legs[j]->tuple.depth == path.edgelabels.size () ) {
              // do not include legs after the longest path!
              do
                j--;
              while ( j >= i && path.legs[j]->tuple.depth == path.edgelabels.size () );
              j2 = j + 1;
            }


            if ( j >= i && path.legs[j]->tuple.depth == path.nodelabels.size () - 2 ) {
              // at the lowest level of the second path, no edge with a lower label than the last on
              // that path may be added. Incorporate that into computation!
              if ( path.legs[j]->tuple.edgelabel >= path.edgelabels.back () ) {
                do {
                  j--;
                }
                while ( j >= i && path.legs[j]->tuple.depth == path.nodelabels.size () - 2
                                && path.legs[j]->tuple.edgelabel >= path.edgelabels.back () );
                for ( k = j + 1; k < j2; k++ ) {
                  legoccurrencesptr = join ( leg.occurrences, path.legs[k]->tuple.connectingnode, path.legs[k]->occurrences );
                  if ( legoccurrencesptr ) {
                    addLeg ( path.legs[k]->tuple.connectingnode, path.legs[k]->tuple.depth - nodesize2, path.legs[k]->tuple.edgelabel, *legoccurrencesptr );
                  }
                }
              }
              while ( j >= i && path.legs[j]->tuple.depth == path.nodelabels.size () - 2 )
                j--;
            }

            if ( j >= i )
              addRightLegs ( path, leg, j, NODEPTH, NOEDGELABEL, rightstart, nodesize2 );

            // we're done!

          }
          else {
            // extension is an extension of the second path

        	  patterntree->rootpathstart = patterntree->nextpathstart = patterntree->treetuples.size ();

            // add second path
            for ( i = rightstart, j = 0; i < path.edgelabels.size (); i++, j++ ) {
              vector_push_back ( Tuple, patterntree->treetuples, tuple );
              tuple.label = path.edgelabels[i];
              tuple.depth = j;
            }
            // add extension tuple
            vector_push_back ( Tuple, patterntree->treetuples, tuple );
            tuple.label = leg.tuple.edgelabel;
            tuple.depth = leg.tuple.depth - nodesize2;

            patterntree->rootpathrelations.reserve ( tuple.depth + 1 );
            patterntree->rightmostindexes.reserve ( tuple.depth + 1 );
            for ( i = 0; i < tuple.depth; i++ ) {
            	patterntree->rootpathrelations.push_back ( 0 );
            	patterntree->rightmostindexes.push_back ( i + patterntree->rootpathstart );
            }
            patterntree->rightmostindexes.push_back ( patterntree->treetuples.size () - 1 );
            if ( tuple.label == patterntree->treetuples[tuple.depth + patterntree->rootpathstart ].label ) {
            	patterntree->nextprefixindex = tuple.depth + patterntree->rootpathstart + 1;
            	patterntree->rootpathrelations.push_back ( 0 );
            }
            else {
            	patterntree->nextprefixindex = NONEXTPREFIX;
            	patterntree->rootpathrelations.push_back ( patterntree->treetuples[tuple.depth + patterntree->rootpathstart].label - tuple.label );
            }

            // fill in possible extensions below the new node
            addExtensionLegs ( tuple, leg.occurrences );

            // add the leg itself

            addRightLegs ( path, leg, tuple, legindex, rightstart, nodesize2 );

            patterntree->secondpathleg = patterntree->legs.size ();
            // we're done
          }
        }
        else {
          // right part of the tree is the first path in the tree

          patterntree->treetuples.reserve ( path.edgelabels.size () + 1 );
          int i, j;
          for ( i = rightstart, j = 0; i < path.edgelabels.size (); i++, j++ ) {
            vector_push_back ( Tuple, patterntree->treetuples, tuple );
            tuple.label = path.edgelabels[i];
            tuple.depth = j;
          }

          int leftend = edgesize2;
          rightstart = leftend + 1;

          if ( leg.tuple.depth <= leftend ) {
            // extension is an extension of the left, second path
        	  patterntree->rootpathstart = patterntree->nextpathstart = patterntree->treetuples.size ();

            // add second path (the left path)
            for ( i = leftstart, j = 0; i >= 0; i--, j++ ) {
              vector_push_back ( Tuple, patterntree->treetuples, tuple );
              tuple.label = path.edgelabels[i];
              tuple.depth = j;
            }
            // add extension tuple
            vector_push_back ( Tuple, patterntree->treetuples, tuple );
            tuple.label = leg.tuple.edgelabel;
            tuple.depth = edgesize2 - leg.tuple.depth;

            patterntree->rootpathrelations.reserve ( tuple.depth + 1 );
            patterntree->rightmostindexes.reserve ( tuple.depth + 1 );
            for ( i = 0; i < tuple.depth; i++ ) {
            	patterntree->rootpathrelations.push_back ( 0 );
            	patterntree->rightmostindexes.push_back ( i + patterntree->rootpathstart );
            }
            patterntree->rightmostindexes.push_back ( patterntree->treetuples.size () - 1 );
            if ( tuple.label == patterntree->treetuples[tuple.depth + patterntree->rootpathstart ].label ) {
            	patterntree->nextprefixindex = tuple.depth + patterntree->rootpathstart + 1;
            	patterntree->rootpathrelations.push_back ( 0 );
            }
            else {
            	patterntree->nextprefixindex = NONEXTPREFIX;
            	patterntree->rootpathrelations.push_back ( patterntree->treetuples[tuple.depth + patterntree->rootpathstart].label - tuple.label );
            }


            // fill in possible extensions below the new node
            addExtensionLegs ( tuple, leg.occurrences );

            // add the leg itself, and all legs on the left path above in the tree

            addLeftLegs ( path, leg, tuple, legindex, leftend, edgesize2 );

            patterntree->secondpathleg = patterntree->legs.size ();

            // we're done
          }
          else {
            // the right path is the lowest, and we're adding a leg in that path
            // add extension tuple

            int i, j;

            vector_push_back ( Tuple, patterntree->treetuples, tuple );
            tuple.label = leg.tuple.edgelabel;
            tuple.depth = leg.tuple.depth - nodesize2;

            patterntree->rootpathrelations.reserve ( tuple.depth + 1 );
            patterntree->rightmostindexes.reserve ( tuple.depth + 1 );

            for ( i = 0; i < tuple.depth; i++ ) {
            	patterntree->rootpathrelations.push_back ( 0 );
            	patterntree->rightmostindexes.push_back ( i );
            }
            patterntree->rightmostindexes.push_back ( patterntree->treetuples.size () - 1 );
            if ( tuple.label == patterntree->treetuples[tuple.depth].label ) {
            	patterntree->nextprefixindex = tuple.depth + 1;
            	patterntree->rootpathrelations.push_back ( 0 );
            }
            else {
            	patterntree->nextprefixindex = NONEXTPREFIX;
            	patterntree->rootpathrelations.push_back ( patterntree->treetuples[tuple.depth].label - tuple.label );
            }

            patterntree->rootpathstart = 0;
            patterntree->nextpathstart = patterntree->treetuples.size ();

            // fill in possible extensions below the new node
            addExtensionLegs ( tuple, leg.occurrences );

            // add second path
            for ( i = leftstart, j = 0; i >= 0; i--, j++ ) {
              vector_push_back ( Tuple, patterntree->treetuples, tuple );
              tuple.label = path.edgelabels[i];
              tuple.depth = j;
            }

            // here, we have determined the expansion tuple sequence of the tree, and the normal
            // form information. Now we have to fill in the legs at other positions than below
            // the new node. This is complicated as the occurrence list order of the path
            // has to be changed...


            i = addRightLegs ( path, leg, tuple, legindex, rightstart, nodesize2 );

            patterntree->secondpathleg = patterntree->legs.size ();

            // the easier part - the extensions of the left side of the original path, which is the second
            // path in the tree.

            // no legs at the lowest (highest) level
            j = 0;
            while ( j <= i && path.legs[j]->tuple.depth == 0 )
              j++;

            // siblings of the node at the deepest level must have a higher or equal label to the last
            // label on the path, otherwise the string would become lower
            while ( j <= i && path.legs[j]->tuple.depth == 1
                            && path.legs[j]->tuple.edgelabel < path.edgelabels[0] )
              j++;

            if ( j <= i )
              addLeftLegs ( path, leg, j, NODEPTH, NOEDGELABEL, leftend, edgesize2 );

            // we're done!
          }
        }
      }
      else {

    	  patterntree->treetuples.reserve ( path.edgelabels.size () + 1 );
        int i, j;
        for ( i = leftstart, j = 0; i >= 0; i--, j++ ) {
          vector_push_back ( Tuple, patterntree->treetuples, tuple );
          tuple.label = path.edgelabels[i];
          tuple.depth = j;
        }

        // extension is always an extension of the first path, unless the extension
        // is performed on the middle node of a string of odd number of nodes

        // add extension tuple
        vector_push_back ( Tuple, patterntree->treetuples, tuple );
        tuple.label = leg.tuple.edgelabel;
        tuple.depth = edgesize2 - leg.tuple.depth;

        patterntree->rootpathrelations.reserve ( tuple.depth + 1 );
        patterntree->rightmostindexes.reserve ( tuple.depth + 1 );
        for ( i = 0; i < tuple.depth; i++ ) {
        	patterntree->rootpathrelations.push_back ( 0 );
        	patterntree->rightmostindexes.push_back ( i );
        }
        patterntree->rightmostindexes.push_back ( patterntree->treetuples.size () - 1 );
        if ( tuple.label == patterntree->treetuples[tuple.depth].label ) {
        	patterntree->nextprefixindex = tuple.depth + 1;
        	patterntree->rootpathrelations.push_back ( 0 );
        }
        else {
        	patterntree->nextprefixindex = NONEXTPREFIX;
        	patterntree->rootpathrelations.push_back ( patterntree->treetuples[tuple.depth].label - tuple.label );
        }

        patterntree->rootpathstart = 0;
        patterntree->nextpathstart = patterntree->treetuples.size ();

        // fill in possible extensions below the new node
        addExtensionLegs ( tuple, leg.occurrences );

        // add second path
        for ( i = rightstart, j = 0; i < path.edgelabels.size (); i++, j++ ) {
          vector_push_back ( Tuple, patterntree->treetuples, tuple );
          tuple.label = path.edgelabels[i];
          tuple.depth = j;
        }

        // here, we have determined the expansion tuple sequence of the tree, and the normal
        // form information. Now we have to fill in the legs at other positions than below
        // the new node. This is complicated as the occurrence list order of the path
        // has to be changed...

        i = addLeftLegs ( path, leg, tuple, legindex, rightstart - 1, edgesize2 );

        // a difficult part - the extensions of the right side of the original path; here the labels
        // are in correct order, but the depth order is incorrect (more precisely, reversed)
        // furthermore, as the string is symmetric, the possible legs for the second path are limitted by
        // the first side leg of the first path; the second subtree must be equal, or higher!

        patterntree->secondpathleg = patterntree->legs.size ();


        int targetdepth = path.nodelabels.size () - 1 - leg.tuple.depth;

        j = path.legs.size () - 1;

        if ( targetdepth == path.edgelabels.size () - 1 ) {
          while ( j >= i && path.legs[j]->tuple.depth > targetdepth )
            j--;
          int j2 = j;
          while ( j >= i && path.legs[j]->tuple.depth == targetdepth && path.legs[j]->tuple.edgelabel >= leg.tuple.edgelabel )
            j--;
          for ( int k = j + 1; k <= j2; k++ ) {
            LegOccurrencesPtr legoccurrencesptr = join ( leg.occurrences, path.legs[k]->tuple.connectingnode, path.legs[k]->occurrences );
            if ( legoccurrencesptr )
              addLeg ( path.legs[k]->tuple.connectingnode, tuple.depth, path.legs[k]->tuple.edgelabel, *legoccurrencesptr );
          }
          while ( j >= i && path.legs[j]->tuple.depth == targetdepth )
            j--;
        }
        else {
          while ( j >= i && path.legs[j]->tuple.depth > targetdepth )
            j--;
          if ( j >= i ) {
            if ( patterntree->rootpathrelations.back () != 0 && path.legs[j]->tuple.depth == targetdepth ) {
              int j2 = j;
              EdgeLabel lowestlabel = path.edgelabels[targetdepth];
              while ( j >= i &&
    	          path.legs[j]->tuple.depth == targetdepth &&
    	  	  path.legs[j]->tuple.edgelabel >= tuple.label )
                j--;
              for ( int k = j + 1; k <= j2; k++ )
                if ( path.legs[k]->tuple.edgelabel != lowestlabel ) {
                  LegOccurrencesPtr legoccurrencesptr = join ( leg.occurrences, path.legs[k]->tuple.connectingnode, path.legs[k]->occurrences );
                  if ( legoccurrencesptr )
                    addLeg ( path.legs[k]->tuple.connectingnode, tuple.depth, path.legs[k]->tuple.edgelabel, *legoccurrencesptr );
              }
              while ( j >= i && path.legs[j]->tuple.depth == targetdepth )
                j--;
            }
          }
        }

        if ( j >= i )
          addRightLegs ( path, leg, j, NODEPTH, NOEDGELABEL, rightstart, nodesize2 );
            // we're done!

        // symmetry
      }

      // ADDED
      graphstate.backbonelength = path.nodelabels.size ();
      if ( graphstate.backbonelength % 2 == 0 )
        graphstate.bicenterlabel = path.edgelabels [ graphstate.backbonelength / 2 - 1 ];
      else
        graphstate.centerlabel = path.nodelabels [ ( graphstate.backbonelength - 1 ) / 2 ];
      graphstate.nasty = false;
      graphstate.treetuples = &(patterntree->treetuples);
      graphstate.closetuples = NULL;
      graphstate.startsecondpath = patterntree->nextpathstart;
    }


    void createTree ( PatternTree &parenttree, unsigned int legindex ) {
      patterntree = new PatternTree;
      Leg &leg = * ( parenttree.legs[legindex] );

      OUTPUT(parenttree.legs[legindex]->occurrences.frequency, ostr);

      addCloseExtensions ( patterntree->closelegs, parenttree.closelegs, leg.occurrences );

      patterntree->symmetric = parenttree.symmetric;
      // update information used to determine canonical form
      int i, pos;

      int maxleg;


      patterntree->treetuples.reserve ( parenttree.treetuples.size () );
      if ( !parenttree.rootpathstart && legindex < parenttree.secondpathleg ) {
        for ( i = 0; i < parenttree.nextpathstart; i++ )
        	patterntree->treetuples.push_back ( parenttree.treetuples[i] );
        pos = patterntree->treetuples.size ();
        patterntree->treetuples.push_back ( leg.tuple );
        patterntree->nextpathstart = patterntree->treetuples.size ();
        patterntree->rootpathstart = 0;

        for ( ; i < parenttree.treetuples.size (); i++ )
        	patterntree->treetuples.push_back ( parenttree.treetuples[i] );

        maxleg = parenttree.secondpathleg;
      }
      else {
    	patterntree->nextpathstart = patterntree->rootpathstart = parenttree.nextpathstart;
        for ( i = 0; i < parenttree.treetuples.size (); i++ )
        	patterntree->treetuples.push_back ( parenttree.treetuples[i] );
        pos = patterntree->treetuples.size ();
        patterntree->treetuples.push_back ( leg.tuple );

        maxleg = parenttree.legs.size ();
      }

      patterntree->maxdepth = parenttree.maxdepth;

      patterntree->rootpathrelations.reserve ( leg.tuple.depth + 2 );
      patterntree->rightmostindexes.reserve ( leg.tuple.depth + 2 );
      if ( parenttree.rootpathstart == 0 && legindex >= parenttree.secondpathleg ) {
        // we're going from the first path to the second
        for ( i = 0; i < leg.tuple.depth; i++ ) {
        	patterntree->rootpathrelations.push_back ( 0 );
        	patterntree->rightmostindexes.push_back ( patterntree->rootpathstart + i );
        }
        patterntree->rootpathrelations.push_back ( patterntree->treetuples[patterntree->rootpathstart + i].label - leg.tuple.label );
        patterntree->rightmostindexes.push_back ( patterntree->treetuples.size () - 1 );
        if ( patterntree->symmetric && patterntree->treetuples[patterntree->maxdepth + 1] == patterntree->treetuples.back () )
          // in this case, we're copying the previous tree
        	patterntree->nextprefixindex = patterntree->maxdepth + 2;
        else
          if ( patterntree->treetuples[patterntree->rootpathstart + leg.tuple.depth].label == leg.tuple.label )
        	  patterntree->nextprefixindex = patterntree->rootpathstart + leg.tuple.depth + 1;
          else
        	  patterntree->nextprefixindex = NONEXTPREFIX;
      }
      else {
        for ( i = 0; i < leg.tuple.depth; i++ ) {
        	patterntree->rootpathrelations.push_back ( parenttree.rootpathrelations[i] );
        	patterntree->rightmostindexes.push_back ( parenttree.rightmostindexes[i] );
        }
        patterntree->rightmostindexes.push_back ( pos );
        if ( patterntree->rootpathrelations.size () == 0 || patterntree->rootpathrelations.back () ==  0 )
        	patterntree->rootpathrelations.push_back ( patterntree->treetuples[patterntree->rootpathstart + leg.tuple.depth].label - leg.tuple.label );
        else
        	patterntree->rootpathrelations.push_back ( patterntree->rootpathrelations.back () );

        if ( parenttree.nextprefixindex != NONEXTPREFIX && parenttree.treetuples[parenttree.nextprefixindex] == leg.tuple )
        	patterntree->nextprefixindex = parenttree.nextprefixindex + 1;
        else {
          if ( leg.tuple.depth < parenttree.rightmostindexes.size () &&
               parenttree.treetuples[parenttree.rightmostindexes[leg.tuple.depth]].label == leg.tuple.label )
        	  patterntree->nextprefixindex = parenttree.rightmostindexes[leg.tuple.depth] + 1;
          else
        	  patterntree->nextprefixindex = NONEXTPREFIX;
        }
      }

      // ADDED
      graphstate.treetuples = &(patterntree->treetuples);
      graphstate.closetuples = NULL;
      graphstate.startsecondpath = patterntree->nextpathstart;

      if ( patterntree->nextprefixindex == patterntree->nextpathstart && patterntree->symmetric == 1 ) {
    	  patterntree->secondpathleg = 0; // THE BUG
        extend ( leg.occurrences, MAXEDGELABEL, NONODE );
        addCloseExtensions ( patterntree->closelegs, leg.occurrences.number );
        return;
      }

      // determine legs that can be added at leg.tuple.depth + 1 (if possible) (phase 1)

      addExtensionLegs ( leg.tuple, leg.occurrences );

      int index = legindex;

      if ( patterntree->nextprefixindex != NONEXTPREFIX && patterntree->treetuples[patterntree->nextprefixindex].depth <= leg.tuple.depth ) {
        Depth nextprefixdepth = patterntree->treetuples[patterntree->nextprefixindex].depth;
        EdgeLabel nextprefixlabel = patterntree->treetuples[patterntree->nextprefixindex].label;
        EdgeLabel lowestlabel = patterntree->treetuples[patterntree->rootpathstart + nextprefixdepth].label;
        while ( index < maxleg  &&
                parenttree.legs[index]->tuple.depth > nextprefixdepth )
          index++;
        if ( index < maxleg && parenttree.legs[index]->tuple.depth == nextprefixdepth &&
             nextprefixlabel != lowestlabel ) {
          if ( parenttree.legs[index]->tuple.label == lowestlabel )
            index++;
          while ( index < maxleg &&
                  parenttree.legs[index]->tuple.depth == nextprefixdepth &&
    	      parenttree.legs[index]->tuple.label < nextprefixlabel )
    	index++;
        }
      }

      if ( index == legindex ) {
        LegOccurrencesPtr legoccurrencesptr = join ( leg.occurrences );
        if ( legoccurrencesptr )
          addLeg ( leg.tuple.connectingnode, leg.tuple.depth, leg.tuple.label, *legoccurrencesptr );
        index++;
      }

      if ( patterntree->rootpathstart == 0 ) {
    	  patterntree->secondpathleg = patterntree->legs.size (); // THE BUG
        while ( index < parenttree.legs.size () ) {
          if ( index == parenttree.secondpathleg )
        	  patterntree->secondpathleg = patterntree->legs.size ();
          LegOccurrencesPtr legoccurrencesptr = join ( leg.occurrences, parenttree.legs[index]->tuple.connectingnode, parenttree.legs[index]->occurrences );
          if ( legoccurrencesptr )
            addLeg ( parenttree.legs[index]->tuple.connectingnode, parenttree.legs[index]->tuple.depth, parenttree.legs[index]->tuple.label, *legoccurrencesptr );
          index++;
        }
        if ( index == parenttree.secondpathleg )
        	patterntree->secondpathleg = patterntree->legs.size ();
      }
      else {
        while ( index < parenttree.legs.size () ) {
          LegOccurrencesPtr legoccurrencesptr = join ( leg.occurrences, parenttree.legs[index]->tuple.connectingnode,  parenttree.legs[index]->occurrences );
          if ( legoccurrencesptr )
            addLeg ( parenttree.legs[index]->tuple.connectingnode, parenttree.legs[index]->tuple.depth, parenttree.legs[index]->tuple.label, *legoccurrencesptr );
          index++;
        }
        patterntree->secondpathleg = patterntree->legs.size ();
      }
    }


    //for graph
    void init ( vector<CloseLegPtr> &closelegssource, int legindex ) {
      patterngraph->closetuples.push_back ( closelegssource[legindex]->tuple );
      patterngraph->frequency = closelegssource[legindex]->occurrences.frequency;

      // ADDED
      graphstate.closetuples = &(patterngraph->closetuples);
      graphstate.treetuples = &(patterngraph->treetuples);

      patterngraph->closelegssource.resize(closelegssource.size());
      for(int i = 0; i<closelegssource.size();i++){
    	  patterngraph->closelegssource[i] = new CloseLeg;
    	  *(patterngraph->closelegssource[i]) = *(closelegssource[i]); //todo hange by wenwen, a
      }
      patterngraph->legindex = legindex;

    }
    void createGraph( PatternTree &tree, int legindex ) {
    	patterngraph = new PatternGraph;
    	patterngraph->treetuples = tree.treetuples;
        init ( tree.closelegs, legindex );
    }

    void createGraph( PatternGraph &parentpatterngraph, int legindex ) {
      patterngraph = new PatternGraph;
      patterngraph->treetuples = parentpatterngraph.treetuples;
      patterngraph->closetuples = parentpatterngraph.closetuples;
      init ( parentpatterngraph.closelegs, legindex );
    }


	void print(ostream& fout){
		fout << "t ";
		fout << "# " << ID++ << endl;
		for ( int i = 0; i < graphstate.nodes.size (); i++ ) {
			fout << "v " << i <<' '<<(int) database.nodelabels[graphstate.nodes[i].label].inputlabel<<endl;
		}
		for ( int i = 0; i < graphstate.nodes.size (); i++ ) {
			for ( int j = 0; j < graphstate.nodes[i].edges.size (); j++ ) {
				GraphState::GSEdge &edge = graphstate.nodes[i].edges[j];
				if ( i < edge.tonode ) {
					fout << "e " << i << ' ' <<(int) edge.tonode << ' ' << (int) database.edgelabels[database.edgelabelsindexes[edge.edgelabel]].inputedgelabel << endl;
				}
			}
		}
	}

	void OUTPUT(Frequency frequency, ostream* ostr){
		*ostr << "# " << frequency  << endl;
		print ( *(ostr) );
	}

};




#endif /* APP_GASTON_GASTONPATTERN_H_ */

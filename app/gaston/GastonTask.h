/*
 * GastonTask.h
 *
 *  Created on: 2021Äê3ÔÂ15ÈÕ
 *      Author: Administrator
 */

#ifndef APP_GASTON_GASTONTASK_H_
#define APP_GASTON_GASTONTASK_H_

#include "GastonPattern.h"


struct ChildrenT{
	vector<int> pathChildrenList;
	vector<int> treeChildrenList;
	vector<int> graphChildrenList;

	vector<int>::iterator pathIter;
	vector<int>::iterator treeIter;
	vector<int>::iterator graphIter;
};

class GastonTask: public Task<GastonPattern, ChildrenT, Database > {
public:
	int patternsize;

	virtual bool pre_check(ostream& fout){
		pat.ostr = &fout; // to track fout so that it can be used in setChildren
		return true;
	}

	virtual void setChildren(ChildrenT& children){
		//by wenwen : from an existed path to new path
		if (pat.stage == 0){ //from root path pattern
		  for ( int i = 0; i < pat.path->legs.size (); i++ ) {
		    PathTuple &tuple = pat.path->legs[i]->tuple;
		    if ( tuple.nodelabel >= pat.path->nodelabels[0] ) {
		      //pat.graphstate.insertNode ( tuple.connectingnode, tuple.edgelabel, pat.path->legs[i]->occurrences.maxdegree );
		      children.pathChildrenList.push_back(i);
		      //graphstate.deleteNode ();
		    }
		  }
		  //pat.graphstate.deleteStartNode ();
		}


		else if(pat.stage ==1){ //from other path pattern
			  // does not work for strings with only one node
			  if ( pat.path->closelegs.size () && phase > 2 ) {

			    NodeId from = pat.graphstate.nodes.size ();
			    NodeId to = 0;
			    while ( pat.graphstate.nodes[to].edges.size () == 2 )
			      to++;
			    to++;

			    for ( int i = 0; i < pat.path->closelegs.size (); i++ ) {
			      if ( pat.path->closelegs[i]->tuple.from == from &&
			    		  pat.path->closelegs[i]->tuple.to == to &&
						  pat.path->isnormal ( pat.path->closelegs[i]->tuple.label ) ) {
			    	  pat.graphstate.insertEdge ( pat.path->closelegs[i]->tuple.from, pat.path->closelegs[i]->tuple.to, pat.path->closelegs[i]->tuple.label );
			    	  pat.OUTPUT(pat.path->closelegs[i]->occurrences.frequency,  pat.ostr);
			    	  pat.graphstate.deleteEdge ( pat.path->closelegs[i]->tuple.from, pat.path->closelegs[i]->tuple.to );

			        // DON'T RECURSE GRAPH GROWING!
			        // only circle graphs can only grow from paths, all other graphs
			        // can grow from spanning trees, which we prefer for now
			      }
			    }
			  }

			  for ( int i = 0; i < pat.path->legs.size (); i++ ) {
			    PathTuple &tuple = pat.path->legs[i]->tuple;
			    if ( tuple.depth == pat.path->nodelabels.size () - 1 ) {
			      if ( tuple.nodelabel > pat.path->nodelabels[0] ||
			           ( tuple.nodelabel == pat.path->nodelabels[0] &&
			             ( tuple.edgelabel > pat.path->edgelabels[0] ||
			               ( tuple.edgelabel == pat.path->edgelabels[0] && pat.path->backsymmetry <= 0 )
			             )
			           ) ) {
			    	  //pat.graphstate.insertNode ( pat.path->legs[i]->tuple.connectingnode, pat.path->legs[i]->tuple.edgelabel, pat.path->legs[i]->occurrences.maxdegree );
			    	  children.pathChildrenList.push_back(i );
			      }
			    }
			    else
			      if ( pat.path->legs[i]->tuple.depth == 0 ) {
			        if ( pat.path->totalsymmetry &&
			             ( tuple.nodelabel > pat.path->nodelabels.back () ||
			             ( tuple.nodelabel == pat.path->nodelabels.back () &&
			               ( tuple.edgelabel > pat.path->edgelabels.back () ||
			                 ( tuple.edgelabel == pat.path->edgelabels.back () && pat.path->frontsymmetry >= 0 )
			               )
			             ) ) ) {
			        	//pat.graphstate.insertNode ( pat.path->legs[i]->tuple.connectingnode, pat.path->legs[i]->tuple.edgelabel, pat.path->legs[i]->occurrences.maxdegree );
			        	children.pathChildrenList.push_back(i );
			        }
			      }
			      else {
			        if ( ( pat.path->totalsymmetry || pat.path->legs[i]->tuple.depth <= pat.path->edgelabels.size () / 2 ) &&
				     ( pat.path->legs[i]->tuple.depth != 1 || pat.path->legs[i]->tuple.edgelabel >= pat.path->edgelabels[0] ) &&
				     ( pat.path->legs[i]->tuple.depth != pat.path->nodelabels.size () - 2 || pat.path->legs[i]->tuple.edgelabel >= pat.path->edgelabels.back () ) &&
				     phase > 1
				   ) {
			        	// grow tree
			        	//pat.graphstate.insertNode ( pat.path->legs[i]->tuple.connectingnode, pat.path->legs[i]->tuple.edgelabel, pat.path->legs[i]->occurrences.maxdegree );
			        	children.treeChildrenList.push_back(i );

			        }
			      }
			  }
			  patternsize--;
		}

		else if(pat.stage ==2){ //from tree
			  if ( phase > 2 )
			    for ( int i = 0; i < pat.patterntree->closelegs.size(); i++ ) {
			      //pat.graphstate.insertEdge ( pat.patterntree->closelegs[i]->tuple.from, pat.patterntree->closelegs[i]->tuple.to, pat.patterntree->closelegs[i]->tuple.label );
			      //PatternGraph p ( pat.patterntree->closelegs, i );
			      //p.expand ();
			      //pat.graphstate.deleteEdge ( pat.patterntree->closelegs[i]->tuple.from, pat.patterntree->closelegs[i]->tuple.to );
			    	children.graphChildrenList.push_back(i );
			    }

			  for ( int i = pat.patterntree->legs.size () - 1; i >= 0; i-- ) {
			    //pat.graphstate.insertNode ( pat.patterntree->legs[i]->tuple.connectingnode, pat.patterntree->legs[i]->tuple.label, pat.patterntree->legs[i]->occurrences.maxdegree );
			    //PatternTree p ( *this, i );
			    //p.expand ();
			    //graphstate.deleteNode ();
				  children.treeChildrenList.push_back(i );
			  }
			  patternsize--;
		}
		else{//from graph
			  int id = pat.graphstate.isnormal ();
			  if ( id == 0 ) {
			    for ( int k = pat.patterngraph->legindex + 1; k < pat.patterngraph->closelegssource.size (); k++ ) {
			      if ( ((pat.patterngraph->closelegssource))[k]->copy ) {
			        CloseLegOccurrencesPtr closelegoccurrencesptr =
			          pat.join ( ((pat.patterngraph->closelegssource))[pat.patterngraph->legindex]->occurrences, ((pat.patterngraph->closelegssource))[k]->occurrences );
			        if ( closelegoccurrencesptr ) {
			          CloseLegPtr closelegptr = new CloseLeg;
			          closelegptr->tuple = ((pat.patterngraph->closelegssource))[k]->tuple;
			          swap ( *closelegoccurrencesptr, closelegptr->occurrences );
			          pat.patterngraph->closelegs.push_back ( closelegptr );
			        }
			      }
			    }
			    pat.OUTPUT(pat.patterngraph->frequency,pat.ostr);

			    for ( int k = pat.patterngraph->closelegs.size () - 1; k >= 0; k-- ) {
			      //pat.graphstate.insertEdge ( pat.patterngraph->closelegs[k]->tuple.from, pat.patterngraph->closelegs[k]->tuple.to, pat.patterngraph->closelegs[k]->tuple.label );
			      //PatternGraph patterngraph ( *this, k );
			      //patterngraph.expand ();
			      //graphstate.deleteEdge ( closelegs[k]->tuple.from, closelegs[k]->tuple.to );
			    	children.graphChildrenList.push_back(k );
			    }
			  }
			  else
			    if ( id == 2 ) {
			      ((pat.patterngraph->closelegssource))[pat.patterngraph->legindex]->copy = false; // should not be added to any later tree
			    }
		}

		children.pathIter = children.pathChildrenList.begin();
		children.treeIter = children.treeChildrenList.begin();
		children.graphIter = children.graphChildrenList.begin();
	}


	void copyState(GastonPattern& newPat, GastonPattern& parent){
		newPat.graphstate = parent.graphstate;

		newPat.graphstate.deletededges = parent.graphstate.deletededges;
		newPat.graphstate.nodes = parent.graphstate.nodes;
		newPat.graphstate.edgessize =parent.graphstate.edgessize;

		newPat.candidatelegsoccurrences = parent.candidatelegsoccurrences;
		//newPat.legoccurrences = parent.legoccurrences;
		//newPat.closelegsoccsused = parent.closelegsoccsused;
		//newPat.candidatecloselegsoccs = parent.candidatecloselegsoccs;
		//newPat.candidatecloselegsoccsused = parent.candidatecloselegsoccsused;
		//newPat.closelegoccurrences = parent.closelegoccurrences;

		newPat.ostr = pat.ostr;
	}

	virtual Task* get_next_child(){
		if(pat.stage <2){
			while(children.pathIter !=children.pathChildrenList.end()){
				int i = *(children.pathIter);

				GastonTask* newTask = new GastonTask;
				newTask->pat.stage = 1;
				newTask->patternsize = patternsize+1;
				GastonPattern& newPat = newTask->pat;

				copyState(newPat, pat);

				PathTuple &tuple = pat.path->legs[i]->tuple;
				newPat.graphstate.insertNode ( tuple.connectingnode, tuple.edgelabel, pat.path->legs[i]->occurrences.maxdegree );
				//new pattern is old pattern plus new element
				newPat.createPath(*(pat.path), i);



				children.pathIter++;

				return newTask;
			}

			while(children.treeIter !=children.treeChildrenList.end()){
				int i = *(children.treeIter);

				GastonTask* newTask = new GastonTask;
				newTask->pat.stage = 2;
				newTask->patternsize = patternsize+1;
				GastonPattern& newPat = newTask->pat;

				copyState(newPat, pat);

				PathTuple &tuple = pat.path->legs[i]->tuple;
				newPat.graphstate.insertNode ( tuple.connectingnode, tuple.edgelabel, pat.path->legs[i]->occurrences.maxdegree );
				//new pattern is old pattern plus new element
				newPat.createTree(*(pat.path), i);



				children.treeIter++;

				return newTask;
			}
		}
		if(pat.stage == 2){
			while(children.treeIter !=children.treeChildrenList.end()){
				int i = *(children.treeIter);

				GastonTask* newTask = new GastonTask;
				newTask->pat.stage = 2;
				newTask->patternsize = patternsize+1;
				GastonPattern& newPat = newTask->pat;

				copyState(newPat, pat);

				newPat.graphstate.insertNode ( pat.patterntree->legs[i]->tuple.connectingnode, pat.patterntree->legs[i]->tuple.label, pat.patterntree->legs[i]->occurrences.maxdegree );
				newPat.createTree(*(pat.patterntree), i);



				children.treeIter++;

				return newTask;
			}

			while(children.graphIter !=children.graphChildrenList.end()){
				int i = *(children.graphIter);

				GastonTask* newTask = new GastonTask;
				newTask->pat.stage = 3;
				newTask->patternsize = patternsize+1;
				GastonPattern& newPat = newTask->pat;

				copyState(newPat, pat);

				newPat.graphstate.insertEdge ( pat.patterntree->closelegs[i]->tuple.from, pat.patterntree->closelegs[i]->tuple.to, pat.patterntree->closelegs[i]->tuple.label );
				newPat.createGraph(*(pat.patterntree), i);


				children.graphIter++;

				return newTask;
			}
		}

		if(pat.stage == 3){
			while(children.graphIter !=children.graphChildrenList.end()){
				int k = *(children.graphIter);

				GastonTask* newTask = new GastonTask;
				newTask->pat.stage = 3;
				newTask->patternsize = patternsize+1;
				GastonPattern& newPat = newTask->pat;

				copyState(newPat, pat);

				newPat.graphstate.insertEdge ( pat.patterngraph->closelegs[k]->tuple.from, pat.patterngraph->closelegs[k]->tuple.to, pat.patterngraph->closelegs[k]->tuple.label );
				newPat.createGraph(*(pat.patterngraph), k);



				children.graphIter++;

				return newTask;
			}
		}
		return 0;
	}

	virtual bool needSplit() // whether to put new tasks got from get_next_child() into task queue, or process it by current thread
	{
		return true;
		/*
		int prodbSize = 0;
		int i;
		if(pat.stage < 3){
			for(i = 0; i<pat.path->legs.size(); i++)
				prodbSize += pat.path->legs[i]->occurrences.elements.size();
			for(i = 0; i<pat.path->closelegs.size(); i++)
				prodbSize += pat.path->closelegs[i]->occurrences.elements.size();
		}
		else if(pat.stage == 3)
			for(i = 0; i<pat.path->closelegs.size(); i++)
				prodbSize += pat.path->closelegs[i]->occurrences.elements.size();
		return (prodbSize >= tauDB_singlethread);
		*/
	}


};



#endif /* APP_GASTON_GASTONTASK_H_ */

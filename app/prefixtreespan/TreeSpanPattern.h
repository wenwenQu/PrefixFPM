/*
 * TreeSpanPattern.h
 *
 *  Created on: Apr 19, 2020
 *      Author: wen
 */

#ifndef PREFIXTREESPAN_TREESPANPATTERN_H_
#define PREFIXTREESPAN_TREESPANPATTERN_H_
#include "TreeSpanTrans.h"


class TreeSpanPattern: public Pattern {
public:
	Tree pattern_tree; // elements of a tree (in order)
	vector<ProjTree> projDB; // should be set from outside, usually passed in from children_map

	virtual void print(ostream& fout){
		for(vector<int>::iterator it = pattern_tree.hcodes.begin(); it!=pattern_tree.hcodes.end(); it++) fout << *it << " ";
		fout << " |PDB| = " << projDB.size() << endl;
	}
};


#endif /* PREFIXTREESPAN_TREESPANPATTERN_H_ */

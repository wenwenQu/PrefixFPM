/*
 * TreeSpanTrans.h
 *
 *  Created on: Apr 19, 2020
 *      Author: wen
 */

#ifndef TREESPANTRANS_H_
#define TREESPANTRANS_H_
#include <map>
#include <stack>
#include <set>

#include "Utils.h"

//--------------------------------

class Element{
public:
	int label;
	int pos; //father position
	Element(int l, int p): label(l), pos(p){};

	friend bool operator<(const Element& e1, const Element& e2)
		{
		    if(e1.pos<e2.pos)	return true;
		    if((e1.pos == e2.pos) && e1.label < e2.label) return true;
		    return false;
		}
};

//---------------------------

class Tree: public Trans{
public:
	vector<int> hcodes; //horizontal codes
	vector<int> partner;

	void compute_scope(){
		partner.resize(hcodes.size());
		stack<int> label_stack ;
		for(int i = 0; i<hcodes.size(); i++){
			if(hcodes[i] != -1)
				label_stack.push(i);
			else{
				int start = label_stack.top();
				label_stack.pop();
				partner[start] = i;
				partner[i] = -1;
			}
		}
	}
};

//---------------------------

class ProjTree: public ProjTrans{
public:
	vector<int> pattern_tree_nodes;

	void get_scope(Tree& t, int& left, int& right){
		int root = pattern_tree_nodes[0];
		left = pattern_tree_nodes.back();
		right = t.partner[root];
	}
};

////pdb to certain pattern
class treePDB: public vector<ProjTree> {
public:
	int sup; // pdb may contain multiple matched instances of the same transaction
	void push (int tid, int pos, ProjTree *prev)
	{
		resize (size() + 1);
		ProjTree &d = (*this)[size()-1];
		d.tid = tid;
		if(prev!=0)
			d.pattern_tree_nodes = prev->pattern_tree_nodes;
		d.pattern_tree_nodes.push_back(pos);
	}
	//support counting. Each tree will support current pattern only once
	unsigned int support ()
	{
		unsigned int size = 0;
		set<unsigned int> visited;

		for (vector<ProjTree>::iterator cur = begin(); cur != end(); ++cur) {
			int tid = cur->tid;
			if (visited.find(tid) == visited.end()) {
				visited.insert(tid);
				++size;
			}
		}

		sup = size;
		return size;
	}
};

#endif /* TREESPANTRANS_H_ */

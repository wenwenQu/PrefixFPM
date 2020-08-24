/*
 * TreeSpanTask.h
 *
 *  Created on: Apr 20, 2020
 *      Author: wen
 */

#ifndef PREFIXTREESPAN_TREESPANTASK_H_
#define PREFIXTREESPAN_TREESPANTASK_H_
#include "TreeSpanPattern.h"

typedef map<Element, treePDB> ChildrenT;
class TreeSpanTask: public Task<TreeSpanPattern, ChildrenT, vector<Tree> >{
public:
	ChildrenT::iterator it;
	// with omp, branching on tauDB_omp
	virtual void setChildren(ChildrenT& children) {
		//get current projected DB (to set children's projected DBs)
		vector<ProjTree>& oldPDB = pat.projDB;

		if(true)
		{
			//--------------------consume 2/3 time
			for (int i=0; i<oldPDB.size(); i++) {
				//----------------consume 1/7 time
				int left, right;
				ProjTree& proj = oldPDB[i];
				Tree& tree = TransDB()[proj.tid];
				proj.get_scope(tree,left,right);
				//-------------------------


				for(int j = left+1; j<right; j++){
					int label = tree.hcodes[j];

					if(label != -1 && label != -3) {
						//----------------consume 1/7 time
						int father_pos = -1;
						vector<int>& pattern_nodes = proj.pattern_tree_nodes;
						for(int k = 0; k<pattern_nodes.size(); k++){
							if(j > pattern_nodes[k] && j <= tree.partner[pattern_nodes[k]]){
								father_pos = k;
							}
						}
						//-------------------------

						//--------------consume 2/5 time
						Element new_node(label, father_pos);
						children[new_node].push(proj.tid, j, &proj);
						//--------------------------
					}
				}
			}
		}
		else
		{
			vector<ChildrenT> childrenOfThread(THREADS);
			// parallel-for, set the array of childrenOfThread
			#pragma omp parallel for schedule(dynamic, CHUNK) num_threads(THREADS)
			for (int i=0; i<oldPDB.size(); i++) {
				int thread_id = omp_get_thread_num();
				ChildrenT& child_list = childrenOfThread[thread_id];
				//-----------
				int left, right;
				ProjTree& proj = oldPDB[i];
				Tree& tree = TransDB()[proj.tid];
				proj.get_scope(tree,left,right);
				for(int j = left+1; j<right; j++){
					int label = tree.hcodes[j];
					//cout << "@" << left << " " << right << " " << proj.tid << " " << label << endl;
					if(label != -1 && label!= -3) {
						int father_pos = -1;
						vector<int>& pattern_nodes = proj.pattern_tree_nodes;
						for(int k = 0; k<pattern_nodes.size(); k++){
							if(j > pattern_nodes[k] && j <= tree.partner[pattern_nodes[k]]){
								father_pos = k;
							}
						}
						Element new_node(label, father_pos);
						child_list[new_node].push(proj.tid, j, &proj);
					}
				}
			}
			// merge childrenOfThread elements into children
			for(int i = 0; i < THREADS; i++){
				ChildrenT& children_i = childrenOfThread[i];
				for(ChildrenT::iterator it = children_i.begin(); it!= children_i.end(); it++){
					const Element& e = it->first;
					treePDB& pdb_i = it->second;
					treePDB& pdb = children[e];
					pdb.insert(pdb.end(), pdb_i.begin(), pdb_i.end());
					pdb_i.clear(); // to release memory space timely
				}
				children_i.clear(); // to release memory space timely
			}
		}

		//---------------------consume 1/3 time
		//delete child patterns with PDBs not big enough
		for (ChildrenT::iterator it = children.begin(); it != children.end();)
		{
			treePDB& pdb = it->second;

			if (pdb.support() < minsup) {
				ChildrenT::iterator tmp = it;
				++tmp;
				children.erase(it);
				it = tmp;
			} else {
				++it;
			}
		}
		//------------------------

		// set iterator as the first element
		it = children.begin();
	}

	Task* project(const Element& e, treePDB& projDB){
		const Element& node = it->first;
		treePDB& pdb = it->second;

		int cnt = -1;
		int end = 0;
		for(int i =0; i<pat.pattern_tree.hcodes.size(); i++){
			if(pat.pattern_tree.hcodes[i] != -1){
				cnt += 1;
				if(cnt == node.pos){
					end = pat.pattern_tree.partner[i];
					break;
				}
			}
		}
		Task* newT = new TreeSpanTask;
		TreeSpanPattern& newPat = newT->pat;
		newPat.pattern_tree.hcodes.assign(pat.pattern_tree.hcodes.begin(), pat.pattern_tree.hcodes.end());
		newPat.pattern_tree.hcodes.insert(newPat.pattern_tree.hcodes.begin()+end, e.label);
		newPat.pattern_tree.hcodes.insert(newPat.pattern_tree.hcodes.begin()+end+1, -1);
		newPat.pattern_tree.compute_scope();
		newPat.projDB.swap(projDB);
		return newT;
	}

	virtual Task* get_next_child(){
		if(it != children.end()){
			Task* newTask = this->project(it->first, it->second);
			it++;
			return newTask;
		}
		return 0;
	}

	virtual bool pre_check(ostream& fout){
		pat.print(fout);
		return true;
	}
        virtual bool needSplit(){
                if(pat.projDB.size() > tauDB_singlethread) return true;
                return false;
        };
};




#endif /* PREFIXTREESPAN_TREESPANTASK_H_ */

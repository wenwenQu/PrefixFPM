/*
 * TreeSpanWorker.h
 *
 *  Created on: Apr 21, 2020
 *      Author: wen
 */

#ifndef PREFIXTREESPAN_TREESPANWORKER_H_
#define PREFIXTREESPAN_TREESPANWORKER_H_
#include <strstream>
#include "TreeSpanTask.h"

class TreeSpanWorker:public Worker<TreeSpanTask> {
public:
	ifstream fd;
	int trans_cnt; // to used for adding IDs to transactions when loading data

	TreeSpanWorker(const char *infile, const char *outfolder = "outputs"): Worker(infile, outfolder), trans_cnt(0)
	{}

	//set map[label] = count
	virtual int getNextTrans(TransDBT& transDB){
		string line;
	    int item;
		if(getline (fd, line)){
			TransDB().resize(TransDB().size()+1);
	    	Tree& tmp = TransDB().back();
	    	tmp.tid = trans_cnt++;
	        istrstream istrs ((char *)line.c_str());
	        istrs >> item;
	        istrs >> item;
	        istrs >> item;
	        while (istrs >> item) tmp.hcodes.push_back (item);
	        tmp.hcodes.push_back (-1);
	        tmp.compute_scope();
	    	return 1;
	    }

		return 0;
	}

	virtual void read(const char *filename){
		fd.open(filename, ios::in);
		if (!fd) {
			cerr << "cannot open input file " << filename << endl;
			exit(1);
		}
		//read all lines
		TransDBT& db = TransDB();
		while (getNextTrans(db));
		fd.close();
	}

	virtual void setRoot(stack<TreeSpanTask*>& task_queue){
		ChildrenT children;
		if(false)
		{
			for (int i=0; i<TransDB().size(); i++) {
				Tree& tree = TransDB()[i];
				for (unsigned int j = 0; j < tree.hcodes.size(); j++) {
					if(tree.hcodes[j] != -1){
						children[Element(tree.hcodes[j], -1)].push(i, j, NULL);
					}
				}
			}
		}
		else
		{
			vector<ChildrenT> childrenOfThread(THREADS);
			// parallel-for, set the array of childrenOfThread
			#pragma omp parallel for schedule(dynamic, CHUNK) num_threads(THREADS)
			for (int i=0; i<TransDB().size(); i++) {
				int thread_id = omp_get_thread_num();
				ChildrenT& child_list = childrenOfThread[thread_id];
				//---------
				Tree& tree = TransDB()[i];
				for (unsigned int j = 0; j < tree.hcodes.size(); j++) {
					if(tree.hcodes[j] != -1){
						child_list[Element(tree.hcodes[j], -1)].push(i, j, NULL);
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
		//delete child patterns with PDBs not big enough
		set<int> frequent_labels;
		for (ChildrenT::iterator it = children.begin(); it != children.end();)
		{
			if (it->second.support() < minsup) {
				ChildrenT::iterator tmp = it;
				++tmp;
				children.erase(it);
				it = tmp;
			} else {
				frequent_labels.insert(it->first.label);
				TreeSpanTask *t = new TreeSpanTask;
				t->pat.pattern_tree.hcodes.push_back(it->first.label);
				t->pat.pattern_tree.hcodes.push_back(-1);
				t->pat.pattern_tree.compute_scope();
				t->pat.projDB.swap(it->second);
				task_queue.push(t);
				++it;
			}
		}
		for (int i=0; i<TransDB().size(); i++) {
			Tree& tree = TransDB()[i];
			for(int j = 0;  j<tree.hcodes.size(); j++){
				if(frequent_labels.find(tree.hcodes[j]) == frequent_labels.end()){
					tree.hcodes[j] = -3; //-3 means infrequent label
				}
			}
		}
	}
};


#endif /* PREFIXTREESPAN_TREESPANWORKER_H_ */

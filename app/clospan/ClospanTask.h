/*
 * TreeSpanTask.h
 *
 *  Created on: Apr 20, 2020
 *      Author: wen
 */

#ifndef CLOSPANTASK_H_
#define CLOSPANTASK_H_
#include "ClosedTree.h"
#include <map>

typedef vector<PROJ_DB> ChildrenT;
TimeTracker t1,t2,t3,t4;
int cnt = 0;
class ClospanTask: public Task<ClospanPattern, ChildrenT, Seq>{
public:
	ChildrenT::iterator it;

	TreeNode *currentLevel; // current processed node

	// with omp, branching on tauDB_omp
	virtual void setChildren(ChildrenT& children) {
		int i = 0, j = 0, nFreqCount = 0;

		//add by Xifeng, currentLevel will be modified to next level
	#if defined (_ANOTHER_CLOSED_APPROACH)
		if (addSequence(&pat, &currentLevel, reverseTable) == EQUAL_PROJECT_DB_SIZE) {
			clean_projcted_db(&pat, &nFreqCount);
			return;
		}
	#endif

		// scan sequence database once, find length-1 sequences
		children = make_projdb_from_projected_db(&pat, &nFreqCount); //todo change to children

		if (nFreqCount > 0) {
			for (i = 0; i < nFreqCount; i++) {
				if (children[i].m_nSup < gSUP) {
					//added by xifeng

					//n_total_mem -= (children[i].m_nPatLen * sizeof(int)); //!!!change by wenwen
					freemem((void**) &(children[i].m_pnPat));
					for (j = 0; j < children[i].m_nSup; j++) {
						if (children[i].m_pProjSeq[j].m_nProjCount > 1) {
							//n_total_mem -= (children[i].m_pProjSeq[j].m_nProjCount
							//		* sizeof(int*)); //!!!change by wenwen
							freemem((void**) &(children[i].m_pProjSeq[j].m_ppSeq));
						}
					}
					//n_total_mem -= (children[i].m_nMaxSup * sizeof(struct PROJ_SEQ)); //!!!change by wenwen
					freemem((void**) &(children[i].m_pProjSeq));

				} else{
					n_proj_db++;
				}
			}
		}
		// set iterator as the first element
		it = children.begin();
	}

	Task* project(PROJ_DB& cur_db){
		ClospanTask *t = new ClospanTask;
		t->currentLevel = currentLevel; // will be modify to next level in setChildren()
		t->pat = cur_db;
		return t;
	}

	virtual Task* get_next_child(){
		while(it != children.end() && it->m_nSup < gSUP && it->m_nMaxSup >= gSUP) {
#if defined (_ANOTHER_CLOSED_APPROACH)
			if (addSequence(&(*it), &currentLevel,
						reverseTable) != EQUAL_PROJECT_DB_SIZE){
				currentLevel = currentLevel->Parent;
			}

#endif
			it++;
		}
		if(it == children.end()){
			return 0;
		}
		else if(it != children.end() && it->m_nMaxSup >= gSUP){
			Task* newTask = this->project(*it);
			it++;
			return newTask;
		}
	}

	virtual bool pre_check(ostream& fout){
		return true;
	}
};




#endif /* CLOSPANTASK_H_ */

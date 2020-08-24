//########################################################################
//## Copyright 2019 Da Yan http://www.cs.uab.edu/yanda
//##
//## Licensed under the Apache License, Version 2.0 (the "License");
//## you may not use this file except in compliance with the License.
//## You may obtain a copy of the License at
//##
//## //http://www.apache.org/licenses/LICENSE-2.0
//##
//## Unless required by applicable law or agreed to in writing, software
//## distributed under the License is distributed on an "AS IS" BASIS,
//## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//## See the License for the specific language governing permissions and
//## limitations under the License.
//########################################################################

//########################################################################
//## Contributors
//## * Wenwen Qu
//## * Da Yan
//########################################################################

#ifndef TASK_H_
#define TASK_H_

#include <stack>
#include <vector>
#include "Global.h" // for task queue
#include "timetrack.h"

template <class PatternT, class ChildrenT, class TransDBT>
class Task{
public:

	// types
	typedef TransDBT TransDBType;
	typedef Task<PatternT, ChildrenT, TransDBT> TaskT;

	// members
	PatternT pat;
	ChildrenT children; // list of children (often is "map")
	TimeTracker* tt;

	Task(){tt = NULL;};
	virtual ~Task(){delete tt;};

	// UDFs
	virtual void setChildren(ChildrenT& children) = 0; // generate new (frequent) children patterns and their PDBs

	virtual Task* get_next_child() = 0; //get next item in "children", need to maintain an iterator in the task object

	virtual bool pre_check(ostream& fout)
	{
		return true;
	} // run before setChildren, may output pattern to fout
	// if returns false, will ignore further processing

	virtual bool needSplit() // whether to put new tasks got from get_next_child() into task queue, or process it by current thread
	{
		return true;
	}

	TransDBT& TransDB(){ // get the global transaction database
		return *(TransDBT*)transDB;
	}

	stack<TaskT*>& queue(){ // get the global task queue
		return *(stack<TaskT*> *)task_queue;
	}

	void run(ostream& fout){ // compute the current task, may generate new tasks to task queue
		if(!tt) {
			tt = new TimeTracker;
			tt->start();
		}
		if(!pre_check(fout)) return;
		//generate new candidate
		setChildren(children);
		//run new task;
		while(Task* t = get_next_child()){
			if(tt->escaped() > tauDB_time){//push all children into task queue to avoid multi-lock
				q_mtx.lock();
				queue().push(t);
				while(Task* t = get_next_child())
					queue().push(t);
				q_mtx.unlock();
				break;
			}
			else if(needSplit()){ //whether to run "t" in a thread or split it for parallel processing
				q_mtx.lock();
				queue().push(t);
				q_mtx.unlock();
			}
			else{
				t->run(fout);
				delete t;
			}
		}
		//188 448 9243
	}

};

#endif /* TASK_H_ */

/*
 * GastonWorker.h
 *
 *  Created on: 2021Äê3ÔÂ15ÈÕ
 *      Author: Administrator
 */

#ifndef APP_GASTON_GASTONWORKER_H_
#define APP_GASTON_GASTONWORKER_H_

#include "GastonTask.h"

class GastonWorker: public Worker<GastonTask> {
public:

	GastonWorker(const char *infile, const char *outfolder = "outputs"): Worker(infile, outfolder){
	}

	virtual void read(const char *filename){
		minfreq = minsup;
		FILE *input = fopen ( filename, "r" );
		database.read ( input );
		fclose ( input );
		database.edgecount ();
		database.reorder (); // by wenwen : clear infrequent label
	}
	virtual void setRoot(stack<GastonTask*>& task_queue){
		for ( int i = 0; i < database.nodelabels.size (); i++ ) {
		   if ( database.nodelabels[i].frequency >= minfreq &&
		         database.nodelabels[i].frequentedgelabels.size () ) {
			   GastonTask* rootTask = new GastonTask;
			   rootTask->pat.stage = 0;
			   rootTask->patternsize = 1;
			   GastonPattern& pat = rootTask->pat;
			   pat.initLegStatics ();
			   pat.graphstate.init ();
		       pat.createPath(i);
		       task_queue.push(rootTask);
		    }
		  }

	}



};



#endif /* APP_GASTON_GASTONWORKER_H_ */

/*
 * Utils.h
 *
 *  Created on: Apr 19, 2020
 *      Author: wen
 */



#include "Trans.h"
#include "Pattern.h"
#include "timetrack.h"
#include "Task.h"
#include "Worker.h"
#include "Global.h"

#include "omp.h"
#include <atomic>


#define _FIND_FREQUENT_SEQS
#define _CALC_I_NUM_OF_ITEMS
#define _ANOTHER_CLOSED_APPROACH
#define DISK_BASED
/////////////////
// Added by Xifeng
#if defined (_ANOTHER_CLOSED_APPROACH)
#endif

int gnCustCount=0;  // number of customers in database.
int fre_single_item = 0;

int gMAX_PAT_LEN=30;	// maximum length of a frequent sequence.
int gN_ITEMS=10000;		// # of items in the database.
int gSUP;		// requested support for frequent patterns.
//int* gnArrLargeCount=NULL;// # of large patterns of different lengths. //todo protected by mutex

int nMaxSeqLen = 0;

struct mem_map *pDatasetMemMap = NULL;//todo change to vector

atomic<int> n_proj_db(0);   // number of projected sequences processed.
//int n_max_mem = 0;		// maximum memory usage by projected databases.
//int n_total_mem = 0;	// total memory usage by projected databases.


//////////////////////////////////////////////////////////////////////
// functions.
//////////////////////////////////////////////////////////////////////
FILE *file_open(const char *f_name, const char *mode){

	FILE *f=fopen(f_name, mode);

	if (f==0){
		printf("Fail to open file %s with mode %s\n", f_name, mode);
		exit(-1);
	}
	return(f);
}

inline void* memalloc(size_t nSize){

	void *mem=0;
	if (nSize>0) {
		mem=malloc(nSize);
		if (mem==0){
			printf("Fail to allocate memory with size %i\n", nSize);
			exit(-1);
		}
	}
	return(mem);
}

inline void freemem(void** p){

  if (p!=0 && *p!=0){
    free (*p);
    *p=NULL;
  }
}

using namespace std;

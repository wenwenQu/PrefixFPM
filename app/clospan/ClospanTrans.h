/*
 * TreeSpanTrans.h
 *
 *  Created on: Apr 19, 2020
 *      Author: wen
 */

#ifndef CLOSPANTRANS_H_
#define CLOSPANTRANS_H_


#include "LinuxMemMap.h"



struct COUNTER{
	int s_id;	    // last sequence updated this counter.
	int count;	  // accumulated count.
};


//---------------------------
class Seq: public Trans{
public:
	vector<int> seq;
};

//project sequence
class PROJ_SEQ: public ProjTrans{
public:
	int m_nProjCount;  // number of projected sequences in m_ppSeq.
	int **m_ppSeq;     // pointer to first element if each projection.
};







////

#endif /* CLOSPANTRANS_H_ */

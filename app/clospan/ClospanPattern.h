/*
 * TreeSpanPattern.h
 *
 *  Created on: Apr 19, 2020
 *      Author: wen
 */

#ifndef CLOSPANPATTERN_H_
#define CLOSPANPATTERN_H_
#include "ClospanTrans.h"


class ClospanPattern: public Pattern {
public:
	int  m_nPatLen;    // pattern length of projected database.
	int *m_pnPat;      // pattern of projected database.

	int  m_nMaxSup;    // maximum support that this projection can have.
	int  m_nSup;       // actual support found in dataset.
	int  m_nVer;       // last sequence id that supported this pattern.
	int  m_nSeqSize;   // size of projected sequnce in m_pProjSeq[0].
	struct PROJ_SEQ *m_pProjSeq;  // projected sequences.

	//defined( _CALC_I_NUM_OF_ITEMS )
	bool ItemIsIntra;
	int  Item;
	long NumOfItems; // Added by Ramin.

	virtual void print(ostream& fout){
	}

};

typedef ClospanPattern PROJ_DB;

vector<PROJ_DB> make_projdb_from_projected_db(PROJ_DB *pDB,
		int *pnFreqCount) {
	int **buf_idx = (int**) memalloc(sizeof(int*) * nMaxSeqLen);
	memset(buf_idx, 0, sizeof(int*) * nMaxSeqLen);
	vector<PROJ_DB> proj_db;//!!! change by wenwen

	int i = 0, j = 0, k = 0, l = 0, m = 0, nSize = 0, nCount = 0, nSeqLen = 0,
			nProjCnt = 0;
	int *b = 0, *d = 0, *dataset = 0;
	bool bIntra = false, bIndx = false;
	PROJ_DB *tempDB = NULL;
	PROJ_SEQ *tempSeq = NULL;
	COUNTER *pCnter = 0;

	COUNTER* inter = (struct COUNTER*) memalloc(sizeof(struct COUNTER) * gN_ITEMS);
	COUNTER* intra = (struct COUNTER*) memalloc(sizeof(struct COUNTER) * gN_ITEMS);
	int* inter_freq_idx = (int*) memalloc(sizeof(int) * gN_ITEMS);
	int* intra_freq_idx = (int*) memalloc(sizeof(int) * gN_ITEMS);

	//defined( _CALC_I_NUM_OF_ITEMS )
	int *lastAddr = &total_item;

	*pnFreqCount = 0;	// number of frequent items
	//PrintProjDBs(pDB, 1);

	// scan database once, find inter- and intra- element frequent items
	memset(intra, 0, sizeof(struct COUNTER) * gN_ITEMS);
	memset(inter, 0, sizeof(struct COUNTER) * gN_ITEMS);

	for (nCount = 0; nCount < pDB->m_nSup;) {
		nCount++;
		nProjCnt = pDB->m_pProjSeq[nCount - 1].m_nProjCount;
		for (i = 0; i < nProjCnt; i++) {
			if (nProjCnt == 1)
				dataset = (int*) pDB->m_pProjSeq[nCount - 1].m_ppSeq;
			else
				dataset = pDB->m_pProjSeq[nCount - 1].m_ppSeq[i];

			// counting intra-element items.
			for (; *dataset != -1; dataset++) {
				// eat up consecutive identical numbers.
				while (dataset[0] == dataset[1])
					dataset++;
				pCnter = intra + (*dataset);
				if (pCnter->s_id < nCount) {
					pCnter->count++;
					pCnter->s_id = nCount;
				}
			}
			// for inter we only need to count the longest instance
			// of a projected sequence. ie. the first one (i==0).
			if (i != 0)
				continue;
			for (dataset++; *dataset != -2; dataset++) {
				// counting inter-element items.
				for (; *dataset != -1; dataset++) {
					// eat up consecutive identical numbers.
					while (dataset[0] == dataset[1])
						dataset++;
					pCnter = inter + (*dataset);
					if (pCnter->s_id < nCount) {
						pCnter->count++;
						pCnter->s_id = nCount;
					}
				}
			}
		}
	}

	for (j = k = i = 0; i < gN_ITEMS; i++) {
		if (intra[i].count >= gSUP)
			j++;
		if (inter[i].count >= gSUP)
			k++;
	}


	if ((j + k) > 0) {
		*pnFreqCount = (j + k);
		nSize = (*pnFreqCount * sizeof(PROJ_DB));
		proj_db.resize(*pnFreqCount);
		memset(inter_freq_idx, -1, sizeof(int) * gN_ITEMS);
		memset(intra_freq_idx, -1, sizeof(int) * gN_ITEMS);
		for (j = sizeof(int) * (pDB->m_nPatLen + 1), nCount = i = 0;
				i < gN_ITEMS; i++) {
			if (intra[i].count >= gSUP) {
				proj_db[nCount].m_nPatLen = (pDB->m_nPatLen + 1);
				proj_db[nCount].m_pnPat = (int*) memalloc(j);
				if (pDB->m_nPatLen == 1) {
					proj_db[nCount].m_pnPat[0] = (long) pDB->m_pnPat;
				} else {
					memcpy(proj_db[nCount].m_pnPat, pDB->m_pnPat,
							j - sizeof(int));
				}
				proj_db[nCount].m_pnPat[pDB->m_nPatLen] = i;

				proj_db[nCount].m_nMaxSup = intra[i].count;
				nSize = intra[i].count * sizeof(struct PROJ_SEQ);
				proj_db[nCount].m_pProjSeq = (struct PROJ_SEQ*) memalloc(nSize);
				memset(proj_db[nCount].m_pProjSeq, 0, nSize);

				proj_db[nCount].m_nVer = -1;
				proj_db[nCount].m_nSup = 0;
				//_CALC_I_NUM_OF_ITEMS
				proj_db[nCount].NumOfItems = 0;
				proj_db[nCount].ItemIsIntra = true;
				proj_db[nCount].Item = i;

				intra_freq_idx[i] = nCount;
				nCount++;

			}
			if (inter[i].count >= gSUP) {
				proj_db[nCount].m_nPatLen = (pDB->m_nPatLen + 2);
				proj_db[nCount].m_pnPat = (int*) memalloc(sizeof(int) + j);
				if (pDB->m_nPatLen == 1) {
					proj_db[nCount].m_pnPat[0] = (long) pDB->m_pnPat;
				} else {
					memcpy(proj_db[nCount].m_pnPat, pDB->m_pnPat,
							j - sizeof(int));
				}
				proj_db[nCount].m_pnPat[pDB->m_nPatLen] = -1;
				proj_db[nCount].m_pnPat[pDB->m_nPatLen + 1] = i;

				proj_db[nCount].m_nMaxSup = inter[i].count;
				nSize = inter[i].count * sizeof(struct PROJ_SEQ); //!!!change by wenwen
				proj_db[nCount].m_pProjSeq = (struct PROJ_SEQ*) memalloc(nSize);
				memset(proj_db[nCount].m_pProjSeq, 0, nSize);

				proj_db[nCount].m_nVer = -1;
				proj_db[nCount].m_nSup = 0;
				//_CALC_I_NUM_OF_ITEMS
				proj_db[nCount].NumOfItems = 0;
				proj_db[nCount].ItemIsIntra = false;
				proj_db[nCount].Item = i;

				inter_freq_idx[i] = nCount;
				nCount++;
			}
		}


		// scan database again, do projection
		for (nCount = 0; nCount < pDB->m_nSup;) {
			nCount++;
			nProjCnt = pDB->m_pProjSeq[nCount - 1].m_nProjCount;
			for (i = 0; i < nProjCnt; i++) {
				//DISK_BASED
				if (nProjCnt == 1)
					dataset = (int*) pDB->m_pProjSeq[nCount - 1].m_ppSeq;
				else
					dataset = pDB->m_pProjSeq[nCount - 1].m_ppSeq[i];


				// counting intra-element items.
				for (; *dataset >= 0; dataset++) {
					// eat up consecutive identical numbers.
					while (dataset[0] == dataset[1])
						dataset++;

					j = intra_freq_idx[*dataset];
					if (dataset[2] == -2 || j < 0
							|| proj_db[j].m_nVer >= nCount)
						continue;

					tempDB = &proj_db[j];
					tempSeq = tempDB->m_pProjSeq + tempDB->m_nSup;
					tempDB->m_nVer = nCount;

					//( _CALC_I_NUM_OF_ITEMS )
					(*tempDB).NumOfItems += (lastAddr - dataset);

					for (buf_idx[0] = (dataset + 1), l = 1, j = i;
							j < nProjCnt; j++) {
						if (j == i)
							d = dataset + 1;
						else
							d = pDB->m_pProjSeq[nCount - 1].m_ppSeq[j];
						while (*d != -1 && d[2] != -2 && *d != *dataset)
							d++;
						if (d[2] != -2 && *d == *dataset && d[1] != -1)
							buf_idx[l++] = (d + 1);
					}
					//DISK_BASED
					tempDB->m_nSup++;
					tempSeq->m_nProjCount = l;
					if (l == 1) {
						tempSeq->m_ppSeq = (int**) buf_idx[0];
					} else {
						nSize = sizeof(int*) * l;
						tempSeq->m_ppSeq = (int**) memalloc(nSize);
						memcpy(tempSeq->m_ppSeq, buf_idx, nSize);
					}
				} // end for counting intra-element items.

				// for inter we only need to work with the longest instance
				// of a projected sequence. ie. the first one (i==0).
				if (i != 0)
					continue;
				for (dataset++; *dataset != -2; dataset++) {
					// counting inter-element items.
					for (; *dataset != -1; dataset++) {
						// eat up consecutive identical numbers.
						while (dataset[0] == dataset[1])
							dataset++;
						j = inter_freq_idx[*dataset];
						if (dataset[2] == -2 || j < 0
								|| proj_db[j].m_nVer >= nCount)
							continue;

						tempDB = &proj_db[j];
						tempSeq = tempDB->m_pProjSeq + tempDB->m_nSup;
						tempDB->m_nVer = nCount;

						//( _CALC_I_NUM_OF_ITEMS )
						(*tempDB).NumOfItems += (lastAddr - dataset);

						for (d = dataset + 1, buf_idx[0] = (d), l = 1;
								d[2] != -2; d++) {
							if (*d == *dataset && d[1] != -1)
								buf_idx[l++] = (d + 1);
						}
						//DISK_BASED
						tempDB->m_nSup++;
						tempSeq->m_nProjCount = l;
						if (l == 1) {
							tempSeq->m_ppSeq = (int**) buf_idx[0];
						} else {
							nSize = sizeof(int*) * l; 
							tempSeq->m_ppSeq = (int**) memalloc(nSize);
							memcpy(tempSeq->m_ppSeq, buf_idx, nSize);
						}
					}
				} // end for counting inter-element items.
			} // end of all projection instances of a sequence.
		} // end of all sequences.
	} // enf if number of projections is greater than 0.


	if (pDB->m_nPatLen > 1) {
		freemem((void**) &(pDB->m_pnPat));
	}
	for (i = 0; i < pDB->m_nSup; i++) {
		if (pDB->m_pProjSeq[i].m_nProjCount > 1) {
			//n_total_mem -= (pDB->m_pProjSeq[i].m_nProjCount * sizeof(int*)); //!!!change by wenwen
			freemem((void**) &(pDB->m_pProjSeq[i].m_ppSeq));
		}
	}
	freemem((void**) &(pDB->m_pProjSeq));
	freemem((void**) &inter);
	freemem((void**) &intra);
	freemem((void**) &inter_freq_idx);
	freemem((void**) &intra_freq_idx);
	freemem((void**) &buf_idx);
	// PrintProjDBs(proj_db, *pnFreqCount);
	return proj_db;
}

//this part is copied from make_projdb_from_projected_db's tail part.
void clean_projcted_db(const PROJ_DB *pDB, int *pnFreqCount) {
	int i;
	if (pDB->m_nPatLen > 1) {
		freemem((void**) &(pDB->m_pnPat));
	}
	for (i = 0; i < pDB->m_nSup; i++) {
		if (pDB->m_pProjSeq[i].m_nProjCount > 1) {
			freemem((void**) &(pDB->m_pProjSeq[i].m_ppSeq));
		}
	}
	freemem((void**) &(pDB->m_pProjSeq));
}

//////////////////////////////////////////////////////////////////////
// Local functions.
//////////////////////////////////////////////////////////////////////
void PrintProjDBs(const PROJ_DB *proj_db, const int nCount) {
	int i = 0, j = 0, k = 0, l = 0, nProjCnt = 0, *dataset = 0;

	printf("\nProjected databases:\n");
	for (i = 0; i < nCount; i++) {
		printf("Proj. DB. for (");
		if (proj_db[i].m_nPatLen == 1)
			printf("%l", long(proj_db[i].m_pnPat));
		else
			for (j = 0; j < proj_db[i].m_nPatLen; j++) {
				if (proj_db[i].m_pnPat[j] == -1)
					printf(")");
				else if (j > 0 && proj_db[i].m_pnPat[j - 1] == -1)
					printf(" (%d", proj_db[i].m_pnPat[j]);
				else if (j > 0)
					printf(" %d", proj_db[i].m_pnPat[j]);
				else
					printf("%d", proj_db[i].m_pnPat[j]);
			}
		printf("\n ");

		//( _CALC_I_NUM_OF_ITEMS )
		printf(
				"NumOfItems = %d m_nSupport= %d  maxSupport= %d\n",
				proj_db[i].NumOfItems, proj_db[i].m_nSup, proj_db[i].m_nMaxSup);

		return;

		for (j = 0; j < proj_db[i].m_nSup; j++) {
			nProjCnt = proj_db[i].m_pProjSeq[j].m_nProjCount;
			for (k = 0; k < nProjCnt; k++) {
				//DISK_BASED
				if (nProjCnt == 1)
					dataset = (int*) proj_db[i].m_pProjSeq[j].m_ppSeq;
				else
					dataset = proj_db[i].m_pProjSeq[j].m_ppSeq[k];
				for (l = 0; dataset[l] != -2; l++) {
					if (dataset[l] == -1)
						printf(")");
					else if (l > 0 && dataset[l - 1] == -1)
						printf(" (%d", dataset[l]);
					else if (l > 0)
						printf(" %d", dataset[l]);
					else
						printf("%d", dataset[l]);
				} // end of an instance for a projected sequence.
				printf("\n ");
			} // end of all instances for a projected sequence.
			printf("\n ");
		} // end of all projected sequences.
	}
}


#endif /* CLOSPANPATTERN_H_ */

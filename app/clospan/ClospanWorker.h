/*
 * TreeSpanWorker.h
 *
 *  Created on: Apr 21, 2020
 *      Author: wen
 */

#ifndef CLOSPAN_CLOSPANWORKER_H_
#define CLOSPAN_CLOSPANWORKER_H_
#include <strstream>
#include "ClospanTask.h"
#include <set>

class ClospanWorker: public Worker<ClospanTask> {
public:
	int trans_cnt; // to used for adding IDs to transactions when loading data
    char* outfile; // where the close patterns output

	ClospanWorker(const char *infile, const char *outfolder = "outputs") :
			Worker(infile, outfolder), trans_cnt(0){
        outfile = (char*)malloc(strlen(outfolder)+20);
        strcpy(outfile, outfolder);
        strcat(outfile, "/maxCloseFile.txt");
	}


	//////////////////////////////////////////////////////////////////////
	// functions.
	//////////////////////////////////////////////////////////////////////
	void read(const char *filename) {
		pDatasetMemMap = CreateMemMap(filename, 1536 * 1024 * 1024);
		int *dataset = (int*) GetStartOfMap(pDatasetMemMap);
		int *lastAddr = (int*) GetLastAddrOfMap(pDatasetMemMap);
	}

	void make_projdb_from_org_dataset(const double dSupport,
			int *pnFreqCount, PROJ_DB*& proj_db) {
		int** buf_idx;
		int i = 0, j = 0, nSize = 0, nCount = 0, nPatLen = 0,
				nSeqLen = 0;
		int *d = 0, *dataset = (int*) GetStartOfMap(pDatasetMemMap);
		int *lastAddr = (int*) GetLastAddrOfMap(pDatasetMemMap);
		PROJ_DB *tempDB = NULL;
		PROJ_SEQ *tempSeq = NULL;
		COUNTER *pCnter = 0;

		*pnFreqCount = 0;

		COUNTER* inter = (struct COUNTER*) memalloc(sizeof(struct COUNTER) * gN_ITEMS);
		int* inter_freq_idx = (int*) memalloc(sizeof(int) * gN_ITEMS);

		memset(inter_freq_idx, 0, sizeof(int) * gN_ITEMS);
		memset(inter, 0, sizeof(struct COUNTER) * gN_ITEMS);

		// Scaning DB to count the frequent items.
		for (nCount = 0; dataset < lastAddr; dataset++) {
			nCount++;
			nPatLen = nSeqLen = 0;
			for (; *dataset != -2; dataset++) {
				for (; *dataset != -1; dataset++) {
					// eat up consecutive identical numbers.
					while (dataset[0] == dataset[1])
						dataset++;
					nPatLen++;
					nSeqLen++;
					pCnter = inter + (*dataset);

					if (pCnter->s_id < nCount) {
						pCnter->s_id = nCount;
						pCnter->count++;
						if (dataset[2] != -2)
							inter_freq_idx[*dataset]++;
					}
					nSeqLen++;
				}
				nSeqLen++;
			}
			if (gMAX_PAT_LEN < nPatLen)
				gMAX_PAT_LEN = nPatLen;
			if (nMaxSeqLen < nSeqLen)
				nMaxSeqLen = nSeqLen;
		}

		gnCustCount = nCount;
		gSUP = dSupport;
		gMAX_PAT_LEN += 2; // since we don't use the first and last index.

		buf_idx = (int**) memalloc(sizeof(int) * nMaxSeqLen);
		memset(buf_idx, 0, sizeof(int) * nMaxSeqLen);


	///////////////////////////////
	// Changed by Ramin
		for (i = 0; i < gN_ITEMS; i++) {
			pCnter = inter + i;
			if (pCnter->count >= gSUP) {
				fre_single_item++;//!!! change by wenwen
			}
		}

	///////////////////////////////

		// If there are 1 item frequent seqs.
		if (fre_single_item > 0) {
			*pnFreqCount = fre_single_item;
			nSize = (fre_single_item * sizeof(PROJ_DB));
			proj_db = (PROJ_DB*) memalloc(nSize);

			for (nCount = i = 0; i < gN_ITEMS; i++) {
				if (inter[i].count >= gSUP) {
					proj_db[nCount].m_nPatLen = 1;
					proj_db[nCount].m_pnPat = (int*) i;
					proj_db[nCount].m_nMaxSup = (*(inter + i)).count;
					proj_db[nCount].m_pProjSeq = (struct PROJ_SEQ*) memalloc(nSize);
					memset(proj_db[nCount].m_pProjSeq, 0, nSize);

					proj_db[nCount].m_nVer = -1;
					proj_db[nCount].m_nSup = 0;

					proj_db[nCount].NumOfItems = 0;
					proj_db[nCount].ItemIsIntra = false;
					proj_db[nCount].Item = i;

					inter_freq_idx[i] = nCount;
					nCount++;
				} else
					inter_freq_idx[i] = -1;
			}

			// scan database again, do projection
			dataset = (int*) GetStartOfMap(pDatasetMemMap);

			for (nCount = 0; dataset < lastAddr; dataset++) {
				nCount++;
				for (; *dataset != -2; dataset++) {
					for (; *dataset != -1; dataset++) {
						// eat up consecutive identical numbers.
						while (dataset[0] == dataset[1])
							dataset++;
						i = inter_freq_idx[*dataset];
						// If this is the last item in Seq, or is not frequent, or an instance of this item has been seen in this seq before.
						if (dataset[2] == -2 || i < 0
								|| proj_db[i].m_nVer >= nCount){
							continue;
						}
						// Pointer to Proj_DB for this item.
						tempDB = proj_db + i;
						// Pointer to the next available Seq in this DB.
						tempSeq = tempDB->m_pProjSeq + tempDB->m_nSup;
						// Last sequence contributed to this DB.
						tempDB->m_nVer = nCount;
						//( _CALC_I_NUM_OF_ITEMS )
						(*tempDB).NumOfItems += (lastAddr - dataset);

						//DISK_BASED
						tempDB->m_nSup++;
						for (d = dataset + 1, buf_idx[0] = (d), j = 1;
								d[2] != -2; d++) {
							if (*d == *dataset && d[1] != -1)
								buf_idx[j++] = (d + 1);
						}
						tempSeq->m_nProjCount = j;
						if (j == 1) {
							tempSeq->m_ppSeq = (int**) buf_idx[0];
						} else {
							nSize = sizeof(int*) * j;
							tempSeq->m_ppSeq = (int**) memalloc(nSize);
							memcpy(tempSeq->m_ppSeq, buf_idx, nSize);
						}
					}
				}
			}
		}
		freemem((void**) &inter_freq_idx);
		freemem((void**) &buf_idx);
	}

	void InitApp(const char *filename, const double dSupport,
			const int nItemCount) {
		gN_ITEMS = (nItemCount + 1); // because it is zero based.

		//Added by Xifeng
		//defined (_ANOTHER_CLOSED_APPROACH)
		root = new TreeNode(-1, 0);
		root->ItemsetNumber = 0;
		root->Parent = NULL;

		reverseTable = new ReverseHashTable;

		/////////////////

		//DISK_BASED
		printf("clospan: A sequential pattern mining algorithm (pseudo projection).\n");
		printf("Implemented by Behzad Mortazavi-Asl, in IDBL, SFU\n");
		printf("All rights reserved!\n");
		printf("# of items: %d\n", nItemCount);
		printf("Support threshold: %.3f%%\n", dSupport);
		printf( "-----------------------\n");
	}

	virtual void setRoot(stack<ClospanTask*> &task_queue) {
		InitApp(file_name, minsup, total_item);

		TreeNode* currentLevel = root;

		int i = 0, j = 0, nFreqCount = 0;
		// scan sequence database once, find length-1 sequences
		PROJ_DB *proj_db;
		make_projdb_from_org_dataset(minsup,
				&fre_single_item,proj_db);

		if (fre_single_item > 0) {
			nFreqCount = fre_single_item;
			for (i = 0; i < nFreqCount; i++) {
				if (proj_db[i].m_nSup < gSUP) {
	//added by xifeng
					//(_ANOTHER_CLOSED_APPROACH)
					if (addSequence(&proj_db[i], &currentLevel,
							reverseTable) != EQUAL_PROJECT_DB_SIZE)
						currentLevel = currentLevel->Parent;

					for (j = 0; j < proj_db[i].m_nSup; j++) {

						if (proj_db[i].m_pProjSeq[j].m_nProjCount > 1) {
							freemem((void**) &(proj_db[i].m_pProjSeq[j].m_ppSeq));
						}
					}

					freemem((void**) &(proj_db[i].m_pProjSeq));
				}
			}

			for (i = 0; i < nFreqCount; i++) {
				if (proj_db[i].m_nSup >= gSUP) {
					ClospanTask *t = new ClospanTask;
					t->currentLevel = root; // will be modify to next level in setChildren()
					t->pat = *(proj_db+i);
					task_queue.push(t);
				}
			}
			freemem((void**) &proj_db);
		}
	}

	virtual void finish(){
		//added by Xifeng
		//(_ANOTHER_CLOSED_APPROACH)
		NodeVector::iterator it, endit;
		for (it = root->Children->begin(), endit = root->Children->end();
				it != endit; it++) {
			//if ((*it)->Item==6010)
			//printf("GAGAGA\n");
			closed_maxPruning((*it), root);
		}
		FILE *closed_maxFile = NULL;
		closed_maxFile = file_open(outfile, "w");

		fprintf(closed_maxFile, "CLOSED\n");

		for (it = root->Children->begin(), endit = root->Children->end();
				it != endit; it++) {
			if ((*it)->Parent == root)
				(*it)->Print("(", closed_maxFile);
		}

		CloseMemMap(&pDatasetMemMap);
	}
};

#endif /* PREFIXTREESPAN_CLOSPANWORKER_H_ */

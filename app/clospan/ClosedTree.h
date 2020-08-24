#if !defined( _CLOSED_TREE_H__ )
#define _CLOSED_TREE_H__

#include <stdio.h> 
#include <stdlib.h>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <mutex>
#ifdef _MSC_VER
#include <hash_map>
#else
#include <ext/hash_map>
using namespace __gnu_cxx;
#endif

#define EQUAL_PROJECT_DB_SIZE 0
#define INEQUAL_PROJECT_DB_SIZE 1


#include "ClospanPattern.h"

using namespace std;

class TreeNode;
mutex rht_mutex;
mutex add_seq_mtx;

typedef hash_map<int, TreeNode *, std::hash<int>, equal_to<int> > NodeHashTable;
typedef hash_multimap<int, TreeNode *, std::hash<int>, equal_to<int> > ReverseNodeHashTable;
typedef hash_multimap<int, TreeNode *, std::hash<int>, equal_to<int> > ReverseHashTable;
typedef pair<ReverseNodeHashTable::iterator, ReverseNodeHashTable::iterator> ReverseMap;

typedef hash_multimap<int, TreeNode *, std::hash<int>, equal_to<int> > ReverseNodeHashTable;
typedef pair<ReverseNodeHashTable::iterator, ReverseNodeHashTable::iterator> ReverseNodeMap;


typedef vector<TreeNode *> NodeVector;
typedef vector<NodeVector *> NodeVectorTable;

class ConcurrentReverseHashTable{
public:
	ReverseHashTable rht;
	mutex rht_lock;

	ReverseMap equal_range(int idx){
		rht_mutex.lock();
		const ReverseMap& tmp = rht.equal_range(idx);//todo need lock here
		rht_mutex.unlock();
		return tmp;
	}

	void insert(const ReverseHashTable::value_type& val){
		rht_mutex.lock();
		rht.insert(val);
		rht_mutex.unlock();
	}
};

class ItemSet
{
	public:
		int ItemArray[64];
		int Count;

		ItemSet() { Count=0; };
		inline bool IsSubsetOf( ItemSet * anItemSet )
		{
			int j;
			int OtherCount=anItemSet->Count;

			if( Count> OtherCount )
				return false;

			j = 0;
			for( int i=0; i<Count; i++ )
			{
				while( ItemArray[i] != anItemSet->ItemArray[j] )
				{
					j++;
					if( (OtherCount-j) < (Count-i) )
						return false;
				}
			}

			return true;
		}
		int size() {  return Count; };
		inline void	reset() { Count=0; };

		~ItemSet() { };
};

class TreeNode
{
	public:
		int		Item;
		bool	ItemIsIntra;
		int		ItemsetNumber;
		int		Items;
		int		Support;
		long	NumOfItems;

		bool	closed;
		bool	max;
		
		NodeVector		*Children;
		TreeNode		*Parent;

		TreeNode( int anItem = -1, bool IsIntra = false, int Sup = 0, TreeNode * aParent = NULL )
		{

			Children = new NodeVector();

			Parent = aParent;
			ItemsetNumber = 0;
			Items=0;
			Item = anItem;
			ItemIsIntra = IsIntra;
			Support = Sup;
			closed=true;
			max=true;
			NumOfItems=0;
		}
		TreeNode( TreeNode *treeNode)
		{

			Children =treeNode->Children;

			Parent = treeNode->Parent;
			ItemsetNumber = treeNode->ItemsetNumber;
			Items= treeNode->Items;
			Item = treeNode->Item;
			ItemIsIntra = treeNode->ItemIsIntra;
			Support = treeNode->Support;
			closed=true;
			max=true;
			NumOfItems=0;
		}

		inline void		DelChild( TreeNode * Child, NodeVector::iterator it)
		{

			if( Children == NULL )
				return;
			(*Children).erase(it);
		}
		inline void		DelChild( int anItem, bool Intra, NodeVector::iterator it)
		{
			TreeNode *theNode=NULL;

			theNode=FindChild(anItem, Intra);
			if (theNode != NULL) {
				(*Children).erase(it);
				delete theNode;
			}
		}
		inline TreeNode *FindChild( int anItem, bool Intra )
		{
			TreeNode ** Res;
			TreeNode * tmp = new TreeNode( anItem, Intra );

			Res = (TreeNode **) bsearch( &tmp, &(*Children)[0], (*Children).size(), sizeof( TreeNode *), (int (*)(const void*, const void*))TreeNodeCompare );

			delete tmp;

			if( Res )
				return (*Res);
			else
				return NULL;
		}

		TreeNode * AddChild( int anItem, bool Intra, int Sup =0)
		{
			TreeNode *Result = NULL;
			TreeNode *Child  = NULL;

			Result = FindChild( anItem, Intra);
			if( Result == NULL ) {
				Child= new TreeNode(anItem, Intra, Sup);
				Result = Child;
				(*Children).push_back( Child );
				// To keep the children vector sorted.
				inplace_merge( (*Children).begin(), (*Children).end()-1, (*Children).end(), TreeNodeLess );
				Child->Parent = this;
				if( Child->ItemIsIntra ) {
					Child->ItemsetNumber = ItemsetNumber;
					Child->Items = Items + 1;
				}
				else {
					Child->ItemsetNumber = ItemsetNumber + 1;
					Child->Items = Items + 1;
				}
			} else {
				if( Result->Support < Sup ) {
					printf("Item: %d %d %d\n", anItem, Result->Support, Sup);
					printf("ParentItem: %d %d\n", Item, Support);
					Result->Support = Sup;
				}
			}
			return Result;
		}
		TreeNode * AddChild(TreeNode * Child )
		{
			TreeNode * Result = NULL;


			Result = FindChild( Child->Item, Child->ItemIsIntra);
			if( Result == NULL ) {
				Result = Child;
				(*Children).push_back( Child );
				// To keep the children vector sorted.
				//inplace_merge( (*Children).begin(), (*Children).end()-1, (*Children).end(), TreeNodeLess );
				Child->Parent = this;
				if( Child->ItemIsIntra ) {
					Child->ItemsetNumber = ItemsetNumber;
					Child->Items = Items + 1;
				}
				else {
					Child->ItemsetNumber = ItemsetNumber + 1;
					Child->Items = Items + 1;
				}
			} else {
				if( Child->Support > Result->Support )
						Result->Support = Child->Support;
				delete Child;

			}
			return Result;
		}
		inline TreeNode *AddChildWithoutChecking(int anItem, bool Intra, int Sup=0)
		{
			TreeNode *Child;

			Child= new TreeNode(anItem, Intra, Sup);

#pragma omp critical
			(*Children).push_back( Child );
			// To keep the children vector sorted.
#pragma omp critical
			inplace_merge( (*Children).begin(), (*Children).end()-1, (*Children).end(), TreeNodeLess );
			Child->Parent = this;
			if( Child->ItemIsIntra ) {
				Child->ItemsetNumber = ItemsetNumber;
				Child->Items = Items + 1;
			}
			else {
				Child->ItemsetNumber = ItemsetNumber + 1;
				Child->Items = Items + 1;
			}
			return Child;
		}
		inline void	SetProjDBSize(long size)
		{
			NumOfItems = size;
		}

		inline int NumOfChildren()
		{
			if( Children==NULL )
				return 0;
			else
				return (*Children).size();
		}
		inline int MaxChildSupport()
		{
			int maxChildSup = 0;
			NodeVector::iterator it;

			if( Children==NULL )
				return 0;

			for( it = (*Children).begin(); it != (*Children).end(); it++ )
				if( ((*it))->Support > maxChildSup )
					maxChildSup = (*it)->Support;

			return maxChildSup;
		}
		bool LastItemOfSequence()
		{
			if( Children==NULL )
				return true;

			if( MaxChildSupport() < Support )
				return true;

			return false;
		}
		bool isRoot()
		{
			if (Parent == NULL )
				return true;
			else
				return false;
		}
		void Print( char* PrefixString = "", FILE * aFile = stdout )
		{
			NodeVector::iterator it;
			char NewPrefixString[256];

			if(ItemIsIntra) {
				sprintf(NewPrefixString, "%s %d", PrefixString, Item);
			}
			else {
				sprintf(NewPrefixString, "%s)(%d", PrefixString, Item);
			}

			if( Children != NULL && NumOfChildren() > 0) {
				for( it = (*Children).begin(); it != (*Children).end(); it++ ) {
					if ((*it)->Parent == this)  // if not equal, its subtree must be totally absorbed
						(*it)->Print(NewPrefixString, aFile );
				}
			}


			if (closed) {
					//closed_maxFreq[Items] ++;
					//if (this->Item == 9998 )
							//printf("HEHE: 9970-%d\n", this->Support);
					fprintf( aFile, "<%s)>  Support = %d  \n", NewPrefixString+2, Support );
					//fprintf( aFile, "<%s)>\n", NewPrefixString+2);
				}

		}

		static bool TreeNodeLess( TreeNode * a, TreeNode * b )
				{
					if( (*a).ItemIsIntra == (*b).ItemIsIntra )
						return (*a).Item < (*b).Item;
					else if( (*a).ItemIsIntra && !(*b).ItemIsIntra )
						return true;
					else
						return false;
				}

		static int TreeNodeCompare( TreeNode ** a, TreeNode ** b )
				{
					if( (*(*a)).ItemIsIntra == (*(*b)).ItemIsIntra )
						if( (*(*a)).Item == (*(*b)).Item )
							return 0;
						else if( (*(*a)).Item < (*(*b)).Item )
							return -1;
						else
							return 1;
					else if( (*(*a)).ItemIsIntra && !(*(*b)).ItemIsIntra )
						return -1;
					else
						return 1;
				}

		~TreeNode()
		{
			NodeVector::iterator it;

			if( Children )
				for( it = (*Children).begin(); it != (*Children).end(); it++)
					delete *it;

			delete Children;

		}
};




		inline TreeNode * GetLastItemSet(TreeNode * treeNode, ItemSet *itemSet)
		{
			int *itemArray=itemSet->ItemArray;
			int i=0;

			itemSet->reset();
			do {
				itemArray[i++]=treeNode->Item;
				if (!treeNode->ItemIsIntra)
					break;
				else
					treeNode=treeNode->Parent;
			} while (treeNode->Items > 0);

			itemSet->Count=i;
			return (treeNode->Parent);
		}

		inline int isContained_DBSizeEqual(PROJ_DB *pDB, TreeNode *currentLevel, TreeNode *candidate)
		{
			ItemSet supSet;
			ItemSet subSet;

			int		myItem = pDB->Item;
			bool	myItemIsIntra = pDB->ItemIsIntra;
			int		dir;
			bool	isContained;


			TreeNode *supNode;
			TreeNode *subNode;

			if ((myItem != candidate->Item) || (candidate->Items == (currentLevel->Items+1)))
				return 0;

			if ( candidate->Items > (currentLevel->Items + 1))
				dir=1;
			else
				dir=-1;

			if (candidate->ItemIsIntra) {
				if (myItemIsIntra) {
					candidate=candidate->Parent;
				}
				else {
					//in this case, supNode must be candidate, subNode must be currentLevel
					while (candidate->ItemIsIntra && candidate->Items >0)
						candidate = candidate ->Parent;

					candidate=candidate->Parent;
					if ((candidate->Items < currentLevel->Items) || (candidate->ItemsetNumber < currentLevel->ItemsetNumber))
						return 0;

				}
			}
			else {
				if (myItemIsIntra) {
					//in this case, supNode must be currentLevel, subNode must be candidate
					while (currentLevel->ItemIsIntra && currentLevel->Items >0)
						currentLevel = currentLevel ->Parent;

					currentLevel=currentLevel->Parent;
					if ((candidate->Items > currentLevel->Items) || (candidate->ItemsetNumber > currentLevel->ItemsetNumber))
						return 0;


				}
				else {
					candidate=candidate->Parent;
				}
			}


			if (currentLevel->Items == 0) {
				return 1;
			}

			if (candidate->Items == 0) {
				return -1;
			}

			if ( dir == 1) {
				supNode=candidate;
				subNode=currentLevel;
			}
			else {
				supNode=currentLevel;
				subNode=candidate;
			}

			isContained=false;
			supNode=GetLastItemSet(supNode, &supSet);
			subNode=GetLastItemSet(subNode, &subSet);

			while (1) {
				if (subSet.IsSubsetOf(&supSet)) {
					if (subNode->Items == 0) {
						isContained=true;
						break;
					}
					if ((supNode->Items >= subNode->Items) && (supNode->ItemsetNumber >= subNode->ItemsetNumber)) {
						supNode=GetLastItemSet(supNode, &supSet);
						subNode=GetLastItemSet(subNode, &subSet);
					}
					else
						break;
				}
				else {
					if (supNode->Items == 0) // unnecessary to check any more.
						break;

					if ((supNode->Items >= subNode->Items) && (supNode->ItemsetNumber >= subNode->ItemsetNumber)) {
						supNode=GetLastItemSet(supNode, &supSet);
					}
					else
						break;
				}

			}
			if (isContained)
				return dir;
			else
				return 0;


		}

		void updateNodeInfo(TreeNode *treeNode, int extraItems, int extraItemsetNumber)
		{
			NodeVector::iterator it, endit;

			treeNode->Items += extraItems;
			treeNode->ItemsetNumber += extraItemsetNumber;
			for (it=treeNode->Children->begin(), endit=treeNode->Children->end(); it != endit; it++) {
				if ((*it)->Parent == treeNode)
						updateNodeInfo((*it), extraItems, extraItemsetNumber);
			}
		}

		void updateOldNodeInfo(TreeNode* parent, TreeNode *pNode, TreeNode *qNode)
		{
			TreeNode **Res;
			Res = (TreeNode **) bsearch( &pNode, &(*(parent->Children))[0], (*(parent->Children)).size(), sizeof( TreeNode *), (int (*)(const void*, const void*))TreeNode::TreeNodeCompare );
			*Res=qNode;
		}

		void checkNodeInfo(TreeNode *treeNode)
		{
			int items;
			int itemset_number;
			TreeNode *parent = treeNode->Parent;
			NodeVector::iterator it, endit;


			if( treeNode->ItemIsIntra ) {
				itemset_number = parent->ItemsetNumber;
				items = parent->Items + 1;
			}
			else {
				itemset_number = parent->ItemsetNumber + 1;
				items = parent->Items + 1;
			}

			if ((items != treeNode->Items) || (itemset_number != treeNode->ItemsetNumber))
				printf("Node Info: Items or ItemsetNumber Error");

			for (it=treeNode->Children->begin(), endit=treeNode->Children->end(); it != endit; it++) {
				if ((*it)->Parent == treeNode)
					checkNodeInfo((*it));
			}



		}

		//int qqq;
		int addSequence(PROJ_DB *pDB, TreeNode **currentLevelp, ReverseHashTable *reverseTable)
		{
			TreeNode   *pNode, *qNode;
			TreeNode *currentLevel=*currentLevelp;

			ReverseHashTable::iterator it, endit;

			int ret, myNumOfItems;

			/*printf("SSN: %d Item: %d\n", ++qqq, pDB->Item);

			if (qqq == 20496) {
				printf("hello\n");
			}*/



			//myNumOfItems=pDB->NumOfItems + pDB->m_nMaxSup - pDB->m_nSup;
			myNumOfItems=pDB->NumOfItems + pDB->m_nMaxSup;
			//myNumOfItems=pDB->NumOfItems;

			/*if (myNumOfItems == 3378012)
					printf("GUGU\n");
			*/


			add_seq_mtx.lock();
			const ReverseMap& pMap=(*reverseTable).equal_range(myNumOfItems);
			for (it=pMap.first, endit=pMap.second; it != endit; it++) {
				pNode=(*it).second;
				if (pNode->Support != pDB->m_nMaxSup)
					continue;
				ret=isContained_DBSizeEqual(pDB, currentLevel, pNode);
				switch (ret) {
					case 0:
						continue;
					case 1:  //Backward SubPattern
						//(*(currentLevel->Children)).push_back(pNode);
						//(*(currentLevel->IntraChildren))[pDB->Item]=pNode;
						//qNode is a mirror of pNode, but has slight different
						//the major goal is to let closed_maxPruning much easier
						qNode= new TreeNode();
						qNode= currentLevel->AddChildWithoutChecking( pDB->Item, pDB->ItemIsIntra, pDB->m_nMaxSup);
						qNode->SetProjDBSize(myNumOfItems);
						qNode->Children=pNode->Children;

						add_seq_mtx.unlock();
						return EQUAL_PROJECT_DB_SIZE;

					case -1: //Backward SuperPattern

						qNode= new TreeNode(pNode);
						updateOldNodeInfo(pNode->Parent, pNode, qNode);

						if (pDB->ItemIsIntra) {
							updateNodeInfo(pNode,
										   currentLevel->Items - pNode->Parent->Items,
										   currentLevel->ItemsetNumber - pNode->Parent->ItemsetNumber);
							//checkNodeInfo(currentLevel);
							pNode->ItemIsIntra=true;
							pNode->Parent=currentLevel;
							(*(currentLevel->Children)).push_back(pNode);
							inplace_merge((*(currentLevel->Children)).begin(), (*(currentLevel->Children)).end()-1,
										  (*(currentLevel->Children)).end(), TreeNode::TreeNodeLess );
						}
						else {
							updateNodeInfo(pNode,
										   currentLevel->Items - pNode->Parent->Items,
										   currentLevel->ItemsetNumber - pNode->Parent->ItemsetNumber);
							//checkNodeInfo(currentLevel);
							//pNode->Parent->Children->erase(pNode);
							pNode->ItemIsIntra=false;
							pNode->Parent=currentLevel;
							(*(currentLevel->Children)).push_back(pNode);
							inplace_merge((*(currentLevel->Children)).begin(), (*(currentLevel->Children)).end()-1,
										  (*(currentLevel->Children)).end(), TreeNode::TreeNodeLess );

						}

						//reverseTable->erase(it);
						//reverseTable->insert(ReverseHashTable::value_type(myNumOfItems, qNode));
						add_seq_mtx.unlock();
						return EQUAL_PROJECT_DB_SIZE;
					default:
						break;
				}
			}
			pNode= new TreeNode();
			pNode= currentLevel->AddChildWithoutChecking( pDB->Item, pDB->ItemIsIntra, pDB->m_nMaxSup);
			pNode->SetProjDBSize(myNumOfItems);
			(*currentLevelp)=pNode;
			reverseTable->insert(ReverseHashTable::value_type(myNumOfItems, pNode));
			add_seq_mtx.unlock();
			return INEQUAL_PROJECT_DB_SIZE;
		}


		void closed_maxChecking(TreeNode *pNode, TreeNode *pNode_parent, TreeNode *qNode)
		{
			NodeVector::iterator it, endit;

			if (pNode == NULL)
				return;

			if (pNode->Parent != pNode_parent)
				return;

			if (pNode->Support == qNode->Support)
				pNode->closed = false;

			for (it=qNode->Children->begin(), endit=qNode->Children->end(); it != endit; it++)
				closed_maxChecking (pNode->FindChild((*it)->Item, (*it)->ItemIsIntra), pNode, (*it));

		}

		void closed_maxPruning(TreeNode *treeNode, TreeNode *parent)
		{
			NodeVector::iterator it, endit, ti;
			bool myItemIsIntra=treeNode->ItemIsIntra;
			TreeNode *pNode;

			if (parent->Support == treeNode->Support) {
				parent->closed = false;
			}


			for (it=treeNode->Children->begin(), endit=treeNode->Children->end(); it != endit; it++) {
				if ((*it)->Parent == treeNode)
					closed_maxPruning((*it), treeNode);
			}
			//check  subpattern closed

			for (it=treeNode->Children->begin(), endit=treeNode->Children->end(); it != endit; it++) {
				if (!((*it)->ItemIsIntra && myItemIsIntra))
						pNode=parent->FindChild((*it)->Item, false);
				else
					pNode=parent->FindChild((*it)->Item, true);

				closed_maxChecking (pNode, parent, (*it));
			}

		}

typedef vector<TreeNode *> TreeNodeList;

TreeNode *root;
ReverseHashTable *reverseTable; // from NumOfItems to TreeNode*, multimap //!!! change by wenwen



#endif // !defined( _CLOSED_TREE_H__ )

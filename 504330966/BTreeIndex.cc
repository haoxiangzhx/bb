/*
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */
 
#include "BTreeIndex.h"
#include "BTreeNode.h"
#include <iostream>
#include <queue>
#include <unordered_set>

using namespace std;

void printLeaf(PageId pid, const PageFile& pf, queue<PageId>& q);
void printNonLeaf(PageId pid, const PageFile& pf, queue<PageId>& q);

// int main(){
// 	BTreeIndex index = BTreeIndex();
// 	RC ret = index.open("mv.idx", 'w');
// 	cout<<"ret of index.open() is "<<ret<<endl;
// 	RecordId rid;
	

// 	// get rootpid and tree height
// 	printf("Rootpid is %d and Treeheight is %d\n", index.getRootPid(), index.getTreeHeight());

// 	// get root node info
// 	queue<PageId> q;
// 	printNonLeaf(index.getRootPid(), index.getPageFile(), q);
// 	int h = index.getTreeHeight();
// 	PageFile file = index.getPageFile();
// 	while(h > 0){
// 		h--;
// 		if(h==1){ //leaf node
// 			int size=q.size();
// 			for(int i=0; i<size; i++){
// 				PageId id = q.front(); q.pop();
// 				cout<<"pid is "<<id<<endl;
// 				printLeaf(id, file, q);
// 			}
// 		}
// 		else{ //nonleaf node
// 			int size=q.size();
// 			for(int i=0; i<size; i++){
// 				PageId id = q.front(); q.pop();
// 				cout<<"pid is "<<id<<endl;
// 				printNonLeaf(id, file, q);
// 			}
// 		}
// 	}
// 	// printLeaf(3, index.getPageFile());
// 	// printLeaf(4, index.getPageFile());
// 	index.close();

// 	IndexCursor cursor;
// 	RC rc = index.locate(886, cursor);
// 	printf("Returned rc is %d, cursor.pid is %d and cursor.eid is %d\n", rc, cursor.pid, cursor.eid);

// 	cursor = {120, 2};
// 	int k;
// 	RecordId r;
// 	rc = index.readForward(cursor, k, r);
// 	printf("rc is %d, cursor pid is %d, cursor eid is %d, key is %d and rid.pid is %d, rid.sid is %d\n", rc, cursor.pid, cursor.eid, k, r.pid, r.sid);
// }

void printNonLeaf(PageId pid, const PageFile& pf, queue<PageId> &q){
	BTNonLeafNode nl = BTNonLeafNode();
	nl.read(pid, pf);
	for(int i=0; i<nl.getKeyCount(); i++){
		if(i==0){
			q.push(bytes2int(nl.buffer));
			cout<<"First pid is "<<bytes2int(nl.buffer)<<endl;
		}
		q.push(bytes2int(nl.buffer+i*8+8));
		printf("At offset %d, key is %d and pid is %d\n", i, bytes2int(nl.buffer+i*8+4), bytes2int(nl.buffer+i*8+8));
	}
}

// print leaf node information
void printLeaf(PageId pid, const PageFile& pf, queue<PageId>& q){
	BTLeafNode l = BTLeafNode();
	l.read(pid, pf);
	int eid = 0, key;
	RecordId rid;
	cout<<"l.getKeyCount() is "<<l.getKeyCount()<<endl;
	for(eid; eid<l.getKeyCount(); eid++){
		l.readEntry(eid, key, rid);
		printf("At eid %d, key is %d, rid.pid is %d and rid.sid is %d\n", eid, key, rid.pid, rid.sid);
	}

	PageId next = l.getNextNodePtr();
	cout<<"nextnodeptr is "<<next<<endl;
}

/*
 * BTreeIndex constructor
 */
BTreeIndex::BTreeIndex()
{
    rootPid = -1;
    treeHeight = 0;
    memset(buffer, 0, PageFile::PAGE_SIZE);
}

/*
 * Open the index file in read or write mode.
 * Under 'w' mode, the index file should be created if it does not exist.
 * @param indexname[IN] the name of the index file
 * @param mode[IN] 'r' for read, 'w' for write
 * @return error code. 0 if no error
 */
RC BTreeIndex::open(const string& indexname, char mode)
{
	RC error = pf.open(indexname, mode);
	if(error!=0)
		return error;

	if(pf.endPid()==0){
		rootPid = -1;
		treeHeight = 0;
		memcpy(buffer, &rootPid, sizeof(PageId));
		memcpy(buffer+sizeof(PageId), &treeHeight, sizeof(int));
		error = pf.write(0, buffer);
		if(error!=0)
			return error;
	}
	else{
		error = pf.read(0, buffer);
		if(error!=0)
			return error;
		memcpy(&rootPid, buffer, sizeof(PageId));
		memcpy(&treeHeight, buffer+sizeof(PageId), sizeof(int));
	}
	
    return error;
}

/*
 * Close the index file.
 * @return error code. 0 if no error
 */
RC BTreeIndex::close()
{
    memcpy(buffer, &rootPid, sizeof(PageId));
	memcpy(buffer+sizeof(PageId), &treeHeight, sizeof(int));
	RC error = pf.write(0, buffer);
	if(error!=0)
		return error;
	error = pf.close();
    return error;
}
// 
/*
 * Insert (key, RecordId) pair to the index.
 * @param key[IN] the key for the value inserted into the index
 * @param rid[IN] the RecordId for the record being inserted into the index
 * @return error code. 0 if no error
 */
RC BTreeIndex::insert(int key, const RecordId& rid)
{
	RC error = -1;
	
	if (rootPid == -1)
	{
		rootPid = pf.endPid();
		treeHeight=2;

		BTNonLeafNode nln;
		nln.initializeRoot(pf.endPid()+1, key, pf.endPid()+2);
		error = nln.write(pf.endPid(), pf);
		if (error != 0)
			return error;

		BTLeafNode temp;
		temp.setNextNodePtr(pf.endPid()+1);
		error = temp.write(pf.endPid(), pf);
		if (error != 0)
			return error;

		BTLeafNode ln;
		ln.insert(key, rid);
		error = ln.write(pf.endPid(), pf);
		if (error != 0)
			return error;

		return 0;
	}

	stack<PageId> path;
    path.push(rootPid);
    BTNonLeafNode nln;
    PageId pid = rootPid;

    for(int i=0; i<treeHeight-1; i++){
    	error = nln.read(pid, pf);
    	if(error != 0)
    		return error;
    	error = nln.locateChildPtr(key, pid);
    	if(error != 0)
    		return error;
    	path.push(pid);
    }
    path.pop();

    BTLeafNode ln;
    error = ln.read(pid, pf);
    if (error != 0)
    	return error;

    if (ln.getKeyCount() < 80)
    {
    	ln.insert(key, rid);
    	ln.write(pid, pf);
    	if (error != 0)
    		return error;
    	return 0;
    }

    BTLeafNode sibling;
    int siblingKey;
    ln.insertAndSplit(key, rid, sibling, siblingKey);
    sibling.setNextNodePtr(ln.getNextNodePtr());
    ln.setNextNodePtr(pf.endPid());

    ln.write(pid, pf);
	if (error != 0)
    	return error;
    sibling.write(pf.endPid(), pf);
	if (error != 0)
    	return error;

    error = insertParent(path, siblingKey, pf.endPid()-1);
    if (error != 0)
    	return error;

    return 0;
}

RC BTreeIndex::insertParent(stack<PageId> path, int key, PageId pid)
{
	RC error = -1;
	if (path.empty())
	{
		BTNonLeafNode nln;
		nln.initializeRoot(rootPid, key, pid);
		error = nln.write(pf.endPid(), pf);
		if (error != 0)
			return error;

		rootPid = pf.endPid() - 1;
		treeHeight++;

		return 0;
	}

	PageId parent = path.top();
	path.pop();

	BTNonLeafNode nln;
	error = nln.read(parent, pf);
	if (error != 0)
		return error;

	if (nln.getKeyCount() < 120)
	{
		nln.insert(key, pid);
		nln.write(parent, pf);
		if (error != 0)
			return error;
		return 0;
	}

	BTNonLeafNode sibling;
	int midKey;
	nln.insertAndSplit(key, pid, sibling, midKey);
	nln.write(parent, pf);
	if (error != 0)
		return error;
	sibling.write(pf.endPid(), pf);
	if (error != 0)
		return error;

	return insertParent(path, midKey, pf.endPid()-1);
}

/**
 * Run the standard B+Tree key search algorithm and identify the
 * leaf node where searchKey may exist. If an index entry with
 * searchKey exists in the leaf node, set IndexCursor to its location
 * (i.e., IndexCursor.pid = PageId of the leaf node, and
 * IndexCursor.eid = the searchKey index entry number.) and return 0.
 * If not, set IndexCursor.pid = PageId of the leaf node and
 * IndexCursor.eid = the index entry immediately after the largest
 * index key that is smaller than searchKey, and return the error
 * code RC_NO_SUCH_RECORD.
 * Using the returned "IndexCursor", you will have to call readForward()
 * to retrieve the actual (key, rid) pair from the index.
 * @param key[IN] the key to find
 * @param cursor[OUT] the cursor pointing to the index entry with
 *                    searchKey or immediately behind the largest key
 *                    smaller than searchKey.
 * @return 0 if searchKey is found. Othewise an error code
 */
RC BTreeIndex::locate(int searchKey, IndexCursor& cursor)
{
    BTNonLeafNode nonleafnode = BTNonLeafNode();
    // read through non leafnode to get pid of leafnode
    PageId pid = rootPid;
    RC error = -1;
    for(int i=0; i<treeHeight-1; i++){
    	error = nonleafnode.read(pid, pf);
    	if(error != 0)
    		return error;
    	error = nonleafnode.locateChildPtr(searchKey, pid);
    	if(error != 0)
    		return error;
    }

    BTLeafNode leafnode = BTLeafNode();
    int eid = -1;
    error = leafnode.read(pid, pf);
    if(error != 0)
    	return error;

    if(leafnode.getKeyCount()==0){
    	pid = leafnode.getNextNodePtr();
    	error = leafnode.read(pid, pf);
    	if(error!=0)
    		return error;
    }

    error = leafnode.locate(searchKey, eid);
    if(error != 0){
    	cursor.pid = pid;
    	cursor.eid = eid;
    	return error;
    }
    else{
    	cursor.pid = pid;
    	cursor.eid = eid;
    }

    return 0;
}

/*
 * Read the (key, rid) pair at the location specified by the index cursor,
 * and move foward the cursor to the next entry.
 * @param cursor[IN/OUT] the cursor pointing to an leaf-node index entry in the b+tree
 * @param key[OUT] the key stored at the index cursor location.
 * @param rid[OUT] the RecordId stored at the index cursor location.
 * @return error code. 0 if no error
 */
void printR(RecordId& rid){
  printf("Rid.pid is %d and rid.sid is %d\n", rid.pid, rid.sid);
}

RC BTreeIndex::readForward(IndexCursor& cursor, int& key, RecordId& rid)
{
	//printf("Before action, cursor.pid is %d and cursor.eid is %d\n", cursor.pid, cursor.eid);
	PageId pid = cursor.pid;
	int eid = cursor.eid;
	if(pid <=0 || eid < 0 )
		return RC_INVALID_CURSOR;

	BTLeafNode leafnode = BTLeafNode();
	RC error = -1;
	error = leafnode.read(pid, pf);
	if(error != 0)
		return error;

	//cout<<"Line 382 key "<<key<<endl;
	//printR(rid);
	error = leafnode.readEntry(eid, key, rid);
	//cout<<"Line 385 key "<<key<<endl;
	//printR(rid);
	if(error != 0)
		return error;

	eid++;
	if(eid == leafnode.getKeyCount()){
		eid = 0;
		pid = leafnode.getNextNodePtr();
		error = leafnode.read(pid, pf);
		if(error != 0)
			return error;
	}

	cursor.pid = pid;
	cursor.eid = eid;
	//printf("After action, cursor.pid is %d and cursor.eid is %d\n", cursor.pid, cursor.eid);

    return 0;
}
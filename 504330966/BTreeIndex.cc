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
#include <string.h>

using namespace std;

int main{};

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
	else{
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
		treeHeight++;

		BTNonLeafNode nln;
		nln.initializeRoot(pf.endPid()+1, key, pf.endPid()+2);
		error = nln.write(pf.endPid(), pf);
		if (error != 0)
			return error;

		char temp[PageFile::PAGE_SIZE];
		error = pf.write(pf.endPid(), temp);
		if (error != 0)
			return error;

		BTLeafNode ln;
		ln.insert(key, rid);
		error = ln.write(pf.endPid(), pf);
		if (error != 0)
			return error;

		rootPid = pf.endPid() - 1;

		return 0;
	}

	stack<PageId> path;
    path.push(rootPid);
    BTNonLeafNode nln;
    PageId pid = rootPid;

    for(int i=0; i<treeHeight; i++){
    	error = nln.read(pid, pf);
    	if(error != 0)
    		return error;
    	error = nln.locateChildPtr(key, pid);
    	if(error != 0)
    		return error;
    	path.push(pid);
    }

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
    for(int i=0; i<treeHeight; i++){
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
    error = leafnode.locate(searchKey, eid);
    if(error != 0)
    	return error;
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
RC BTreeIndex::readForward(IndexCursor& cursor, int& key, RecordId& rid)
{
	PageId pid = cursor.pid;
	int eid = cursor.eid;
	if(pid <=0 || eid < 0 )
		return RC_INVALID_CURSOR;

	BTLeafNode leafnode = BTLeafNode();
	RC error = -1;
	error = leafnode.read(pid, pf);
	if(error != 0)
		return error;
	error = leafnode.readEntry(eid, key, rid);
	if(error != 0)
		return error;

	if(eid == leafnode.getKeyCount()){
		eid = 0;
		pid = leafnode.getNextNodePtr();
	}
	else
		eid++;

	cursor.pid = pid;
	cursor.eid = eid;
    return 0;
}

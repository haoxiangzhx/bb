#include "BTreeNode.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unordered_set>

using namespace std;

void int2bytes(int n, char* bytes)
{
	int x=n;
    // bytes[0] = (x >> 24) & 0xFF;
    // bytes[1] = (x >> 16) & 0xFF;
    // bytes[2] = (x >> 8) & 0xFF;
    // bytes[3] = x & 0xFF;
    memcpy(bytes, &x, sizeof(int));
}

int bytes2int(char* bytes)
{
    //return (unsigned int) (bytes[0] << 24) | (unsigned int) (bytes[1] << 16) | (unsigned int) (bytes[2] << 8) | (unsigned int) bytes[3];
	int x;
	memcpy(&x, bytes, sizeof(int));
	return x;
}

int    
main (void)    
{

	// //=====================================
	// // test BTL::read and BTL::write

	// PageFile pf("test.txt", 'w');
	// BTNonLeafNode ln1, ln2;
 //    memset(ln1.buffer, 0, 1024);
 //    cout<<"getKeyCount "<<ln1.getKeyCount()<<endl;
	// PageId pid = pf.endPid();
	// cout<<"pid is "<<pid<<endl;
	// char bf[]="this is pagefile 1";
	// strcpy(ln1.buffer, bf);
	// cout<<"ln1 write "<<ln1.buffer<<endl;
	// ln1.write(pid, pf);
	// ln2.read(pid, pf);
	// cout<<"ln2 read "<<ln2.buffer<<endl;
	// //======================================

	//======================================
	// test int2bytes and bytes2int
	// char bytes[4];
	// while(true){
	// 	int r=rand();
	// 	int2bytes(r, bytes);
	// 	int res=bytes2int(bytes);
	// 	printf("trans int %d to res %d\n", r, res);
	// 	if(r!=res)
	// 		break;
	// }

	// //======================================
	// stress test!!!!
	//

	// while (true)
	// {
	// 	BTNonLeafNode test;
 //        memset(test.buffer, 0, 1024);
 //        test.initializeRoot(98, -100, 99);
 //        printf("first pid %d, first key %d, second pid %d and count %d\n", bytes2int(test.buffer), bytes2int(test.buffer+4), bytes2int(test.buffer+8), bytes2int(test.buffer+1016));


	// 	unordered_set<int> dup;

 //        int end=100, rem=10000;

	// 	for(int i=0; i < end; i++){
	// 		PageId p=100;
	// 		int key=rand()%rem;
	// 		while(dup.find(key)!=dup.end()){
	// 			key=rand()%rem;
	// 			cout<<key<<endl;
	// 		}
	// 		dup.insert(key);
	// 		test.insert(key, p+i);
	// 		printf("Insert key %d, pid %d\n", key, p+i);
	// 	}

	// 	int key, tmp=-200, offset=0; PageId pageid;
	// 	for(int i=0; i < end+1; i++){
	// 		offset=i*8;
 //            key = bytes2int(test.buffer+offset+4);
 //            printf("At eid %d, key at is %d, tmp is %d, page id is %d\n", i, key, tmp, bytes2int(test.buffer+offset+8));
	// 		//printf("At eid %d, key at offset %d is %d, tmp is %d, page id at offset %d is %d\n", i,offset+4, bytes2int(test.buffer+offset+4), tmp, offset, bytes2int(test.buffer+offset));
	// 		if(key < tmp)
	// 			goto exit_end; 	
	// 		tmp=key;		
	// 	}
	// }

	// exit_end:

	// return 0;


	// BTNonLeafNode test, sib;
	// 	//test.count=0;
	// memset(test.buffer, 0, 1024);

	// unordered_set<int> dup;

 //    int end=9;
 //    PageId p=100;
 //    int2bytes(99, test.buffer);
    
	// for(int i=0; i < end; i++){
	// 	int key=rand()%1000;
	// 	while(dup.find(key)!=dup.end()){
	// 		key=rand()%1000;
	// 		cout<<key<<endl;
	// 	}
	// 	dup.insert(key);
	// 	test.insert(key, p+i);
	// 	printf("Insert key %d, pageid %d\n", key, p+i);
	// }

 //    cout<<"First Pid "<<bytes2int(test.buffer)<<endl;
	// int key, tmp=0; PageId pageid;
	// for(int i=0; i < end; i++){
	// 	int offset=i*8;
 //        key = bytes2int(test.buffer+offset+4);
 //        printf("At eid %d, key at is %d, tmp is %d, page id is %d\n", i, key, tmp, bytes2int(test.buffer+offset+8));
 //        // if(key < tmp)
 //        //     goto exit_end;  
 //        // tmp=key;			
	// }

 //    int loc=0;
 //    int ret = test.locateChildPtr(1000, loc);
 //    cout<<ret<<" "<<loc<<endl;



 //    // key=rand()%1000;
 //    // while(dup.find(key)!=dup.end()){
 //    //  key=rand()%1000;
 //    //  cout<<key<<endl;
 //    // }
 //    // dup.insert(key);
 //    // int midkey;
 //    // test.insertAndSplit(key, p+10, sib, midkey);
 //    // printf("Insert and split key %d, pageid %d, midkey %d\n", key, p+10, midkey);   

 //    // cout<<"First Pid "<<bytes2int(test.buffer)<<endl;
 //    // for(int i=0; i<bytes2int(test.buffer+1016); i++){
 //    //     int offset=i*8;
 //    //     key = bytes2int(test.buffer+offset+4);
 //    //     printf("At eid %d, key at is %d, tmp is %d, page id is %d\n", i, key, tmp, bytes2int(test.buffer+offset+8));
 //    // }

 //    // cout<<"First Pid "<<bytes2int(sib.buffer)<<endl;
 //    // for(int i=0; i<bytes2int(sib.buffer+1016); i++){
 //    //     int offset=i*8;
 //    //     key = bytes2int(sib.buffer+offset+4);
 //    //     printf("At eid %d, key at is %d, tmp is %d, page id is %d\n", i, key, tmp, bytes2int(sib.buffer+offset+8));
 //    // }

	// // // test locate function
	// // // int loc=-1;
	// // // int ret=test.locate(300, loc);
	// // // cout<<ret<<" "<<loc<<endl;

	// PageFile pf("test.txt", 'w');
	// test.write(1, pf);
	// //sib.setNextNodePtr(1);
	// BTNonLeafNode b;
	// b.read(1, pf);
 //    cout<<"First Pid "<<bytes2int(b.buffer)<<endl;
	// for(int i=0; i < b.getKeyCount(); i++){
	// 	int offset=i*8;
 //        key = bytes2int(b.buffer+offset+4);
 //        printf("BTNonLeafNode b At eid %d, key at is %d, tmp is %d, page id is %d\n", i, key, tmp, bytes2int(b.buffer+offset+8));
	// }
 //    exit_end:

    return 0;
}


/*
 * Read the content of the node from the page pid in the PageFile pf.
 * @param pid[IN] the PageId to read
 * @param pf[IN] PageFile to read from
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::read(PageId pid, const PageFile& pf)
{
    int rc;
    if ((rc = pf.read(pid, buffer)) < 0) 
    {
      fprintf(stderr, "Error: while reading a page from page %d\n", pid);
      return rc;
    }
    return 0;
}
    
/*
 * Write the content of the node to the page pid in the PageFile pf.
 * @param pid[IN] the PageId to write to
 * @param pf[IN] PageFile to write to
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::write(PageId pid, PageFile& pf)
{
    int rc;
    if ((rc = pf.write(pid, buffer)) < 0){
        fprintf(stderr, "Error: while writing a page from page %d\n", pid);
        return rc;
    }
    return 0;
}

/*
 * Return the number of keys stored in the node.
 * @return the number of keys in the node
 */
int BTLeafNode::getKeyCount()
{
    return bytes2int(buffer+1016); 
}

/*
 * Insert a (key, rid) pair to the node.
 * @param key[IN] the key to insert
 * @param rid[IN] the RecordId to insert
 * @return 0 if successful. Return an error code if the node is full.
 */
RC BTLeafNode::insert(int key, const RecordId& rid)
{
    int count = getKeyCount();
    int left, right, mid;
    left = 0;
    right = count;
    
    while (left+1 < right)
    {
        mid = (left+right) / 2;
        int offset = mid * 12;
        int midkey = bytes2int(buffer+offset);
        
        if (key < midkey)
            right = mid;
        else
            left = mid; 
    }

    int k=bytes2int(buffer+left*12);
    if(k >= key)
    	right=left;
    
    int offset = (right+1) * 12;
    //printf("begin i is %d, end i is %d\n", count*12-1, offset);
    for (int i = (count+1)*12-1; i >= offset; i--)
        buffer[i] = buffer[i-12];
        
    int2bytes(key, buffer+right*12);
    int2bytes(rid.pid, buffer+right*12+4);
    int2bytes(rid.sid, buffer+right*12+8);

    int2bytes(++count, buffer+1016);

    //printf("Offset is %d, count is %d, key is %d\n", right, count, key);
    
    return 0; 
}

/*
 * Insert the (key, rid) pair to the node
 * and split the node half and half with sibling.
 * The first key of the sibling node is returned in siblingKey.
 * @param key[IN] the key to insert.
 * @param rid[IN] the RecordId to insert.
 * @param sibling[IN] the sibling node to split with. This node MUST be EMPTY when this function is called.
 * @param siblingKey[OUT] the first key in the sibling node after split.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::insertAndSplit(int key, const RecordId& rid, 
                              BTLeafNode& sibling, int& siblingKey)
{
    insert(key, rid);
    int count = getKeyCount();
    int half = (count+1) / 2;
    int otherhalf = count - half;
    int offset = half * 12;
    memcpy(sibling.buffer, buffer+offset, otherhalf*12);
    
    int2bytes(half, buffer+1016);
    int2bytes(otherhalf, sibling.buffer+1016);
    
    siblingKey = bytes2int(buffer+offset);
    
    return 0;
}

/**
 * If searchKey exists in the node, set eid to the index entry
 * with searchKey and return 0. If not, set eid to the index entry
 * immediately after the largest index key that is smaller than searchKey,
 * and return the error code RC_NO_SUCH_RECORD.
 * Remember that keys inside a B+tree node are always kept sorted.
 * @param searchKey[IN] the key to search for.
 * @param eid[OUT] the index entry number with searchKey or immediately
                   behind the largest key smaller than searchKey.
 * @return 0 if searchKey is found. Otherwise return an error code.
 */
RC BTLeafNode::locate(int searchKey, int& eid)
{
    int i = 0, count = getKeyCount();
    while (i < count)
    {
    	///cout<<i<<endl;
        int key = bytes2int(buffer+i*PAIR_SIZE);
        if (searchKey > key)
            i += 1;
        else
        {
            eid = i;
            if (searchKey == key)
                return 0;
            else{
                return RC_NO_SUCH_RECORD;
            }
        }  
    }
    
    eid=count;
    return RC_NO_SUCH_RECORD;
}

/*
 * Read the (key, rid) pair from the eid entry.
 * @param eid[IN] the entry number to read the (key, rid) pair from
 * @param key[OUT] the key from the entry
 * @param rid[OUT] the RecordId from the entry
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::readEntry(int eid, int& key, RecordId& rid)
{
    int offset = eid * 12;
    
    key = bytes2int(buffer+offset);
    rid.pid = bytes2int(buffer+offset+4);
    rid.sid = bytes2int(buffer+offset+8);
    
    return 0;
}

/*
 * Return the pid of the next slibling node.
 * @return the PageId of the next sibling node 
 */
PageId BTLeafNode::getNextNodePtr()
{
    return bytes2int(buffer+1020);
}

/*
 * Set the pid of the next slibling node.
 * @param pid[IN] the PageId of the next sibling node 
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::setNextNodePtr(PageId pid)
{
    int2bytes(pid, buffer+1020);
    return 0;
}

/*
 * Read the content of the node from the page pid in the PageFile pf.
 * @param pid[IN] the PageId to read
 * @param pf[IN] PageFile to read from
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::read(PageId pid, const PageFile& pf)
{
    int rc;
    if ((rc = pf.read(pid, buffer)) < 0) 
    {
      fprintf(stderr, "Error: while reading a page from page %d\n", pid);
      return rc;
    }
    return 0;
}
    
/*
 * Write the content of the node to the page pid in the PageFile pf.
 * @param pid[IN] the PageId to write to
 * @param pf[IN] PageFile to write to
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::write(PageId pid, PageFile& pf)
{
    int rc;
    if ((rc = pf.write(pid, buffer)) < 0){
        fprintf(stderr, "Error: while writing a page from page %d\n", pid);
        return rc;
    }
    return 0;
}

/*
 * Return the number of keys stored in the node.
 * @return the number of keys in the node
 */
int BTNonLeafNode::getKeyCount()
{
    return bytes2int(buffer+1016);
}


/*
 * Insert a (key, pid) pair to the node.
 * @param key[IN] the key to insert
 * @param pid[IN] the PageId to insert
 * @return 0 if successful. Return an error code if the node is full.
 */
RC BTNonLeafNode::insert(int key, PageId pid)
{
    int count = getKeyCount();
    int left, right, mid;
    left = 0;
    right = count;

    //cout<<left<<" "<<right<<endl;
    
    while (left+1 < right)
    {
        mid = (left+right) / 2;
        int offset = mid * 8;
        int midkey = bytes2int(buffer+offset+4);
        
        //cout<<"offset "<<offset<<" midkey "<<midkey<<endl;

        if (key < midkey)
            right = mid;
        else
            left = mid; 
    }
    
    int k = bytes2int(buffer+left*8+4);
    if (k >= key)
        right=left;
    
    //cout<<k<<" "<<left<<" "<<right<<endl;

    int offset = 4 + (right+1) * 8;
    for (int i = 4+(count+1)*8-1; i >= offset; i--)
        buffer[i] = buffer[i-8];
        
    int2bytes(key, buffer+4+right*8);
    int2bytes(pid, buffer+4+right*8+4);
    //printf("At offset %d pid is %d, and at offset %d key is %d\n", right*8, bytes2int(buffer+right*8), right*8+4, bytes2int(buffer+right*8+4));

    int2bytes(++count, buffer+1016);
    
    return 0; 
}

/*
 * Insert the (key, pid) pair to the node
 * and split the node half and half with sibling.
 * The middle key after the split is returned in midKey.
 * @param key[IN] the key to insert
 * @param pid[IN] the PageId to insert
 * @param sibling[IN] the sibling node to split with. This node MUST be empty when this function is called.
 * @param midKey[OUT] the key in the middle after the split. This key should be inserted to the parent node.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::insertAndSplit(int key, PageId pid, BTNonLeafNode& sibling, int& midKey)
{
    insert(key, pid);
    int count = getKeyCount();
    int half = count / 2;
    int otherhalf = count - half - 1;
    int offset = half * 8;
    midKey = bytes2int(buffer+offset+4);
    memcpy(sibling.buffer, buffer+offset+8, otherhalf*8+4);
    
    int2bytes(half, buffer+1016);
    int2bytes(otherhalf, sibling.buffer+1016);
    
    return 0;
}

/*
 * Given the searchKey, find the child-node pointer to follow and
 * output it in pid.
 * @param searchKey[IN] the searchKey that is being looked up.
 * @param pid[OUT] the pointer to the child node to follow.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::locateChildPtr(int searchKey, PageId& pid)
{
    int i = 0, count = getKeyCount();
    while (i < count)
    {
        int key = bytes2int(buffer+i*8+4);
        if (searchKey > key)
            i += 1;
        else
        {
            if (searchKey == key){
            	pid = bytes2int(buffer+(i+1)*8);
                return 0;
            }
            else{
            	pid = bytes2int(buffer+i*8);
                return 0;
            }
        }  
    }
    
    pid = bytes2int(buffer+count*8);
    return 0;
}

/*
 * Initialize the root node with (pid1, key, pid2).
 * @param pid1[IN] the first PageId to insert
 * @param key[IN] the key that should be inserted between the two PageIds
 * @param pid2[IN] the PageId to insert behind the key
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::initializeRoot(PageId pid1, int key, PageId pid2)
{
    int2bytes(pid1, buffer);
    int2bytes(key, buffer+4);
    int2bytes(pid2, buffer+8);
    int2bytes(1, buffer+1016);
    return 0;
}

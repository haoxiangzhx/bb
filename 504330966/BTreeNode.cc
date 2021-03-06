#include "BTreeNode.h"

using namespace std;

void int2bytes(int n, char* bytes)
{
	int x=n;
    memcpy(bytes, &x, sizeof(int));
}

int bytes2int(char* bytes)
{
	int x;
	memcpy(&x, bytes, sizeof(int));
	return x;
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
    for (int i = (count+1)*12-1; i >= offset; i--)
        buffer[i] = buffer[i-12];
        
    int2bytes(key, buffer+right*12);
    int2bytes(rid.pid, buffer+right*12+4);
    int2bytes(rid.sid, buffer+right*12+8);

    int2bytes(++count, buffer+1016);
   
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
    if(eid >= getKeyCount())
        return RC_NO_SUCH_RECORD;
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
    
    while (left+1 < right)
    {
        mid = (left+right) / 2;
        int offset = mid * 8;
        int midkey = bytes2int(buffer+offset+4);

        if (key < midkey)
            right = mid;
        else
            left = mid; 
    }
    
    int k = bytes2int(buffer+left*8+4);
    if (k >= key)
        right=left;

    int offset = 4 + (right+1) * 8;
    for (int i = 4+(count+1)*8-1; i >= offset; i--)
        buffer[i] = buffer[i-8];
        
    int2bytes(key, buffer+4+right*8);
    int2bytes(pid, buffer+4+right*8+4);

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
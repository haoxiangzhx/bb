/**
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <climits>
#include "Bruinbase.h"
#include "SqlEngine.h"
#include "BTreeIndex.h"

using namespace std;

// external functions and variables for load file and sql command parsing 
extern FILE* sqlin;
int sqlparse(void);

bool operator < (const IndexCursor& c1, const IndexCursor& c2);
vector<int> getEQ(const vector<SelCond>& cond);
int getUpperBound(const vector<SelCond> cond, SelCond &res);
int getLowerBound(const vector<SelCond> cond, SelCond &res);
void printTuple(int &attr, int &key, string &value);

void printCursor(IndexCursor& c){
  printf("IndexCursor pid is %d and eid is %d\n", c.pid, c.eid);
}

void printRid(RecordId& rid){
  printf("Rid.pid is %d and rid.sid is %d\n", rid.pid, rid.sid);
}

RC SqlEngine::run(FILE* commandline)
{
  fprintf(stdout, "Bruinbase> ");

  // set the command line input and start parsing user input
  sqlin = commandline;
  sqlparse();  // sqlparse() is defined in SqlParser.tab.c generated from
               // SqlParser.y by bison (bison is GNU equivalent of yacc)

  return 0;
}

RC SqlEngine::select(int attr, const string& table, const vector<SelCond>& cond)
{
  RecordFile rf;   // RecordFile containing the table
  RecordId   rid;  // record cursor for table scanning

  RC     rc;
  int    key;     
  string value;
  int    count;
  int    diff;

  // open the table file
  if ((rc = rf.open(table + ".tbl", 'r')) < 0) {
    fprintf(stderr, "Error: table %s does not exist\n", table.c_str());
    return rc;
  }

  BTreeIndex btidx;
  if (btidx.open(table+".idx", 'r') < 0)
  {
  without_index:
  // scan the table file from the beginning
  rid.pid = rid.sid = 0;
  count = 0;
  while (rid < rf.endRid()) {
    // read the tuple
    if ((rc = rf.read(rid, key, value)) < 0) {
      fprintf(stderr, "Error: while reading a tuple from table %s\n", table.c_str());
      goto exit_select;
    }

    // check the conditions on the tuple
    for (unsigned i = 0; i < cond.size(); i++) {
      // compute the difference between the tuple value and the condition value
      switch (cond[i].attr) {
      case 1:
    diff = key - atoi(cond[i].value);
    break;
      case 2:
    diff = strcmp(value.c_str(), cond[i].value);
    break;
      }

      // skip the tuple if any condition is not met
      switch (cond[i].comp) {
      case SelCond::EQ:
    if (diff != 0) goto next_tuple;
    break;
      case SelCond::NE:
    if (diff == 0) goto next_tuple;
    break;
      case SelCond::GT:
    if (diff <= 0) goto next_tuple;
    break;
      case SelCond::LT:
    if (diff >= 0) goto next_tuple;
    break;
      case SelCond::GE:
    if (diff < 0) goto next_tuple;
    break;
      case SelCond::LE:
    if (diff > 0) goto next_tuple;
    break;
      }
    }

    // the condition is met for the tuple. 
    // increase matching tuple counter
    count++;

    // print the tuple 
    switch (attr) {
    case 1:  // SELECT key
      fprintf(stdout, "%d\n", key);
      break;
    case 2:  // SELECT value
      fprintf(stdout, "%s\n", value.c_str());
      break;
    case 3:  // SELECT *
      fprintf(stdout, "%d '%s'\n", key, value.c_str());
      break;
    }

    // move to the next tuple
    next_tuple:
    ++rid;
  }

  // print matching tuple count if "select count(*)"
  if (attr == 4) {
    fprintf(stdout, "%d\n", count);
  }
  rc = 0;

  // close the table file and return
  exit_select:
  rf.close();
  return rc;
  }
  else
  {
    // condition on value or comp = NE
    for (int i = 0; i < cond.size(); i++)
    {
      if ((cond[i].attr == 2) || (cond[i].comp == SelCond::NE))
        goto without_index;   
    }

    // EQ
    vector<int> eq = getEQ(cond);
    if (eq.size() == 1)
    {
      IndexCursor cursor = {-1, -1};
      RC error = btidx.locate(eq[0], cursor);
      if(error != 0)
        return error;
      else{
        int k = -1;
        RecordId r = {-1, -1};
        error = btidx.readForward(cursor, k, r);
        if(error != 0)
          return error;
        if(k != eq[0])
          return RC_NO_SUCH_RECORD;
        else{
          if(attr == 1)
            fprintf(stdout, "%d\n", k);
          if(attr == 4)
            fprintf(stdout, "%d\n", 1);
          int retKey;
          string retVal;
          if((rc = rf.read(r, retKey, retVal)) != 0)
            return rc;
          else{
            if(attr == 2)
              fprintf(stdout, "%s\n", retVal.c_str());
            if(attr == 3)
              fprintf(stdout, "%d '%s'\n", retKey, retVal.c_str());
          }
        }
      }
      return 0;
    }
    else if (eq.size() > 1)
      return 0;

    // range
    SelCond upper, lower;
    int max, min;
    max = getUpperBound(cond, upper);
    min = getLowerBound(cond, lower);

    IndexCursor minCursor, maxCursor;
    if(min == INT_MIN)
      minCursor = {1, 0};
    else{
      rc = btidx.locate(min, minCursor);
      //minCursor={0, 0};
    }
    if (rc == 0)
    {
      if (lower.comp == SelCond::GT)
        btidx.readForward(minCursor, key, rid);
    }
    if(max == INT_MAX){
      cout<<"enter 213"<<endl;
      maxCursor={0,0};
    }
    else{
      cout<<"enter 217"<<endl;
      rc = btidx.locate(max, maxCursor);
    }

    if(attr == 4){
      count = 0;
    cout<<"max";
    printCursor(maxCursor);
    //printCursor(minCursor);
      while (minCursor < maxCursor)
      {
      //printCursor(minCursor);
      //printRid(rid);
        // if(minCursor.pid == 0 && minCursor.eid == 0){
        //   cout<<"enter 236"<<endl;
        //   break;
        // }

        RC r = btidx.readForward(minCursor, key, rid);
        if(r != 0){
          cout<<"enter 241"<<endl;
          break;
        }
      //printCursor(minCursor);
        //rf.read(rid, key, value);
       // printTuple(attr, key, value);

      // if(!(minCursor < maxCursor)){
      //   printCursor(maxCursor);
      //   printCursor(minCursor);
      //   cout<<"mincursor not < maxcursor"<<endl;
      // }

      //cout<<"rc value is "<<r<<endl;
        count++;
      }

      if (rc == 0 && upper.comp == SelCond::LE)
      {
        //cout<<"enter 232"<<endl;
        RC r = btidx.readForward(minCursor, key, rid);
        count++;
      }
      fprintf(stdout, "%d\n", count);
    }
    else{
    //cout<<"max";
    //printCursor(maxCursor);
    while (minCursor < maxCursor)
    {
      //printCursor(minCursor);
      //printRid(rid);
      RC r = btidx.readForward(minCursor, key, rid);
      if(r != 0){
        //cout<<"rc value is "<<r<<endl;
        break;
      }
      //printCursor(minCursor);
      rf.read(rid, key, value);
      printTuple(attr, key, value);

      //if(!(minCursor < maxCursor))
        //cout<<"mincursor not < maxcursor"<<endl;

      //cout<<"rc value is "<<r<<endl;
      //count++;
    }

    if (rc == 0 && upper.comp == SelCond::LE)
    {
      cout<<"enter 232"<<endl;
      RC r = btidx.readForward(minCursor, key, rid);
      rf.read(rid, key, value);
      printTuple(attr, key, value);
      //count++;
    }

  }

    //if (attr == 4) 
      //fprintf(stdout, "%d\n", count);

    rf.close();
  }
}

RC SqlEngine::load(const string& table, const string& loadfile, bool index)
{
  /* your code here */
  RC rc;

  RecordFile rf;
  rf.open(table+".tbl", 'w');

  const char *file = loadfile.c_str();
  ifstream infile(file);
  string line;

  if (!index)
  {
    while (getline(infile, line))
    {
        int key;
        string value;
        parseLoadLine(line, key, value);

        RecordId rid = rf.endRid();
        rc = rf.append(key, value, rid);
        if (rc != 0)
          return rc;
    }
  }
  else
  {
    BTreeIndex btidx;
    rc = btidx.open(table+".idx", 'w');
    if (rc != 0)
      return rc;

    while (getline(infile, line))
    {
        int key;
        string value;
        parseLoadLine(line, key, value);

        RecordId rid = rf.endRid();
        rc = rf.append(key, value, rid);
        if (rc != 0)
          return rc;
        rc = btidx.insert(key, rid);
        if (rc != 0)
          return rc;
    }

    rc = btidx.close();
    if (rc != 0) 
      return rc;
  }

  rc = rf.close();
  if (rc != 0)
    return rc;

  return 0;
}

RC SqlEngine::parseLoadLine(const string& line, int& key, string& value)
{
    const char *s;
    char        c;
    string::size_type loc;
    
    // ignore beginning white spaces
    c = *(s = line.c_str());
    while (c == ' ' || c == '\t') { c = *++s; }

    // get the integer key value
    key = atoi(s);

    // look for comma
    s = strchr(s, ',');
    if (s == NULL) { return RC_INVALID_FILE_FORMAT; }

    // ignore white spaces
    do { c = *++s; } while (c == ' ' || c == '\t');
    
    // if there is nothing left, set the value to empty string
    if (c == 0) { 
        value.erase();
        return 0;
    }

    // is the value field delimited by ' or "?
    if (c == '\'' || c == '"') {
        s++;
    } else {
        c = '\n';
    }

    // get the value string
    value.assign(s);
    loc = value.find(c, 0);
    if (loc != string::npos) { value.erase(loc); }

    return 0;
}

vector<int> getEQ(const vector<SelCond>& cond)
{
  vector<int> res;
  for (int i = 0; i < cond.size(); i++)
  {
    if (cond[i].comp == SelCond::EQ)
    {
      int temp = atoi(cond[i].value);
      if (find(res.begin(), res.end(), temp) == res.end())
        res.push_back(temp);
    }
  }
  return res;
}

int getUpperBound(const vector<SelCond> cond, SelCond &res)
{
  int min = INT_MAX;
  for (int i = 0; i < cond.size(); i++)
  {
    if (cond[i].comp == SelCond::LT)
    {
      int temp = atoi(cond[i].value);
      if ((temp < min) || (temp == min && res.comp == SelCond::LE))
      {
        min = temp;
        res = cond[i];
      }
    }
    else if (cond[i].comp == SelCond::LE)
    {
      int temp = atoi(cond[i].value);
      if (temp < min)
      {
        min = temp;
        res = cond[i];
      }
    }
  }
  return min;
}

int getLowerBound(const vector<SelCond> cond, SelCond &res)
{
  int max = INT_MIN;
  for (int i = 0; i < cond.size(); i++)
  {
    if (cond[i].comp == SelCond::GT)
    {
      int temp = atoi(cond[i].value);
      if ((temp > max) || (temp == max && res.comp == SelCond::GE))
      {
        max = temp;
        res = cond[i];
      }
    }
    else if (cond[i].comp == SelCond::GE)
    {
      int temp = atoi(cond[i].value);
      if (temp > max)
      {
        max = temp;
        res = cond[i];
      }
    }
  }
  return max;
}

void printTuple(int &attr, int &key, string &value){
    switch (attr) {
    case 1:  // SELECT key
      fprintf(stdout, "%d\n", key);
      break;
    case 2:  // SELECT value
      fprintf(stdout, "%s\n", value.c_str());
      break;
    case 3:  // SELECT *
      fprintf(stdout, "%d '%s'\n", key, value.c_str());
      break;
    }
}

bool operator < (const IndexCursor& c1, const IndexCursor& c2)
{
  if (c1.pid == c2.pid){
    if(c1.eid == c2.eid)
      return false;
    return true;
  }
  return true;
}

// bool operator < (const IndexCursor& c1, const IndexCursor& c2)
// {
//   if (c1.pid == c2.pid)
//     return c1.eid < c2.eid;
//   return c1.pid < c2.pid;
// }
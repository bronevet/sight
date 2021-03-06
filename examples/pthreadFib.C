////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013, Lawrence Livermore National Security, LLC.
// Produced at the Lawrence Livermore National Laboratory.
// Written by the Greg Bronevetsky <bronevetsky1@llnl.gov> / <greg@bronevetsky.com>. 
//
// LLNL-CODE-642002
// All rights reserved.
//
// This file is part of Sight. For details, see https://e-reports-ext.llnl.gov/pdf/781752.pdf or
// https://github.com/bronevet/sight.
//
// Licensed under the GNU Lesser General Public License (Lesser GPU) Version 2.1, 
// February 1999; you may not use this file except in compliance with the License.  
// The full licence is included in file LICENCE and you may obtain a copy of the 
// License at: 
// https://www.gnu.org/licenses/old-licenses/lgpl-2.1.en.html
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
// implied. See the License for the specific language governing
// permissions and limitations under the license.
////////////////////////////////////////////////////////////////////////////////

#include "sight_pthread.h"
using namespace sight;
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

pthread_attr_t joinableAttr;
class fibArg {
  public:
  int a;
  vector<int> comparisonKeys;
  vector<comparison*> comparisonTags;
  
  fibArg(int a) {
    this->a = a;
    comparisonKeys.push_back(a);
  }
  
  fibArg(int a, const fibArg& callerArg) {
    this->a = a;
    comparisonKeys = callerArg.comparisonKeys;
    //comparisonKeys.push_back(callerArg.a);
    comparisonKeys.push_back(a);
  }
  
  void beginComparisons() {
    //cout << pthread_self() << ": "<<str()<<" #comparisonKeys="<<comparisonKeys.size() << endl;
    for(vector<int>::const_iterator k=comparisonKeys.begin(); k!=comparisonKeys.end(); k++) {
      //cout << pthread_self() << ": "<<str()<<" k="<<*k<< endl;
      comparisonTags.push_back(new comparison(txt()<<*k));
    }
  }
  
  void completeComparisons() {
    for(vector<comparison*>::reverse_iterator c=comparisonTags.rbegin(); c!=comparisonTags.rend(); c++)
      delete *c;
  }
  
  std::string str() const {
    ostringstream oss;
    for(vector<int>::const_iterator k=comparisonKeys.begin(); k!=comparisonKeys.end(); k++) {
      if(k!=comparisonKeys.begin()) oss << ":";
      oss << *k;
    }
    return oss.str();
  }
};

void* fib(void* data_arg) {
  fibArg* data = (fibArg*)data_arg;
  data->beginComparisons();
  long ret=0;
  {
    comparison c("-1");
    {
    scope s(txt()<<"Thread: "<<pthread_self(), scope::low);
    dbg << /*pthread_self() << ": "<<data->str()<<*/" fib("<<data->a<<")"<<endl;

    long ret=0;
    if(data->a==1 || data->a==0) {
      dbg << data->str()<<" 1" << endl;
      //cout << pthread_self()<<": Exit"<<endl;
      ret = 1;
      //return (void*)1;
    } else {
      pthread_t thread[2];

      fibArg min1(data->a-1, *data);
      pthread_create(&thread[0], &joinableAttr, fib, (void *)&min1);

      fibArg min2(data->a-2, *data);
      pthread_create(&thread[1], &joinableAttr, fib, (void *)&min2);
      
      void* ret1;
      pthread_join(thread[0], (void**)&ret1);
      ret += (long)ret1;
      void* ret2;
      pthread_join(thread[1], (void**)&ret2);
      ret += (long)ret2;
      dbg << /*data->str()<<" "<<*/ret << endl;
    }
    }
  }
    
  data->completeComparisons();
  //cout << pthread_self()<<": Exit"<<endl;

  //pthread_exit((void*)ret);
  return (void*)ret;
}

int main (int argc, char *argv[])
{
  // The fibonacci number to return
  int a=(argc>=2? atoi(argv[1]): 3);
  
  SightInit(argc, argv, "Pthread Cond Ex", "dbg.pthreadFib.individual");

  pthread_attr_init(&joinableAttr);
  pthread_attr_setdetachstate(&joinableAttr, PTHREAD_CREATE_JOINABLE);
  
  cout << pthread_self()<<": Root thread"<<endl;
  
  fibArg data(a);
  pthread_t thread;
  pthread_create(&thread, &joinableAttr, fib, (void *)&data);
  //long ret = (long)fib(&data);
  void* ret;
  pthread_join(thread, (void**)&ret);
  dbg << (long)ret<<endl;
  cout << pthread_self() << "End"<<endl;
  
  //pthread_exit(NULL);
}

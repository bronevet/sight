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

// Special preprocessor macros that make it easy to turn Sight operations on and off
// at compilation to eliminate its cost during runs that do not need Sight.
#pragma once
//#include "sight.h"

// Applications that wish to disable Sight completely #define DISABLE_SIGHT
#ifdef DISABLE_SIGHT

#define dbg common::nullS
#define SIGHT_VERB_DECL(Type, Args, verbThreshold, verbosity)

#define SIGHT_VERB_DECL_REF(Type, Args, ref, verbThreshold, verbosity)

#define SIGHT_VERB(code, verbThreshold, verbosity)

#define SIGHT_VERB_IF(verbThreshold, verbosity)

#define SIGHT_VERB_FI()

#define SIGHT_DECL(Type, Args, cond)

#define SIGHT_DECL_REF(Type, Args, ref, cond)

#define SIGHT(code, cond)

#define SIGHT_IF(cond)

#define SIGHT_FI()

// Definitions for apps that wish to enable Sight selectively based on specific conditions or
// level of verbosity (the _VERB macros)
#else

template<typename Type>
class SightVerbWrapper {
  Type* instance;
  public:
  SightVerbWrapper(Type* instance): instance(instance) { 
//    if(instance) std::cout << "<<"<<std::endl;
  }
  // Sets ref to point to the instance in this object, if any
  SightVerbWrapper(Type* instance, Type** ref): instance(instance) { 
    *ref = instance;
  }
  Type* get() { return instance; }
  ~SightVerbWrapper() {
    if(instance) {
//      std::cout << ">>" << std::endl;
      delete instance;
    }
  }
};

#define CONCAT2(x, y) x ## y
#define CONCAT(x, y) CONCAT2(x, y)

// Declares a local variable of the given type and with the given constructor argumetns 
// if the verbosity argument reaches the given threshold
#define SIGHT_VERB_DECL(Type, Args, verbThreshold, verbosity) \
SightVerbWrapper<Type> CONCAT(SightVerbWrapperVar, __COUNTER__) ((verbThreshold)<=(verbosity)? new Type Args: (Type*)NULL);

// Declares a local variable of the given type and with the given constructor argumetns 
// if the verbosity argument reaches the given threshold.
// Also, sets ref to point to the created variable
#define SIGHT_VERB_DECL_REF(Type, Args, ref, verbThreshold, verbosity) \
SightVerbWrapper<Type> CONCAT(SightVerbWrapperVar, __COUNTER__) ((verbThreshold)<=(verbosity)? new Type Args: (Type*)NULL, &(ref));

#define SIGHT_VERB(code, verbThreshold, verbosity) \
  if((verbThreshold)<=(verbosity)) { code; }

#define SIGHT_VERB_IF(verbThreshold, verbosity) \
  if((verbThreshold)<=(verbosity)) {

#define SIGHT_VERB_FI() }

// Declares a local variable of the given type and with the given constructor argumetns 
// if the condition argument is true.
#define SIGHT_DECL(Type, Args, cond) \
SightVerbWrapper<Type> CONCAT(SightVerbWrapperVar, __COUNTER__) (cond? new Type Args: (Type*)NULL);

// Declares a local variable of the given type and with the given constructor argumetns 
// if the condition argument is true.
// Also, sets ref to point to the created variable.
#define SIGHT_DECL_REF(Type, Args, ref, cond) \
SightVerbWrapper<Type> CONCAT(SightVerbWrapperVar, __COUNTER__) (cond? new Type Args: (Type*)NULL, &(ref));

#define SIGHT(code, cond) \
  if(cond) { code; }

#define SIGHT_IF(cond) \
  if(cond) {

#define SIGHT_FI() }

// Other aliases
template<typename T>
using shared_ptr = typename  boost::shared_ptr<T>;

#endif


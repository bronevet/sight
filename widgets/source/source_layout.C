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

#include "../../sight_layout.h"
#include "../../sight_common.h"
#include "source_layout.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>

using namespace std;
using namespace sight::common;

namespace sight {
namespace layout {

// Record the layout handlers in this file
void* sourceEnterHandler(properties::iterator props) { return new source(props); }
void  sourceExitHandler(void* obj) { source* s = static_cast<source*>(obj); delete s; }
  
sourceLayoutHandlerInstantiator::sourceLayoutHandlerInstantiator() { 
  (*layoutEnterHandlers)["source"] = &sourceEnterHandler;
  (*layoutExitHandlers) ["source"] = &sourceExitHandler;
}
sourceLayoutHandlerInstantiator sourceLayoutHandlerInstance;

source::source(properties::iterator props) : scope(properties::next(props))
{
  // Keeps track of whether we've included the syntax highlighting scripts required by source
  static bool scriptsIncluded=false;
  if(!scriptsIncluded) {
    // Create the directory that holds the source-specific scripts
    dbg.createWidgetDir("source");
    
    dbg.includeWidgetScript("source/sh_main.js",   "text/javascript");  dbg.includeFile("source/sh_main.js");
    dbg.includeWidgetScript("source/sh_cpp.js",    "text/javascript");  dbg.includeFile("source/sh_cpp.js");
    dbg.includeWidgetScript("source/sh_style.css", "text/css");         dbg.includeFile("source/sh_style.css");
    
    dbg.widgetScriptPrologCommand("sh_highlightDocument();");
    scriptsIncluded = true;
  }
  
  int numRegions = properties::getInt(props, "numRegions");
  for(int i=0; i<numRegions; i++) {
    string fName     = properties::get(props, txt()<<"fName_"<<i);
    string startLine = properties::get(props, txt()<<"startLine_"<<i);
    string endLine   = properties::get(props, txt()<<"endLine_"<<i);

    dbg << "<u>"<<fName<<" ["<<startLine<<" - "<<endLine<<"]</u>"<<endl;

    dbg.ownerAccessing();
    //dbg << "<pre><code class=\"language-c++\">"<<endl;
    dbg << "<pre class=\"sh_cpp\">"<<endl;
    //dbg.userAccessing();

    ifstream sourceFile(fName.c_str());
    assert(sourceFile.is_open());

    string line;
    int lNum=1;
    bool emitting = false;
    int numLinesEmitted = 0;
    while ( getline (sourceFile,line) ) {
      // If we've found the start of the region, start emitting
      if(line.find(txt()<<"#pragma sightLoc "<<startLine) != string::npos)
        emitting = true;
      // If we've found the end of the region, stop emitting
      else if(line.find(txt()<<"#pragma sightLoc "<<endLine) != string::npos)
        emitting = false;
      else if(emitting) {
        if(numLinesEmitted>0) dbg << endl;
        dbg << escape(line);
        numLinesEmitted++;
        /*if(lNum < endLine)
          dbg << endl;*/
      }
      lNum++;
    }
    sourceFile.close(); 
  
    //dbg.ownerAccessing();
    //dbg << "</code></pre>"<<endl;
    dbg << "</pre>"<<endl;
    dbg.userAccessing();
  }
}

// Called to enable the block to print its entry and exit text
void source::printEntry(string loadCmd) {
}

void source::printExit() {
}

}; // namespace layout
}; // namespace sight

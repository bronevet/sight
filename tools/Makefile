##//////////////////////////////////////////////////////////////////////////////
## Copyright (c) 2013, Lawrence Livermore National Security, LLC.
## Produced at the Lawrence Livermore National Laboratory.
## Written by the Greg Bronevetsky <bronevetsky1@llnl.gov> / <greg@bronevetsky.com>. 
##
## LLNL-CODE-642002
## All rights reserved.
##
## This file is part of Sight. For details, see https://e-reports-ext.llnl.gov/pdf/781752.pdf or
## https://github.com/bronevet/sight.
##
## Licensed under the GNU Lesser General Public License (Lesser GPU) Version 2.1, 
## February 1999; you may not use this file except in compliance with the License.  
## The full licence is included in file LICENCE and you may obtain a copy of the 
## License at: 
## https://www.gnu.org/licenses/old-licenses/lgpl-2.1.en.html
##
## Unless required by applicable law or agreed to in writing, software
## distributed under the License is distributed on an "AS IS" BASIS,
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
## implied. See the License for the specific language governing
## permissions and limitations under the license.
##//////////////////////////////////////////////////////////////////////////////

all: dtl callpath boost_1_55_0

dtl-1.17.tar.gz:
	wget --no-check-certificate https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/dtl-cpp/dtl-1.17.tar.gz

dtl: #dtl-1.17.tar.gz
	wget --no-check-certificate https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/dtl-cpp/dtl-1.17.tar.gz
	rm -fr dtl-1.17
	tar -xf dtl-1.17.tar.gz 
	mv dtl-1.17 dtl
	rm dtl-1.17.tar.gz

boost_1_55_0.tar:
	wget http://downloads.sourceforge.net/project/boost/boost/1.55.0/boost_1_55_0.tar.bz2
	rm -f boost_1_55_0.tar
	bunzip2 boost_1_55_0.tar.bz2

boost_1_55_0: #boost_1_55_0.tar
	wget http://downloads.sourceforge.net/project/boost/boost/1.55.0/boost_1_55_0.tar.bz2
	rm -f boost_1_55_0.tar
	bunzip2 boost_1_55_0.tar.bz2
	tar -xf boost_1_55_0.tar
	rm -r boost_1_55_0.tar

adept-utils.zip:
	#wget --no-check-certificate https://github.com/tgamblin/adept-utils/archive/master.zip
	#mv master master.zip
	#mv master.zip adept-utils.zip
	../getGithub https://github.com/tgamblin/adept-utils/archive/master.zip zip unzip master

adept-utils: boost_1_55_0 # adept-utils.zip
	rm -fr adept-utils adept-utils-master
	#wget --no-check-certificate https://github.com/tgamblin/adept-utils/archive/master.zip
	#mv master master.zip
	#mv master.zip adept-utils.zip
	#unzip adept-utils.zip
	../getGithub https://github.com/tgamblin/adept-utils/archive/master.zip zip unzip master
	#rm adept-utils.zip
	mv adept-utils-master adept-utils
#	cp adept-utils-makefiles/CMakeLists.root.txt   adept-utils
#	cp adept-utils-makefiles/CMakeLists.cutils.txt adept-utils/cutils/CMakeLists.txt
#	cp adept-utils-makefiles/CMakeLists.utils.txt  adept-utils/utils/CMakeLists.txt
	cd adept-utils; cmake -DCMAKE_INSTALL_PREFIX=${ROOT_PATH}/tools/adept-utils \
	                      -DCMAKE_C_COMPILER=${CC} -DCMAKE_CXX_COMPILER=${CCC} \
	                      -DBOOST_ROOT:PATHNAME=${ROOT_PATH}/tools/boost_1_55_0 .
	cd adept-utils; make; make install
	
callpath.zip:
	#wget --no-check-certificate https://github.com/tgamblin/callpath/archive/master.zip
	#mv master master.zip
	#mv master.zip callpath.zip
	../getGithub https://github.com/tgamblin/callpath/archive/master.zip zip unzip master

callpath.tar.gz:
	#rm -f callpath.tar.gz*
	wget http://greg.bronevetsky.com/data/callpath.tar.gz
	tar -xf callpath.tar.gz
	
ifeq (${CALLPATH_ENABLED}, 1)
callpath: adept-utils # callpath.tar.gz
	rm -rf callpath-master callpath
	../getGithub https://github.com/tgamblin/callpath/archive/master.zip zip unzip master
	mv callpath-master callpath
	rm -f callpath/CMakeCache.txt callpath/*/CMakeCache.txt
	cp CMakeLists.callpath.txt callpath/CMakeLists.txt
	cp callpath_safe_bool.h callpath/src/safe_bool.h
	cd callpath/src; cmake -DCMAKE_INSTALL_PREFIX=${ROOT_PATH}/tools/callpath \
	                       -DCALLPATH_WALKER=backtrace -DCMAKE_BUILD_TYPE=RelWithDebInfo \
	                       -DCMAKE_CXX_FLAGS=-fpermissive \
	                       -DCMAKE_C_COMPILER=${CC} -DCMAKE_CXX_COMPILER=${CCC} \
	                       -Dadept_utils_DIR=${ROOT_PATH}/tools/adept-utils/share/cmake/adept_utils ..
	cd callpath/src; make; make install
else
callpath:
endif
#-DCALLPATH_HAVE_MPI=false 

clean:
	rm -fr *.tar *.tar.gz *.tar.bz2 dtl adept-utils callpath boost_1_55_0

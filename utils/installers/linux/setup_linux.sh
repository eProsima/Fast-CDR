#!/bin/sh

# This scripts creates a tar.gz file with all the linux installation.
# Also it creates a RPM package.
# @param The version of the project

project="FastCDR"

function installer
{
	# Copy licenses.
	cp ../../../doc/licencias/FASTCDR_LIBRARY_LICENSE.txt tmp/$project
	errorstatus=$?
	if [ $errorstatus != 0 ]; then return; fi
	cp ../../../doc/licencias/LGPLv3_LICENSE.txt tmp/$project
	errorstatus=$?
	if [ $errorstatus != 0 ]; then return; fi

	# Copy CDR headers
	mkdir -p tmp/$project/include
	cp -r ../../../include/cdr tmp/$project/include
	errorstatus=$?
	if [ $errorstatus != 0 ]; then return; fi
        if [ -d tmp/$project/include/cdr/.svn ]; then
            find tmp/$project/include/cdr -iname .svn -exec rm -rf {} \;
        fi

	# Copy eProsima header files
	mkdir -p tmp/$project/include/cdr/eProsima_cpp
	cp $EPROSIMADIR/code/eProsima_cpp/eProsima_auto_link.h tmp/$project/include/cdr/eProsima_cpp
	errorstatus=$?
	if [ $errorstatus != 0 ]; then return; fi

	# Copy CDR sources
	mkdir -p tmp/$project/src
	cp -r ../../../src/cpp tmp/$project/src
	errorstatus=$?
	if [ $errorstatus != 0 ]; then return; fi
        if [ -d tmp/$project/src/cpp/.svn ]; then
            find tmp/$project/src/cpp -iname .svn -exec rm -rf {} \;
        fi

	# Copy autoconf configuration files.
	cp configure.ac tmp/$project
	errorstatus=$?
	if [ $errorstatus != 0 ]; then return; fi
	cp Makefile.am tmp/$project
	errorstatus=$?
	if [ $errorstatus != 0 ]; then return; fi
	cp include_Makefile.am tmp/$project/include/Makefile.am
	errorstatus=$?
	if [ $errorstatus != 0 ]; then return; fi

	# Generate autoconf files
	cd tmp/$project
	sed -i "s/VERSION/${version}/g" configure.ac
	errorstatus=$?
	if [ $errorstatus != 0 ]; then return; fi
	sed -i "s/VERSION_MAJOR/`echo ${version} | cut -d. -f1`/g" Makefile.am
	errorstatus=$?
	if [ $errorstatus != 0 ]; then return; fi
	sed -i "s/VERSION_MINOR/`echo ${version} | cut -d. -f2`/g" Makefile.am
	errorstatus=$?
	if [ $errorstatus != 0 ]; then return; fi
	autoreconf --force --install
	errorstatus=$?
	if [ $errorstatus != 0 ]; then return; fi
	cd ../..

	cd tmp
	tar cvzf "../${project}_${version}.tar.gz" $project
	errorstatus=$?
	cd ..
	if [ $errorstatus != 0 ]; then return; fi
}

if [ $# -lt 1 ]; then
	echo "Needs as parameter the version of the product $project"
	exit -1
fi

version=$1

# Get distro version
. $EPROSIMADIR/scripts/common_pack_functions.sh getDistroVersion

# Create the temporaly directory.
mkdir tmp
mkdir tmp/$project

installer

# Remove temporaly directory
rm -rf tmp

if [ $errorstatus == 0 ]; then
    echo "INSTALLER SUCCESSFULLY"
else
    echo "INSTALLER FAILED"
fi

exit $errorstatus

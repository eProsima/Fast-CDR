#!/bin/sh

# This scripts creates a tar.gz file with all the linux installation.
# Also it creates a RPM package.
# @param The version of the project

# To create RPM in Fedora you have to follot this link:
#   https://fedoraproject.org/wiki/How_to_create_an_RPM_package#Preparing_your_system

project="fastcdr"

function installer
{
	# Copy licenses.
	cp ../../../doc/licencias/FASTCDR_LIBRARY_LICENSE.txt tmp/$project
	errorstatus=$?
	if [ $errorstatus != 0 ]; then return; fi
	cp ../../../doc/licencias/LGPLv3_LICENSE.txt tmp/$project
	errorstatus=$?
	if [ $errorstatus != 0 ]; then return; fi

	# Copy FastCDR headers
	mkdir -p tmp/$project/include
	cp -r ../../../include/fastcdr tmp/$project/include
	errorstatus=$?
	if [ $errorstatus != 0 ]; then return; fi
        if [ -d tmp/$project/include/fastcdr/.svn ]; then
            find tmp/$project/include/fastcdr -iname .svn -exec rm -rf {} \;
        fi

	# Copy eProsima header files
	mkdir -p tmp/$project/include/fastcdr/eProsima_cpp
	cp $EPROSIMADIR/code/eProsima_cpp/eProsima_auto_link.h tmp/$project/include/fastcdr/eProsima_cpp
	errorstatus=$?
	if [ $errorstatus != 0 ]; then return; fi

	# Copy FastCDR sources
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

function rpminstaller
{
	# Copy SPEC file
	sed "s/VERSION/${version}/g" FastCDR.spec > ~/rpmbuild/SPECS/FastCDR.spec
	errorstatus=$?
	if [ $errorstatus != 0 ]; then return; fi

	# Copy source
	cp "${project}_${version}.tar.gz" ~/rpmbuild/SOURCES
	errorstatus=$?
	if [ $errorstatus != 0 ]; then return; fi

	# Go to directory to build.
	cd ~/rpmbuild/SPECS
	errorstatus=$?
	if [ $errorstatus != 0 ]; then return; fi

	# Build command for i686.
	rpmbuild -bb --target i686 FastCDR.spec
	errorstatus=$?
	if [ $errorstatus != 0 ]; then cd -; return; fi

	# Build command for x86_64.
	rpmbuild -bb --target x86_64 FastCDR.spec
	errorstatus=$?
	# Return
	cd -
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

#TODO Check the distro to know if RMP is supported.
[ $errorstatus == 0 ] && { rpminstaller; }

# Remove temporaly directory
rm -rf tmp

if [ $errorstatus == 0 ]; then
    echo "INSTALLER SUCCESSFULLY"
else
    echo "INSTALLER FAILED"
fi

exit $errorstatus

#!/bin/sh

#: This script pack FastCDR product for any platform in Linux.
#
# This script needs the next programs to be run.
# - subversion
# Also this script needs the eProsima.documentation.changeVersion macro installed in the system.
#
# Fedora 19: This script has to be run in a Fedora 17 x64.
#            The system has to have installed: libgcc.i686 and libstdc++.i686

errorstatus=0

function package
{
    # Get current version of GCC. (Not more needed)
    #. $EPROSIMADIR/scripts/common_pack_functions.sh getGccVersion

    # Compile FastCDR library for i86.
    rm -rf output
    EPROSIMA_TARGET="i86Linux2.6gcc"
    rm -r lib/$EPROSIMA_TARGET
    make
    errorstatus=$?
    if [ $errorstatus != 0 ]; then return; fi
    # Compile FastCDR library for x64.
    rm -rf output
    EPROSIMA_TARGET="x64Linux2.6gcc"
    rm -r lib/$EPROSIMA_TARGET
    make
    errorstatus=$?
    if [ $errorstatus != 0 ]; then return; fi

    # Get the current version of FastBuffers
    . $EPROSIMADIR/scripts/common_pack_functions.sh getVersionFromCPP fastcdrversion include/fastcdr/FastCdr_version.h
    errorstatus=$?
    if [ $errorstatus != 0 ]; then return; fi

    # Create installers
    cd utils/installers/linux
    ./setup_linux.sh $fastcdrversion
    errorstatus=$?
    cd ../../..
    if [ $errorstatus != 0 ]; then return; fi

    # Remove the doxygen tmp directory
    rm -rf output
}

# Check that the environment.sh script was run.
if [ "$EPROSIMADIR" == "" ]; then
    echo "environment.sh must to be run."
    exit -1
fi

# Go to root
cd ../..

package

if [ $errorstatus == 0 ]; then
    echo "PACKAGING SUCCESSFULLY"
else
    echo "PACKAGING FAILED"
fi

exit $errorstatus

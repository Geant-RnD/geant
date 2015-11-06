#!/bin/bash -x

export LC_CTYPE=en_US.UTF-8
export LC_ALL=en_US.UTF-8


THIS=$(dirname ${BASH_SOURCE[0]})

# first arguments is the source directory
if [ $# -ge 7 ]; then
  LABEL=$1 ; shift
  COMPILER=$1 ; shift
  BUILDTYPE=$1 ; shift
  EXTERNALS=$1 ; shift
  WORKSPACE=$1 ; shift
  TYPE=$1 ; shift
  BACKEND=$1 ; shift

else
  echo "$0: expecting 7 arguments [LABEL]  [COMPILER] [BUILDTYPE] [EXTERNALS] [WORKSPACE] [TYPE] [BACKEND]"
  return
fi

if [ $LABEL == slc6 ] || [ $LABEL == cc7 ] || [ $LABEL == cuda7 ] || [$LABEL == xeonphi ]

    kinit sftnight@CERN.CH -5 -V -k -t /ec/conf/sftnight.keytab

then
  export PATH=/afs/cern.ch/sw/lcg/contrib/CMake/3.0.0/Linux-i386/bin:${PATH}
else
  export EXTERNALDIR=$HOME/ROOT-externals/
fi

if [[ $COMPILER == *gcc* ]]
then
  gcc47version=4.7
  gcc48version=4.8
  gcc49version=4.9
  COMPILERversion=${COMPILER}version

  ARCH=$(uname -m)
  if [ $LABEL == cuda7 ]
  then
    . /afs/cern.ch/sw/lcg/contrib/gcc/${!COMPILERversion}/${ARCH}-slc6/setup.sh
  else
    . /afs/cern.ch/sw/lcg/contrib/gcc/${!COMPILERversion}/${ARCH}-${LABEL}/setup.sh
  fi
  export FC=gfortran
  export CXX=`which g++`
  export CC=`which gcc`

  export CMAKE_SOURCE_DIR=$WORKSPACE/geant
  export CMAKE_BINARY_DIR=$WORKSPACE/geant/builds
  export CMAKE_BUILD_TYPE=$BUILDTYPE

  export CTEST_BUILD_OPTIONS=" '-DCMAKE_CXX_FLAGS=-O2 -std=c++11' -DUSE_ROOT=ON -DCTEST=ON "
  export CMAKE_INSTALL_PREFIX=$WORKSPACE/geant/installation
  export BACKEND=$BACKEND
  export LD_LIBRARY_PATH=$WORKSPACE/lib:$LD_LIBRARY_PATH

fi

if [[ $COMPILER == *icc* ]]; then

  iccyear=2013
  icc14year=2013
  icc15year=2015
  icc16year=2016
  COMPILERyear=${COMPILER}year

  iccgcc=4.9
  icc14gcc=4.9
  icc15gcc=4.9
  icc16gcc=4.9
  GCCversion=${COMPILER}gcc

  ARCH=$(uname -m)

  . /afs/cern.ch/sw/lcg/contrib/gcc/${!GCCversion}/${ARCH}-slc6/setup.sh
  . /afs/cern.ch/sw/IntelSoftware/linux/setup.sh
  . /afs/cern.ch/sw/IntelSoftware/linux/${ARCH}/xe${!COMPILERyear}/bin/ifortvars.sh intel64
  . /afs/cern.ch/sw/IntelSoftware/linux/${ARCH}/xe${!COMPILERyear}/bin/iccvars.sh intel64
  export CC=icc
  export CXX=icc
  export FC=ifort


  export CMAKE_SOURCE_DIR=$WORKSPACE/geant
  export CMAKE_BINARY_DIR=$WORKSPACE/geant/builds
  export CMAKE_BUILD_TYPE=$BUILDTYPE


  export CTEST_BUILD_OPTIONS=" '-DCMAKE_CXX_FLAGS=-O2 -std=c++11' -DUSE_ROOT=ON -DCTEST=ON "
  export CMAKE_INSTALL_PREFIX=$WORKSPACE/geant/installation
  export BACKEND=$BACKEND
  export LD_LIBRARY_PATH=$WORKSPACE/lib:$LD_LIBRARY_PATH
fi

echo ${THIS}/setup.py -o ${LABEL} -c ${COMPILER} -b ${BUILDTYPE} -v ${EXTERNALS} -w ${WORKSPACE} -t ${TYPE}
eval `${THIS}/setup.py -o ${LABEL} -c ${COMPILER} -b ${BUILDTYPE} -v ${EXTERNALS} -w ${WORKSPACE} -t ${TYPE}`

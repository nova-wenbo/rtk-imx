#!/bin/bash

export WORKDIR="${PWD}"

function compile_lib(){
	echo "start compile shared lib ... "
	if [ ! -d $WORKDIR/lib ];then
		mkdir -p $WORKDIR/lib
	fi
	cd $WORKDIR/common/cJson && make > null
	cd $WORKDIR/common/debuglog && make > null
	cd $WORKDIR/common/fifo && make > null
	cd $WORKDIR/common/serialport && make > null
	cd $WORKDIR
}

function compile_obj(){
	echo "start compile executable program ... "
	if [ ! -d $WORKDIR/obj ];then
                mkdir -p $WORKDIR/obj
        fi
	cd $WORKDIR/ctd && make > null
	cd $WORKDIR/rtks && make > null
	cd $WORKDIR/rtkc && make > null
	cd $WORKDIR
}
function compile_all(){
	compile_lib
	compile_obj
	echo "compile end !!!"
}
function compile_clean(){
	rm $WORKDIR/lib -rf
	rm $WORKDIR/obj -rf
}
function choose_build_module(){
        if   [ "$1" == "-lib" ]; then
                compile_lib
        elif [ "$1" == "-app" ]; then
                compile_obj
	elif [ "$1" == "-all" ]; then
                compile_all
        elif [ "$1" == "-clean" ]; then
                compile_clean
        else
                echo " usage : <$0 [-lib | -app | -all | -clean >"
        fi
}

choose_build_module $1


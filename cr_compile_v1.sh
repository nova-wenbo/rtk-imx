#!/bin/bash

export WORKDIR="${PWD}"

function compile_lib(){
	echo "start compile shared lib ... "
	if [ ! -d $WORKDIR/lib ];then
		mkdir -p $WORKDIR/lib
	fi
	cd $WORKDIR/common/cJson && make 
	cd $WORKDIR/common/debuglog && make 
	cd $WORKDIR/common/fifo && make 
	cd $WORKDIR/common/serialport && make 
	cd $WORKDIR/common/websocket && make 
	cp $WORKDIR/include/* /usr/include/
	cp $WORKDIR/lib/* /lib/ 
	cd $WORKDIR
}

function compile_obj(){
	echo "start compile executable program ... "
	if [ ! -d $WORKDIR/obj ];then
                mkdir -p $WORKDIR/obj
        fi
	cd $WORKDIR/ctd/gps && make
	cd $WORKDIR/ctd/gyr && make
	cd $WORKDIR/ctd/temp && make
	cd $WORKDIR/rtks && make 
	cd $WORKDIR/rtkc && make 
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
	find $WORKDIR -name null |xargs rm -rf
	find $WORKDIR -name .*swp |xargs rm -rf
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


#!/bin/bash
DIR=${1:-.}

cp $DIR/global_for_wasmedge.img _globinst_0_value.img
cp $DIR/memory_page_count_for_wasmedge.img _meminst_0_memtype.img
cp $DIR/memory_data_for_wasmedge.img _meminst_0_dataptr.img
cp $DIR/stack_for_wasmedge.img stackmgr_value.img
cp $DIR/frame_for_wasmedge.img stackmgr_frame.img
cp $DIR/iter_for_wasmedge.img iter.img

##############
# JustGarble #
##############
input arguments
---------------
num_inputs: number of inputs for the logsum operation
num_bits: number of bits representing each of the inputs
scale_num_bits: defines range of inputs --> [-2^(scale_num_bits-1),2^(scale_num_bits-1)]
                WARNING: only look-up tables for 'scale_num_bits=3' are provided
num_entries: number of segments used for the linear piecewise approximation

compile JustGarble toolkit
--------------------------
1) follow instructions provided by the JustGarble toolkit

compile the logsum program
----------------------
1) go to directory './logsum__JustGarble/justGarble/portelo/logsum/'
2) run the following commands:
      gcc -O3 -lm -lrt -lpthread -maes -msse4 -lmsgpack -march=native -std=gnu99 -fgnu89-inline -I ../../include -I../CIRCUIT_OPS ../../obj/*.o ../CIRCUIT_OPS/*.c logsum_RQ.c -o logsum_RQ.out
      gcc -O3 -lm -lrt -lpthread -maes -msse4 -lmsgpack -march=native -std=gnu99 -fgnu89-inline -I ../../include -I../CIRCUIT_OPS ../../obj/*.o ../CIRCUIT_OPS/*.c logsum_SQ.c -o logsum_SQ.out
   NOTE: some warnings are expected

execute the logsum program
--------------------------
1) go to directory './logsum__JustGarble/justGarble/portelo/logsum/'
2) compact execution --> generates random inputs, computes the real value of the logsum and the values obtained with the RQ and SQ approach
      num_inputs=2; num_bits=8; scale_num_bits=3; num_entries=32; perl logsum__initialization.pl $num_inputs $num_bits $scale_num_bits; logsum_GC_scaled=`./logsum_RQ.out $num_inputs $num_entries $num_bits $scale_num_bits -1 -1 1000000 | grep "  i:   0 ; outputs: " | sed 's/ //g' | cut -d ':' -f 3 | cut -d '|' -f 1`; logsum_GC=`echo "$logsum_GC_scaled/2^($num_bits-$scale_num_bits)" | bc -l`; echo "  logsum_RQ:  $logsum_GC --> $logsum_GC_scaled"; logsum_GC_scaled=`./logsum_SQ.out $num_inputs $num_entries $num_bits $scale_num_bits -1 -1 1000000 | grep "  i:   0 ; outputs: " | sed 's/ //g' | cut -d ':' -f 3 | cut -d '|' -f 1`; logsum_GC=`echo "$logsum_GC_scaled/2^($num_bits-$scale_num_bits)" | bc -l`; echo "  logsum_SQ:  $logsum_GC --> $logsum_GC_scaled"
3) detailed execution --> computes the logsum and shows the execution times required by each block
      num_inputs=2; num_bits=8; scale_num_bits=3; num_entries=32; ./logsum_RQ.out $num_inputs $num_entries $num_bits $scale_num_bits -1 -1 1000000
      num_inputs=2; num_bits=8; scale_num_bits=3; num_entries=32; ./logsum_SQ.out $num_inputs $num_entries $num_bits $scale_num_bits -1 -1 1000000

##########
# MATLAB #
##########
input arguments
---------------
num_inputs: number of inputs for the logsum operation
num_bits: number of bits representing each of the inputs
scale_num_bits: defines range of inputs --> [-2^(scale_num_bits-1),2^(scale_num_bits-1)]
                WARNING: only look-up tables for 'scale_num_bits=3' are provided
num_segments: number of segments used for the linear piecewise approximation
num_samples: number of times the logsum is to be computed --> error analysis

execute the program
-------------------
1) open MATLAB
2) go to directory './logsum__MATLAB/'
3) run the following command:
      num_inputs=8; num_bits=8; scale_num_bits=3; num_segments=32; num_samples=1000; logsum_trials(num_inputs,num_bits,scale_num_bits,num_segments,num_samples)
      NOTE: a summary of the error analysis will be stored at './logsum__MATLAB/logsum_error/'

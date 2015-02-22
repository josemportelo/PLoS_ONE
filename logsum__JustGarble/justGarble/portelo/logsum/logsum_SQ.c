/*
 * logsum_SQ.c
 *
 *  Created on: 08 Jul 2013
 *      Author: portelo
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

// #include "../../include/garble.h"
// #include "../../include/common.h"
// #include "../../include/circuits.h"
// #include "../../include/gates.h"
// #include "../../include/util.h"
#include "../../include/justGarble.h"

#include "../CIRCUIT_OPS/io_ops.h"
#include "../CIRCUIT_OPS/comparison_ops.h"
#include "../CIRCUIT_OPS/linear_ops.h"
#include "../CIRCUIT_OPS/logsum_ops.h"


struct timespec clock_gettime_diff(struct timespec start, struct timespec end) {
  struct timespec temp;
  
  if ((end.tv_nsec-start.tv_nsec)<0) {
    temp.tv_sec=end.tv_sec-start.tv_sec-1;
    temp.tv_nsec=1000000000+end.tv_nsec-start.tv_nsec;
  } else {
    temp.tv_sec=end.tv_sec-start.tv_sec;
    temp.tv_nsec=end.tv_nsec-start.tv_nsec;
  }
  
  return temp;
}


int main(int argc, char **argv) {
  
  srand(time(NULL));
  GarbledCircuit garbledCircuit;
  GarblingContext garblingContext;
  
  
  int num_inputs__values=atoi(argv[1]);  // num_inputs__values (number of input values for the logsum)
  int num_inputs__table=atoi(argv[2]);   // num_inputs__table (number of entries of the look-up table for the logsum)
  int num_wires_input=atoi(argv[3]);     // num_wires_input (n) (number of bits representing all the inputs)
  int scale_num_bits=atoi(argv[4]);      // scale_num_bits (defines range of inputs --> [-2^(scale_num_bits-1),2^(scale_num_bits-1)])
  int num_outputs;//=atoi(argv[5]);      // num_outputs (number of outputs of the logsum --> num_outputs=1)
  int num_wires_output;//=atoi(argv[6]); // num_wires_output (m) (defined by the logsum --> num_wires_output=num_wires_input+ceil(log2(num_inputs__values)))
  int num_gates=atoi(argv[7]);           // num_gates (q) (upper bound for the number of gates --> use a sufficiently large number)
  
  num_outputs=1;
  int extra_output_wires=(int)ceil(log(num_inputs__values)/log(2));
  num_wires_output=num_wires_input+extra_output_wires;
  
  int *input_wires__values,*input_wires__table,*output_wires;
  int num_wires_t=(num_inputs__table-1)*num_wires_input,num_wires_n=num_inputs__table*num_wires_input;
  int *inputs_dec__values,*inputs_dec__table_t,*inputs_dec__table_n,
        *inputs_bin__values,*inputs_bin__table_t,*inputs_bin__table_n,*inputs_bin,*outputs_dec,*outputs_bin;
  char file__lookup_table_t[500],file__lookup_table_n[500];
  
  int total_num_wires_input__values=num_inputs__values*num_wires_input;                 // total_num_wires_input__values (n_values)
  int total_num_wires_input__table=num_wires_t+num_wires_n;                             // total_num_wires_input__table (n_table=n_table_t+n_table_n)
  int total_num_wires_input=total_num_wires_input__values+total_num_wires_input__table; // total_num_wires_input (n=n_values+n_table)
  int total_num_wires_output=num_outputs*num_wires_output;                              // total_num_wires_output (m)
  int total_num_wires=total_num_wires_input+num_gates;                                  // total_num_wires=total_num_wires_input+num_gates (r=n+q)
  
  // execution time
  struct timeval tv1__build_circuit,tv2__build_circuit,tv1__write_circuit,tv2__write_circuit,
        tv1__read_inputs,tv2__read_inputs,
        tv1__garble_circuit,tv2__garble_circuit,tv1__eval_circuit,tv2__eval_circuit;
  struct timespec time1__build_circuit,time2__build_circuit,time1__write_circuit,time2__write_circuit,
        time1__read_inputs,time2__read_inputs,
        time1__garble_circuit,time2__garble_circuit,time1__eval_circuit,time2__eval_circuit,
        diff__build_circuit,diff__write_circuit,diff__read_inputs,diff__garble_circuit,diff__eval_circuit;
  
  // setup input and output tokens/labels
  block *labels=(block*) malloc(sizeof(block)*(2*total_num_wires_input));    // size: 2*n
  block *outputbs=(block*) malloc(sizeof(block)*(2*total_num_wires_output)); // size: 2*m
  
  OutputMap outputMap=outputbs;
  block computedOutputMap[total_num_wires_output]; // size: m
  InputLabels inputLabels=labels;
  block extractedLabels[total_num_wires_input];    // size: n
  
  
  // allocate memory for inputs/outputs
  input_wires__values=(int*) malloc(sizeof(int)*total_num_wires_input__values);
  input_wires__table=(int*) malloc(sizeof(int)*total_num_wires_input__table);
  output_wires=(int*) malloc(sizeof(int)*total_num_wires_output);
  
  inputs_dec__values=(int*) malloc(sizeof(int)*num_inputs__values);            // size: n_values/num_wires_input
  inputs_dec__table_t=(int*) malloc(sizeof(int)*(num_inputs__table-1));        // size: n_table_t/num_wires_input
  inputs_dec__table_n=(int*) malloc(sizeof(int)*num_inputs__table);            // size: n_table_n/num_wires_input
  inputs_bin__values=(int*) malloc(sizeof(int)*total_num_wires_input__values); // size: n_values
  inputs_bin__table_t=(int*) malloc(sizeof(int)*num_wires_t);                  // size: n_table_t
  inputs_bin__table_n=(int*) malloc(sizeof(int)*num_wires_n);                  // size: n_table_n
  inputs_bin=(int*) malloc(sizeof(int)*total_num_wires_input);                 // size: n
  outputs_dec=(int*) malloc(sizeof(int)*num_outputs);                          // size: m/num_wires_output
  outputs_bin=(int*) malloc(sizeof(int)*total_num_wires_output);               // size: m
  
  // input/output info
  printf("### Input/Output info ###\n");
  printf("  num_inputs__values: %d ; num_inputs__table: %d ; num_outputs: %d\n",num_inputs__values,num_inputs__table,num_outputs);
  printf("  num_wires_t: %d ; num_wires_n: %d\n",num_wires_t,num_wires_n);
  printf("  total_num_wires_input__values: %d ; total_num_wires_input__table: %d ; total_num_wires_output: %d ; num_gates: %d ; total_num_wires: %d\n",
        total_num_wires_input__values,total_num_wires_input__table,total_num_wires_output,num_gates,total_num_wires);
  
  // enumerate input wires
  printf("### Input wires ###\n");
  printf("  --- values ---\n");
  for (int i=0; i<num_inputs__values; i++) {
    for (int j=0; j<num_wires_input; j++) {
      input_wires__values[i*num_wires_input+j]=i*num_wires_input+j;
      //printf("    i: %3d ; j: %3d ; input_wires__values: %3d\n",i,j,input_wires__values[i*num_wires_input+j]);
    }
  }
  
  printf("  --- look-up table: t ---\n");
  for (int i=0; i<num_inputs__table-1; i++) {
    for (int j=0; j<num_wires_input; j++) {
      input_wires__table[i*num_wires_input+j]=(total_num_wires_input__values)+i*num_wires_input+j;
      //printf("    i: %3d ; j: %3d ; input_wires__table: %3d\n",i,j,input_wires__table[i*num_wires_input+j]);
    }
  }
  printf("  --- look-up table: n ---\n");
  for (int i=0; i<num_inputs__table; i++) {
    for (int j=0; j<num_wires_input; j++) {
       input_wires__table[(num_wires_t)+i*num_wires_input+j]=(total_num_wires_input__values)+(num_wires_t)+i*num_wires_input+j;
       //printf("    i: %3d ; j: %3d ; input_wires__table: %3d\n",i,j,input_wires__table[(num_wires_t)+i*num_wires_input+j]);
    }
  }
  
  // create a circuit
  gettimeofday(&tv1__build_circuit,NULL);
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&time1__build_circuit);
  
  //   pre-processing
  createInputLabels(labels,total_num_wires_input); // size arg1: 2*n, size arg2: n
  createEmptyGarbledCircuit(&garbledCircuit,total_num_wires_input,total_num_wires_output,num_gates,total_num_wires,inputLabels);
  startBuilding(&garbledCircuit,&garblingContext);
  
  //   circuit definition
  //Logsum_SQ(&garbledCircuit,&garblingContext,2*num_wires_input,input_wires__values,
  //      num_inputs__table,input_wires__table,output_wires,0);
  MultipleLogsum_SQ(&garbledCircuit,&garblingContext,num_inputs__values,num_wires_input,input_wires__values,
        num_inputs__table,input_wires__table,output_wires,0);
  
  //   post-processing
  finishBuilding(&garbledCircuit,&garblingContext,outputMap,output_wires);
  
  gettimeofday(&tv2__build_circuit,NULL);
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&time2__build_circuit);
  
  // write the circuit to file
  gettimeofday(&tv1__write_circuit,NULL);
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&time1__write_circuit);
  
  writeCircuitToFile(&garbledCircuit,"logsum_SQ.scd");
  
  gettimeofday(&tv2__write_circuit,NULL);
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&time2__write_circuit);
  
  // show the created circuit
  //ShowGarbledCircuit(&garbledCircuit);
// // output info
// printf("  --- output wires ---\n");
// for (int i=0; i<num_outputs; i++) {
//   for (int j=0; j<num_wires_output; j++) {
//     printf("    i: %d ; j: %3d ; output_wires: %6d\n",i,j,garbledCircuit.outputs[i*num_wires_output+j]);
//   }
//   printf("\n");
// }
  
  // read the inputs from file (convert from decimal to binary)
  printf("### Inputs ###\n");
  
  gettimeofday(&tv1__read_inputs,NULL);
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&time1__read_inputs);
  
  //   values
  ReadInputs("input_values.txt",num_inputs__values,inputs_dec__values);
  DEC2BIN(num_inputs__values,inputs_dec__values,num_wires_input,inputs_bin__values);
  
  for (int i=0; i<num_inputs__values; i++) {
    for (int j=0; j<num_wires_input; j++) {
      inputs_bin[i*num_wires_input+j]=inputs_bin__values[i*num_wires_input+j];
    }
  }
  
  //   look-up table: t
  sprintf(file__lookup_table_t,"look-up_table/t.num_segments_%ld.scaling_factor_%ld.num_bits_%ld.scale_num_bits_%ld.txt",
        num_inputs__table,(long int)pow(2,num_wires_input)/8,num_wires_input,scale_num_bits);
  ReadInputs(file__lookup_table_t,num_inputs__table-1,inputs_dec__table_t);
  DEC2BIN(num_inputs__table-1,inputs_dec__table_t,num_wires_input,inputs_bin__table_t);
  
  for (int i=0; i<num_inputs__table-1; i++) {
    for (int j=0; j<num_wires_input; j++) {
      inputs_bin[(total_num_wires_input__values)+i*num_wires_input+j]=inputs_bin__table_t[i*num_wires_input+j];
    }
  }
  
  //   look-up table: n
  sprintf(file__lookup_table_n,"look-up_table/n_SQ.num_segments_%ld.scaling_factor_%ld.num_bits_%ld.scale_num_bits_%ld.txt",
        num_inputs__table,(long int)pow(2,num_wires_input)/8,num_wires_input,scale_num_bits);
  ReadInputs(file__lookup_table_n,num_inputs__table,inputs_dec__table_n);
  DEC2BIN(num_inputs__table,inputs_dec__table_n,num_wires_input,inputs_bin__table_n);
  
  for (int i=0; i<num_inputs__table; i++) {
    for (int j=0; j<num_wires_input; j++) {
      inputs_bin[(total_num_wires_input__values)+(num_wires_t)+i*num_wires_input+j]=inputs_bin__table_n[i*num_wires_input+j];
    }
  }
  
  gettimeofday(&tv2__read_inputs,NULL);
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&time2__read_inputs);
  
//   printf("  --- values ---\n");
//   for (int i=0; i<num_inputs__values; i++) {
//     printf("    i: %3d ; inputs:  %6d --> ",i,inputs_dec__values[i]);
//     for (int j=0; j<num_wires_input; j++) {
//       printf("%d",inputs_bin[i*num_wires_input+((num_wires_input-1)-j)]);
//     }
//     printf("\n");
//   }
//   
//   printf("  --- look-up table: t ---\n");
//   for (int i=0; i<num_inputs__table-1; i++) {
//     printf("    i: %3d ; inputs:  %6d --> ",i,inputs_dec__table_t[i]);
//     for (int j=0; j<num_wires_input; j++) {
//       printf("%d",inputs_bin[(total_num_wires_input__values)+i*num_wires_input+((num_wires_input-1)-j)]);
//     }
//     printf("\n");
//   }
//   
//   printf("  --- look-up table: n ---\n");
//   for (int i=0; i<num_inputs__table; i++) {
//     printf("    i: %3d ; inputs:  %6d --> ",i,inputs_dec__table_n[i]);
//     for (int j=0; j<num_wires_input; j++) {
//       printf("%d",inputs_bin[(total_num_wires_input__values)+(num_wires_t)+i*num_wires_input+((num_wires_input-1)-j)]);
//     }
//     printf("\n");
//   }
  
  // garble the circuit
  gettimeofday(&tv1__garble_circuit,NULL);
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&time1__garble_circuit);
  
  garbleCircuit(&garbledCircuit,inputLabels,outputMap);
  
  gettimeofday(&tv2__garble_circuit,NULL);
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&time2__garble_circuit);
  
  // evaluate the circuit
  gettimeofday(&tv1__eval_circuit,NULL);
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&time1__eval_circuit);
  
  extractLabels(extractedLabels,inputLabels,inputs_bin,total_num_wires_input);
  evaluate(&garbledCircuit,extractedLabels,computedOutputMap);
  mapOutputs(outputMap,computedOutputMap,outputs_bin,total_num_wires_output);
  
  gettimeofday(&tv2__eval_circuit,NULL);
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&time2__eval_circuit);
  
  // display the results (convert from binary to decimal)
  //BIN2DEC(num_outputs,num_wires_output,outputs_bin,outputs_dec);
  BIN2DECc(num_outputs,num_wires_output,outputs_bin,outputs_dec);
  
  printf("### Output values ###\n");
  for (int i=0; i<num_outputs; i++) {
    printf("  i: %3d ; outputs: %6d | ",i,outputs_dec[i]);
    for (int j=0; j<num_wires_output; j++) {
      printf("%d",outputs_bin[i*num_wires_output+((num_wires_output-1)-j)]);
    }
    printf("\n");
  }
  
  // execution time
  printf("### Execution time ###\n");
  
  printf("  --- gettimeofday() ---\n");
  printf("    Build circuit:    %f seconds\n",
        (double)(tv2__build_circuit.tv_sec-tv1__build_circuit.tv_sec)+
        (double)(tv2__build_circuit.tv_usec-tv1__build_circuit.tv_usec)/1000000);
  printf("    Write circuit:    %f seconds\n",
        (double)(tv2__write_circuit.tv_sec-tv1__write_circuit.tv_sec)+
        (double)(tv2__write_circuit.tv_usec-tv1__write_circuit.tv_usec)/1000000);
  printf("    Read inputs:      %f seconds\n",
        (double)(tv2__read_inputs.tv_sec-tv1__read_inputs.tv_sec)+
        (double)(tv2__read_inputs.tv_usec-tv1__read_inputs.tv_usec)/1000000);
  printf("    Garble circuit:   %f seconds\n",
        (double)(tv2__garble_circuit.tv_sec-tv1__garble_circuit.tv_sec)+
        (double)(tv2__garble_circuit.tv_usec-tv1__garble_circuit.tv_usec)/1000000);
  printf("    Evaluate circuit: %f seconds\n",
        (double)(tv2__eval_circuit.tv_sec-tv1__eval_circuit.tv_sec)+
        (double)(tv2__eval_circuit.tv_usec-tv1__eval_circuit.tv_usec)/1000000);
  printf("    TOTAL TIME:       %f seconds\n",
        ((double)(tv2__build_circuit.tv_sec-tv1__build_circuit.tv_sec)+
        (double)(tv2__build_circuit.tv_usec-tv1__build_circuit.tv_usec)/1000000)+
        ((double)(tv2__write_circuit.tv_sec-tv1__write_circuit.tv_sec)+
        (double)(tv2__write_circuit.tv_usec-tv1__write_circuit.tv_usec)/1000000)+
        ((double)(tv2__read_inputs.tv_sec-tv1__read_inputs.tv_sec)+
        (double)(tv2__read_inputs.tv_usec-tv1__read_inputs.tv_usec)/1000000)+
        ((double)(tv2__garble_circuit.tv_sec-tv1__garble_circuit.tv_sec)+
        (double)(tv2__garble_circuit.tv_usec-tv1__garble_circuit.tv_usec)/1000000)+
        ((double)(tv2__eval_circuit.tv_sec-tv1__eval_circuit.tv_sec)+
        (double)(tv2__eval_circuit.tv_usec-tv1__eval_circuit.tv_usec)/1000000));
  
  printf("  --- clock_gettime() ---\n");
  diff__build_circuit=clock_gettime_diff(time1__build_circuit,time2__build_circuit);
  diff__write_circuit=clock_gettime_diff(time1__write_circuit,time2__write_circuit);
  diff__read_inputs=clock_gettime_diff(time1__read_inputs,time2__read_inputs);
  diff__garble_circuit=clock_gettime_diff(time1__garble_circuit,time2__garble_circuit);
  diff__eval_circuit=clock_gettime_diff(time1__eval_circuit,time2__eval_circuit);
  printf("    Build circuit:    %f seconds\n",
        (double)(diff__build_circuit.tv_sec)+(double)(diff__build_circuit.tv_nsec)/1000000000);
  printf("    Write circuit:    %f seconds\n",
        (double)(diff__write_circuit.tv_sec)+(double)(diff__write_circuit.tv_nsec)/1000000000);
  printf("    Read inputs:      %f seconds\n",
        (double)(diff__read_inputs.tv_sec)+(double)(diff__read_inputs.tv_nsec)/1000000000);
  printf("    Garble circuit:   %f seconds\n",
        (double)(diff__garble_circuit.tv_sec)+(double)(diff__garble_circuit.tv_nsec)/1000000000);
  printf("    Evaluate circuit: %f seconds\n",
        (double)(diff__eval_circuit.tv_sec)+(double)(diff__eval_circuit.tv_nsec)/1000000000);
  printf("    TOTAL TIME:       %f seconds\n",
        ((double)(diff__build_circuit.tv_sec)+(double)(diff__build_circuit.tv_nsec)/1000000000)+
        ((double)(diff__write_circuit.tv_sec)+(double)(diff__write_circuit.tv_nsec)/1000000000)+
        ((double)(diff__read_inputs.tv_sec)+(double)(diff__read_inputs.tv_nsec)/1000000000)+
        ((double)(diff__garble_circuit.tv_sec)+(double)(diff__garble_circuit.tv_nsec)/1000000000)+
        ((double)(diff__eval_circuit.tv_sec)+(double)(diff__eval_circuit.tv_nsec)/1000000000));
  
  return 0;
}

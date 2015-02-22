/*
 * io_ops.c
 *
 *  Created on: 01 Jul 2013
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

#include "io_ops.h"


int InitializeWireZERO(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext, int dummy_wire) {
  
  int zero_wire;
  
  
  zero_wire=getNextWire(garblingContext);
  XORGate(garbledCircuit,garblingContext,dummy_wire,dummy_wire,zero_wire); // XOR(a,a)=0

  return zero_wire;
}

int InitializeWireONE(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext, int dummy_wire) {
  
  int not__dummy_wire,one_wire;
  
  
  not__dummy_wire=getNextWire(garblingContext);
  one_wire=getNextWire(garblingContext);
  NOTGate(garbledCircuit,garblingContext,dummy_wire,not__dummy_wire);
  XORGate(garbledCircuit,garblingContext,dummy_wire,not__dummy_wire,one_wire); // XOR(a,~a)=1

  return one_wire;
}


void BIN2DEC(int num_elements, int num_wires_bin, int *inputs_bin, int *outputs_dec) {
  
  for (int i=0; i<num_elements; i++) {
    outputs_dec[i]=0;
    for (int j=0; j<num_wires_bin; j++) {
      // position '0': least significant bit ; position 'n-1': most significant bit
      outputs_dec[i]=outputs_dec[i]+inputs_bin[i*num_wires_bin+j]*pow(2,j);
    }
  }
  
  return;
}

void BIN2DECc(int num_elements, int num_wires_bin, int *inputs_bin, int *outputs_dec) {
  
  for (int i=0; i<num_elements; i++) {
    outputs_dec[i]=0;
    for (int j=0; j<num_wires_bin; j++) {
      // position '0': least significant bit ; position 'n-1': most significant bit
      outputs_dec[i]=outputs_dec[i]+inputs_bin[i*num_wires_bin+j]*pow(2,j);
    }
    
    // negative number, adjust using two's complement
    if (inputs_bin[i*num_wires_bin+(num_wires_bin-1)]==1) {
      outputs_dec[i]=outputs_dec[i]-pow(2,num_wires_bin);
    }
  }
  
  return;
}

void DEC2BIN(int num_elements, int *inputs_dec, int num_wires_bin, int *outputs_bin) {
  
  int input_dec_tmp;
  
  
  for (int i=0; i<num_elements; i++) {
    for (int j=num_wires_bin-1; j>=0; j--) {
      // position '0': least significant bit ; position 'n-2': most significant bit ; position 'n-1': sign
      outputs_bin[i*num_wires_bin+j]=(inputs_dec[i]>>j)&1; // 0 or 1
    }
  }
  
  return;
}


int ReadInputs(char *input_file_name, int num_inputs, int *inputs_dec) {
  
  FILE *fid_inputs;
  char line[500];
  int cont;
  
  
  cont=0;
  fid_inputs=fopen(input_file_name,"rt");
  while(fgets(line,500,fid_inputs)!=NULL && cont<num_inputs) {
    sscanf(line,"%d",&inputs_dec[cont]);
    
    cont++;
  }
  fclose(fid_inputs);
  
  return 0;
}

int ShowGarbledCircuit(GarbledCircuit *garbledCircuit) {
  
  int num_wires_input=garbledCircuit->n;
  int num_wires_output=garbledCircuit->m;
  int num_gates=garbledCircuit->q;
  int total_num_wires=garbledCircuit->r;
  GarbledGate *garbledGate;
  char *gate_type;
  
  
  printf("### Circuit description ###\n");
  
  // general info
  printf("  --- general info ---\n");
  printf("    num_wires_input: %d ; num_wires_output: %d ; num_gates: %d ; total_num_wires: %d\n",
        num_wires_input,num_wires_output,num_gates,total_num_wires);
  
  // gate info
  printf("  --- gate info ---\n");
  for (int k=0; k<num_gates; k++) {
    garbledGate=&(garbledCircuit->garbledGates[k]);
    
    switch (garbledGate->type) {
      case 8:  gate_type="AND"; break;
      case 14: gate_type=" OR"; break;
      case 6:  gate_type="XOR"; break;
      case 5:  gate_type="NOT"; break;
      default: gate_type="???"; break;
    }
    
    printf("    k: %8d ; id: %8d ; gate_type: %2d (%s) ; input0: %8ld ; input1: %8ld ; output: %8ld\n",
          k,garbledGate->id,garbledGate->type,gate_type,garbledGate->input0,garbledGate->input1,garbledGate->output);
  }
  
  // output info
  printf("  --- output wires ---\n");
  for (int j=0; j<num_wires_output; j++) {
    printf("    j: %3d ; output_wires: %8d\n",j,garbledCircuit->outputs[j]);
  }
  
  return 0;
}

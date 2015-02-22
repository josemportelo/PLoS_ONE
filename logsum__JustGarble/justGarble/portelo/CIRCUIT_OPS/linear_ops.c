/*
 * linear_ops.c
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

#include "linear_ops.h"
#include "io_ops.h"


int ADD(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_wires_input_x2, int* input_wires, int* output_wires) {
  
  int input_tmp[3],output_tmp[2];
  int num_wires_input=num_wires_input_x2/2; // 'input_wires': input1 | input2
  
  
  // compute the 1st bit (no carry bit)
  input_tmp[0]=input_wires[0];
  input_tmp[1]=input_wires[num_wires_input];
  
  ADD22Circuit(garbledCircuit,garblingContext,input_tmp,output_tmp);
  output_wires[0]=output_tmp[0]; // least significant bit
  
  // compute the remaining bits (use carry bit from previous operation)
  for (int j=1; j<num_wires_input; j++) {
    input_tmp[0]=output_tmp[1];
    input_tmp[1]=input_wires[j];
    input_tmp[2]=input_wires[num_wires_input+j];
    
    ADD32Circuit(garbledCircuit,garblingContext,input_tmp,output_tmp);
    output_wires[j]=output_tmp[0];
  }
  output_wires[num_wires_input]=output_tmp[1]; // most significant bit
  
  return 0;
}

int MultipleADD(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_inputs, int num_wires_input, int* input_wires, int* output_wires) {
  
  int num_wires_output=num_wires_input+1;
  int input_tmp[2*num_wires_input],output_tmp[num_wires_output],*new_input_wires,wire_tmp;
  int odd_number_of_inputs;
  
  
  if (num_inputs==1) {
    for (int j=0; j<num_wires_input; j++) {
      output_wires[j]=input_wires[j];
    }
    output_wires[num_wires_output-1]=InitializeWireZERO(garbledCircuit,garblingContext,input_wires[0]); // increase number of bits by 1 --> zero-padding
    
    return 0;
  }
  
  if (num_inputs==2) {
    ADD(garbledCircuit,garblingContext,2*num_wires_input,input_wires,output_wires);
    
    return 0;
  }
  
  if (num_inputs%2==1) {
    odd_number_of_inputs=1;
  } else {
    odd_number_of_inputs=0;
  }
  
  new_input_wires=(int*) malloc(sizeof(int)*((num_inputs/2+odd_number_of_inputs)*num_wires_output));
  for (int i=0; i<num_inputs/2; i++) {
    for (int j=0; j<num_wires_input; j++) {
      input_tmp[j]=input_wires[(2*i)*num_wires_input+j];
      input_tmp[num_wires_input+j]=input_wires[(2*i+1)*num_wires_input+j];
    }
    
    ADD(garbledCircuit,garblingContext,2*num_wires_input,input_tmp,output_tmp);
    
    for (int j=0; j<num_wires_output; j++) {
      new_input_wires[i*num_wires_output+j]=output_tmp[j];
    }
  }
  
  if (odd_number_of_inputs==1) {
    for (int j=0; j<num_wires_input; j++) {
      new_input_wires[num_inputs/2*num_wires_output+j]=input_wires[(num_inputs-1)*num_wires_input+j];
    }
    new_input_wires[num_inputs/2*num_wires_output+(num_wires_output-1)]=
          new_input_wires[num_inputs/2*num_wires_output+(num_wires_input-1)]; // increase number of bits by 1, keep sign
  }
  
  MultipleADD(garbledCircuit,garblingContext,num_inputs/2+odd_number_of_inputs,num_wires_output,new_input_wires,output_wires);
  
  free(new_input_wires);
  
  return 0;
}

int ADDc(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_wires_input_x2, int* input_wires, int* output_wires) {
  
  int num_wires_input=num_wires_input_x2/2; // 'input_wires': input1 | input2
  
  
  ADD(garbledCircuit,garblingContext,2*num_wires_input,input_wires,output_wires);
  
  return 0;
}

int MultipleADDc(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_inputs, int num_wires_input, int* input_wires, int* output_wires) {
  
  int num_wires_output=num_wires_input+1;
  int input_tmp[2*num_wires_input],output_tmp[num_wires_output],*new_input_wires,wire_tmp;
  int odd_number_of_inputs;
  
  
  if (num_inputs==1) {
    for (int j=0; j<num_wires_input; j++) {
      output_wires[j]=input_wires[j];
    }
    output_wires[num_wires_output-1]=InitializeWireZERO(garbledCircuit,garblingContext,input_wires[0]); // increase number of bits by 1 --> zero-padding
    
    return 0;
  }
  
  if (num_inputs==2) {
    ADDc(garbledCircuit,garblingContext,2*num_wires_input,input_wires,output_wires);
    
    return 0;
  }
  
  if (num_inputs%2==1) {
    odd_number_of_inputs=1;
  } else {
    odd_number_of_inputs=0;
  }
  
  new_input_wires=(int*) malloc(sizeof(int)*((num_inputs/2+odd_number_of_inputs)*num_wires_output));
  for (int i=0; i<num_inputs/2; i++) {
    for (int j=0; j<num_wires_input; j++) {
      input_tmp[j]=input_wires[(2*i)*num_wires_input+j];
      input_tmp[num_wires_input+j]=input_wires[(2*i+1)*num_wires_input+j];
    }
    
    ADDc(garbledCircuit,garblingContext,2*num_wires_input,input_tmp,output_tmp);
    
    for (int j=0; j<num_wires_output; j++) {
      new_input_wires[i*num_wires_output+j]=output_tmp[j];
    }
  }
  
  if (odd_number_of_inputs==1) {
    for (int j=0; j<num_wires_input; j++) {
      new_input_wires[num_inputs/2*num_wires_output+j]=input_wires[(num_inputs-1)*num_wires_input+j];
    }
    new_input_wires[num_inputs/2*num_wires_output+(num_wires_output-1)]=
         new_input_wires[num_inputs/2*num_wires_output+(num_wires_input-1)]; // increase number of bits by 1, keep sign
  }
  
  MultipleADDc(garbledCircuit,garblingContext,num_inputs/2+odd_number_of_inputs,num_wires_output,new_input_wires,output_wires);
  
  free(new_input_wires);
  
  return 0;
}

int SUB32(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext, int* inputs, int* outputs) {
  
  int wire1=getNextWire(garblingContext);
  int wire2=getNextWire(garblingContext);
  int wire3=getNextWire(garblingContext);
  int wire4=getNextWire(garblingContext);
  int wire5=getNextWire(garblingContext);
  int wire6=getNextWire(garblingContext);
  
  
  XORGate(garbledCircuit,garblingContext,inputs[2],inputs[0],wire1);
  XORGate(garbledCircuit,garblingContext,inputs[1],inputs[0],wire2);
  XORGate(garbledCircuit,garblingContext,inputs[2],wire2,wire3);
  NOTGate(garbledCircuit,garblingContext,wire3,wire4);
  ANDGate(garbledCircuit,garblingContext,wire1,wire2,wire5);
  XORGate(garbledCircuit,garblingContext,inputs[2],wire5,wire6);
  
  outputs[0]=wire4;
  outputs[1]=wire6;
  
  return 0;
}

int SUB(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_wires_input_x2, int* input_wires, int* output_wires) {
  
  int input_tmp[3],output_tmp[2];
  int num_wires_input=num_wires_input_x2/2; // 'input_wires': input1 | input2
  int not__output_tmp;
  
  
  // compute the 1st bit (carry bit equal to '1')
  input_tmp[0]=InitializeWireONE(garbledCircuit,garblingContext,input_wires[0]); // initialize as '1'
  input_tmp[2]=input_wires[0];
  input_tmp[1]=input_wires[num_wires_input];
  
  SUB32(garbledCircuit,garblingContext,input_tmp,output_tmp);
  output_wires[0]=output_tmp[0]; // least significant bit
  
  // compute the remaining bits (use carry bit from previous operation)
  for (int j=1; j<num_wires_input; j++) {
    input_tmp[0]=output_tmp[1];
    input_tmp[2]=input_wires[j];
    input_tmp[1]=input_wires[num_wires_input+j];
    
    SUB32(garbledCircuit,garblingContext,input_tmp,output_tmp);
    output_wires[j]=output_tmp[0];
  }
  
  // flip the most significant bit
  not__output_tmp=getNextWire(garblingContext);
  NOTGate(garbledCircuit,garblingContext,output_tmp[1],not__output_tmp);
  output_wires[num_wires_input]=not__output_tmp;
  
  return 0;
}

int SUBc(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_wires_input_x2, int* input_wires, int* output_wires) {
  
  int num_wires_input=num_wires_input_x2/2; // 'input_wires': input1 | input2
  int expand__input_wires[2*(num_wires_input+1)];
  int not__input_wires[num_wires_input+1],twos_complement__input_wires[num_wires_input+1];
  int new_input_wires[2*(num_wires_input+1)],new_output_wires[(num_wires_input+1)+1];
  
  
  for (int j=0; j<num_wires_input; j++) {
    expand__input_wires[j]=input_wires[j];
    expand__input_wires[(num_wires_input+1)+j]=input_wires[num_wires_input+j];
  }
  expand__input_wires[num_wires_input]=input_wires[num_wires_input-1];
  expand__input_wires[2*num_wires_input+1]=input_wires[2*num_wires_input-1];
  
  // convert 2nd input using two's complement
  for (int j=0; j<num_wires_input+1; j++) {
    not__input_wires[j]=getNextWire(garblingContext);
    NOTGate(garbledCircuit,garblingContext,expand__input_wires[(num_wires_input+1)+j],not__input_wires[j]);
  }
  INCCircuit(garbledCircuit,garblingContext,num_wires_input+1,not__input_wires,twos_complement__input_wires);
  
  for (int j=0; j<num_wires_input+1; j++) {
    new_input_wires[j]=expand__input_wires[j];
    new_input_wires[(num_wires_input+1)+j]=twos_complement__input_wires[j];
  }
  
  // input1-input2 = input1+two's_complement(input2)
  ADD(garbledCircuit,garblingContext,2*(num_wires_input+1),new_input_wires,new_output_wires);
  for (int j=0; j<num_wires_input+1; j++) {
    output_wires[j]=new_output_wires[j];
  }
  
  return 0;
}

// binary multiplier technique: http://en.wikipedia.org/wiki/Binary_multiplier
int MUL(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_wires_input_x2, int* input_wires, int* output_wires) {
  
  int num_wires_input=num_wires_input_x2/2; // 'input_wires': input1 | input2
  int bitwiseAND[num_wires_input][num_wires_input];
  int input_ADD[2*(num_wires_input+1)],output_ADD[(num_wires_input+1)+1];
  
  
  // compute bitwise AND's
  for (int j1=0; j1<num_wires_input; j1++) {
    for (int j2=0; j2<num_wires_input; j2++) {
      bitwiseAND[j1][j2]=getNextWire(garblingContext);
      ANDGate(garbledCircuit,garblingContext,input_wires[j1],input_wires[num_wires_input+j2],bitwiseAND[j1][j2]);
    }
  }
  
  // compute shifted additions
  //   first
  for (int j2=0; j2<num_wires_input; j2++) {
    input_ADD[j2]=bitwiseAND[0][j2];
    input_ADD[(num_wires_input+1)+(j2+1)]=bitwiseAND[1][j2];
  }
  input_ADD[num_wires_input]=InitializeWireZERO(garbledCircuit,garblingContext,input_wires[0]); // initialize as '0'
  input_ADD[num_wires_input+1]=InitializeWireZERO(garbledCircuit,garblingContext,input_wires[0]); // initialize as '0'
  
  ADD(garbledCircuit,garblingContext,2*(num_wires_input+1),input_ADD,output_ADD);
  output_wires[0]=output_ADD[0];
  
  //   iterate
  for (int j1=1; j1<num_wires_input-1; j1++) {
    for (int j2=0; j2<num_wires_input; j2++) {
      input_ADD[j2]=output_ADD[j2+1];
      input_ADD[(num_wires_input+1)+(j2+1)]=bitwiseAND[j1+1][j2];
    }
    input_ADD[num_wires_input]=output_ADD[num_wires_input+1];
    input_ADD[num_wires_input+1]=InitializeWireZERO(garbledCircuit,garblingContext,input_wires[0]); // initialize as '0'
    
    ADD(garbledCircuit,garblingContext,2*(num_wires_input+1),input_ADD,output_ADD);
    output_wires[j1]=output_ADD[0];
  }
  
  //   get remaining bits
  for (int j1=1; j1<num_wires_input+2; j1++) {
    output_wires[num_wires_input-2+j1]=output_ADD[j1];
  }
  
  return 0;
}

// binary multiplier technique: http://en.wikipedia.org/wiki/Binary_multiplier
int MULc(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_wires_input_x2, int* input_wires, int* output_wires) {
  
  int num_wires_input=num_wires_input_x2/2; // 'input_wires': input1 | input2
  int bitwiseAND[num_wires_input][num_wires_input],twos_complement__bitwiseAND[num_wires_input][num_wires_input+1];
  int input_ADD[2*((num_wires_input+1)+1)],output_ADD[(num_wires_input+1)+2];
  
  
  // compute bitwise AND's
  for (int j1=0; j1<num_wires_input; j1++) {
    for (int j2=0; j2<num_wires_input; j2++) {
      bitwiseAND[j1][j2]=getNextWire(garblingContext);
      ANDGate(garbledCircuit,garblingContext,input_wires[j1],input_wires[num_wires_input+j2],bitwiseAND[j1][j2]);
    }
  }
  
  // adjust bitwise AND's to two's complement
  //   first
  for (int j2=0; j2<num_wires_input-1; j2++) {
    twos_complement__bitwiseAND[0][j2]=bitwiseAND[0][j2];
  }
  twos_complement__bitwiseAND[0][num_wires_input-1]=getNextWire(garblingContext);
  NOTGate(garbledCircuit,garblingContext,bitwiseAND[0][num_wires_input-1],twos_complement__bitwiseAND[0][num_wires_input-1]);
  twos_complement__bitwiseAND[0][num_wires_input]=InitializeWireONE(garbledCircuit,garblingContext,bitwiseAND[0][0]); // initialize as '1'

  //   iterate
  for (int j1=1; j1<num_wires_input-1; j1++) {
    for (int j2=0; j2<num_wires_input-1; j2++) {
      twos_complement__bitwiseAND[j1][j2]=bitwiseAND[j1][j2];
    }
    twos_complement__bitwiseAND[j1][num_wires_input-1]=getNextWire(garblingContext);
    NOTGate(garbledCircuit,garblingContext,bitwiseAND[j1][num_wires_input-1],twos_complement__bitwiseAND[j1][num_wires_input-1]);
    twos_complement__bitwiseAND[j1][num_wires_input]=InitializeWireZERO(garbledCircuit,garblingContext,bitwiseAND[0][0]); // initialize as '0'
  }

  // last
  for (int j2=0; j2<num_wires_input-1; j2++) {
    twos_complement__bitwiseAND[num_wires_input-1][j2]=getNextWire(garblingContext);
    NOTGate(garbledCircuit,garblingContext,bitwiseAND[num_wires_input-1][j2],twos_complement__bitwiseAND[num_wires_input-1][j2]);
  }
  twos_complement__bitwiseAND[num_wires_input-1][num_wires_input-1]=bitwiseAND[num_wires_input-1][num_wires_input-1];
  twos_complement__bitwiseAND[num_wires_input-1][num_wires_input]=InitializeWireONE(garbledCircuit,garblingContext,bitwiseAND[0][0]); // initialize as '1'
  
  // compute shifted additions
  //   first
  input_ADD[(num_wires_input+1)+1]=InitializeWireZERO(garbledCircuit,garblingContext,input_wires[0]); // initialize as '0'
  for (int j2=0; j2<num_wires_input+1; j2++) {
    input_ADD[j2]=twos_complement__bitwiseAND[0][j2];
    input_ADD[((num_wires_input+1)+1)+(j2+1)]=twos_complement__bitwiseAND[1][j2];
  }
  input_ADD[num_wires_input+1]=InitializeWireZERO(garbledCircuit,garblingContext,input_wires[0]); // initialize as '0'
  
  ADD(garbledCircuit,garblingContext,2*((num_wires_input+1)+1),input_ADD,output_ADD);
  output_wires[0]=output_ADD[0];
  
  //   iterate
  for (int j1=1; j1<num_wires_input-1; j1++) {
    input_ADD[(num_wires_input+1)+1]=InitializeWireZERO(garbledCircuit,garblingContext,input_wires[0]); // initialize as '0'
    for (int j2=0; j2<num_wires_input+1; j2++) {
      input_ADD[j2]=output_ADD[j2+1];
      input_ADD[((num_wires_input+1)+1)+(j2+1)]=twos_complement__bitwiseAND[j1+1][j2];
    }
    input_ADD[num_wires_input+1]=output_ADD[(num_wires_input+1)+1];
    
    ADD(garbledCircuit,garblingContext,2*((num_wires_input+1)+1),input_ADD,output_ADD);
    output_wires[j1]=output_ADD[0];
  }
  
  //   get remaining bits
  for (int j1=1; j1<num_wires_input+2; j1++) {
    output_wires[num_wires_input-2+j1]=output_ADD[j1];
  }
  
  return 0;
}

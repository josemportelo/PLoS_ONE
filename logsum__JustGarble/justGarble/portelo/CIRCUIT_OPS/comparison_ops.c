/*
 * comparison_ops.c
 *
 *  Created on: 03 Jul 2013
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

#include "comparison_ops.h"
#include "io_ops.h"
#include "linear_ops.h"


int bitMUX(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int* input_wire_1, int* input_wire_2, int* control_wire, int* output_wire) {
  
  int wire1=getNextWire(garblingContext);
  int wire2=getNextWire(garblingContext);
  int wire3=getNextWire(garblingContext);
  
  
  XORGate(garbledCircuit,garblingContext,input_wire_2[0],input_wire_1[0],wire1);
  ANDGate(garbledCircuit,garblingContext,wire1,control_wire[0],wire2);
  XORGate(garbledCircuit,garblingContext,input_wire_2[0],wire2,wire3);
  
  output_wire[0]=wire3;
  
  return 0;
}

int MUX(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_wires_input_x2, int* input_wires, int* control_wire, int* output_wires) {
  
  int num_wires_input=num_wires_input_x2/2; // 'input_wires': input1 | input2
  
  
  for (int j=0; j<num_wires_input; j++) {
    bitMUX(garbledCircuit,garblingContext,&input_wires[j],&input_wires[num_wires_input+j],&control_wire[0],&output_wires[j]);
  }
  
  return 0;
}

int MultipleMUX(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_inputs, int num_wires_input, int* input_wires, int* control_wires, int* output_wires) {
  
  int *new_input_wires_1,*new_input_wires_2,*new_control_wires_1,*new_control_wires_2,
        *new_output_wires_1,*new_output_wires_2,*new_output_wires;
  
  
  // trivial cases
  //   should not be necessary, since 2^N inputs are expected
  if (num_inputs==1) {
    for (int j=0; j<num_wires_input; j++) {
      output_wires[j]=input_wires[j];
    }

    return 0;
  }
  
  //   normal situation, since 2^N inputs are expected
  if (num_inputs==2) {
    MUX(garbledCircuit,garblingContext,2*num_wires_input,input_wires,control_wires,output_wires);
    
    return 0;
  }
  
  // iterative MUX cascade
  new_input_wires_1=(int*) malloc(sizeof(int)*((num_inputs/2)*num_wires_input));
  new_input_wires_2=(int*) malloc(sizeof(int)*((num_inputs/2)*num_wires_input));
  new_control_wires_1=(int*) malloc(sizeof(int)*(num_inputs/2)-1);
  new_control_wires_2=(int*) malloc(sizeof(int)*(num_inputs/2)-1);
  new_output_wires_1=(int*) malloc(sizeof(int)*((num_inputs/2)*num_wires_input));
  new_output_wires_2=(int*) malloc(sizeof(int)*((num_inputs/2)*num_wires_input));
  new_output_wires=(int*) malloc(sizeof(int)*(2*num_wires_input));
  
  for (int i=0; i<num_inputs/2; i++) {
    for (int j=0; j<num_wires_input; j++) {
      new_input_wires_1[i*num_wires_input+j]=input_wires[i*num_wires_input+j];
      new_input_wires_2[i*num_wires_input+j]=input_wires[(num_inputs/2+i)*num_wires_input+j];
    }
  }
  
  for (int i=0; i<num_inputs/2-1; i++) {
    new_control_wires_1[i]=control_wires[i];
    new_control_wires_2[i]=control_wires[(num_inputs/2)+i];
  }
  
  MultipleMUX(garbledCircuit,garblingContext,num_inputs/2,num_wires_input,new_input_wires_1,new_control_wires_1,new_output_wires_1);
  MultipleMUX(garbledCircuit,garblingContext,num_inputs/2,num_wires_input,new_input_wires_2,new_control_wires_2,new_output_wires_2);
  
  for (int j=0; j<num_wires_input; j++) {
    new_output_wires[j]=new_output_wires_1[j];
    new_output_wires[num_wires_input+j]=new_output_wires_2[j];
  }

  MUX(garbledCircuit,garblingContext,2*num_wires_input,new_output_wires,&control_wires[num_inputs/2-1],output_wires);
  
  free(new_input_wires_1);
  free(new_input_wires_2);
  free(new_control_wires_1);
  free(new_control_wires_2);
  free(new_output_wires_1);
  free(new_output_wires_2);
  free(new_output_wires);
  
  return 0;
}


int bitCMP(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int* input_wire_1, int* input_wire_2, int* control_wire, int* output_wire) {
  
  int wire1=getNextWire(garblingContext);
  int wire2=getNextWire(garblingContext);
  int wire3=getNextWire(garblingContext);
  int wire4=getNextWire(garblingContext);
  
  
  XORGate(garbledCircuit,garblingContext,input_wire_2[0],control_wire[0],wire1);
  XORGate(garbledCircuit,garblingContext,input_wire_1[0],control_wire[0],wire2);
  ANDGate(garbledCircuit,garblingContext,wire1,wire2,wire3);
  XORGate(garbledCircuit,garblingContext,input_wire_2[0],wire3,wire4);
  
  output_wire[0]=wire4;

  return 0;
}

int CMP(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_wires_input_x2, int* input_wires, int* control_wire, int* output_wires) {
  
  int num_wires_input=num_wires_input_x2/2; // 'input_wires': input1 | input2
  int old_cmp_wire,new_cmp_wire;
  
  
  for (int j=0; j<num_wires_input; j++) {
    if (j==0) {
      old_cmp_wire=control_wire[0];
    } else {
      old_cmp_wire=new_cmp_wire;
    }
    
    new_cmp_wire=getNextWire(garblingContext);
    
    bitCMP(garbledCircuit,garblingContext,&input_wires[j],&input_wires[num_wires_input+j],&old_cmp_wire,&new_cmp_wire);
  }
  
  output_wires[0]=new_cmp_wire;
  
  return 0;
}

int LT(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_wires_input_x2, int* input_wires, int* output_wires) {
  
  int control_wire;
  
  
  control_wire=InitializeWireZERO(garbledCircuit,garblingContext,input_wires[0]); // initialize as '0'
  CMP(garbledCircuit,garblingContext,num_wires_input_x2,input_wires,&control_wire,output_wires);
  
  return 0;
}

int LEQ(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_wires_input_x2, int* input_wires, int* output_wires) {
  
  int control_wire;
  
  
  control_wire=InitializeWireONE(garbledCircuit,garblingContext,input_wires[0]); // initialize as '1'
  CMP(garbledCircuit,garblingContext,num_wires_input_x2,input_wires,&control_wire,output_wires);
  
  return 0;
}

int GT(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_wires_input_x2, int* input_wires, int* output_wires) {
  
  int control_wire;
  int inputs_tmp[num_wires_input_x2];
  
  // reverse order of inputs
  for (int j=0; j<num_wires_input_x2/2; j++) {
    inputs_tmp[j]=input_wires[num_wires_input_x2/2+j];
    inputs_tmp[num_wires_input_x2/2+j]=input_wires[j];
  }
  
  control_wire=InitializeWireZERO(garbledCircuit,garblingContext,input_wires[0]); // initialize as '0'
  CMP(garbledCircuit,garblingContext,num_wires_input_x2,inputs_tmp,&control_wire,output_wires);
  
  return 0;
}

int GEQ(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_wires_input_x2, int* input_wires, int* output_wires) {
  
  int control_wire;
  int inputs_tmp[num_wires_input_x2];
  
  // reverse order of inputs
  for (int j=0; j<num_wires_input_x2/2; j++) {
    inputs_tmp[j]=input_wires[num_wires_input_x2/2+j];
    inputs_tmp[num_wires_input_x2/2+j]=input_wires[j];
  }
  
  control_wire=InitializeWireONE(garbledCircuit,garblingContext,input_wires[0]); // initialize as '1'
  CMP(garbledCircuit,garblingContext,num_wires_input_x2,inputs_tmp,&control_wire,output_wires);
  
  return 0;
}


int MIN(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_wires_input_x2, int* input_wires, int* output_wires) {
  
  int wire_cmp,output_cmp;
  
  
  wire_cmp=InitializeWireZERO(garbledCircuit,garblingContext,input_wires[0]); // initialize as '0'
  CMP(garbledCircuit,garblingContext,num_wires_input_x2,input_wires,&wire_cmp,&output_cmp);
  
  MUX(garbledCircuit,garblingContext,num_wires_input_x2,input_wires,&output_cmp,output_wires);
  
  return 0;
}

int MINc(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_wires_input_x2, int* input_wires, int* output_wires) {
  
  int output_sub[num_wires_input_x2/2+1],sign_sub;
  
  
  SUBc(garbledCircuit,garblingContext,num_wires_input_x2,input_wires,output_sub);
  sign_sub=output_sub[num_wires_input_x2/2];
  
  MUX(garbledCircuit,garblingContext,num_wires_input_x2,input_wires,&sign_sub,output_wires);
  
  return 0;
}

// int MultipleMIN(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
//       int num_inputs, int num_wires_input, int* input_wires, int* output_wires) {
//   
//   // ### NOT IMPLEMENTED YET ###
//   
//   return 0;
// }

int MultipleMINc(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_inputs, int num_wires_input, int* input_wires, int* output_wires) {
  
  int input_tmp[2*num_wires_input],output_tmp[num_wires_input],*new_input_wires,wire_tmp;
  int odd_number_of_inputs;
  
  
  if (num_inputs==1) {
    for (int j=0; j<num_wires_input; j++) {
      output_wires[j]=input_wires[j];
    }
    
    return 0;
  }
  
  if (num_inputs==2) {
    MINc(garbledCircuit,garblingContext,2*num_wires_input,input_wires,output_wires);
    
    return 0;
  }
  
  if (num_inputs%2==1) {
    odd_number_of_inputs=1;
  } else {
    odd_number_of_inputs=0;
  }
  
  new_input_wires=(int*) malloc(sizeof(int)*((num_inputs/2+odd_number_of_inputs)*num_wires_input));
  for (int i=0; i<num_inputs/2; i++) {
    for (int j=0; j<num_wires_input; j++) {
      input_tmp[j]=input_wires[(2*i)*num_wires_input+j];
      input_tmp[num_wires_input+j]=input_wires[(2*i+1)*num_wires_input+j];
    }
    
    MINc(garbledCircuit,garblingContext,2*num_wires_input,input_tmp,output_tmp);
    
    for (int j=0; j<num_wires_input; j++) {
      new_input_wires[i*num_wires_input+j]=output_tmp[j];
    }
  }
  
  if (odd_number_of_inputs==1) {
    for (int j=0; j<num_wires_input; j++) {
      new_input_wires[num_inputs/2*num_wires_input+j]=input_wires[(num_inputs-1)*num_wires_input+j];
    }
  }
  
  MultipleMINc(garbledCircuit,garblingContext,num_inputs/2+odd_number_of_inputs,num_wires_input,new_input_wires,output_wires);
  
  free(new_input_wires);
  
  return 0;
}

int MAX(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_wires_input_x2, int* input_wires, int* output_wires) {
  
  int wire_cmp,output_cmp;
  int inputs_tmp[num_wires_input_x2];
  
  
  wire_cmp=InitializeWireZERO(garbledCircuit,garblingContext,input_wires[0]); // initialize as '0'
  CMP(garbledCircuit,garblingContext,num_wires_input_x2,input_wires,&wire_cmp,&output_cmp);
  
  // reverse order of inputs
  for (int j=0; j<num_wires_input_x2/2; j++) {
    inputs_tmp[j]=input_wires[num_wires_input_x2/2+j];
    inputs_tmp[num_wires_input_x2/2+j]=input_wires[j];
  }
  
  MUX(garbledCircuit,garblingContext,num_wires_input_x2,inputs_tmp,&output_cmp,output_wires);
  
  return 0;
}

int MAXc(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_wires_input_x2, int* input_wires, int* output_wires) {
  
  int output_sub[num_wires_input_x2/2+1],sign_sub;
  int inputs_tmp[num_wires_input_x2];
  
  
  SUBc(garbledCircuit,garblingContext,num_wires_input_x2,input_wires,output_sub);
  sign_sub=output_sub[num_wires_input_x2/2];
  
  // reverse order of inputs
  for (int j=0; j<num_wires_input_x2/2; j++) {
    inputs_tmp[j]=input_wires[num_wires_input_x2/2+j];
    inputs_tmp[num_wires_input_x2/2+j]=input_wires[j];
  }
  
  MUX(garbledCircuit,garblingContext,num_wires_input_x2,inputs_tmp,&sign_sub,output_wires);
  
  return 0;
}

// int MultipleMAX(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
//       int num_inputs, int num_wires_input, int* input_wires, int* output_wires) {
//   
//   // ### NOT IMPLEMENTED YET ###
//   
//   return 0;
// }

int MultipleMAXc(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_inputs, int num_wires_input, int* input_wires, int* output_wires) {
  
  int input_tmp[2*num_wires_input],output_tmp[num_wires_input],*new_input_wires,wire_tmp;
  int odd_number_of_inputs;
  
  
  if (num_inputs==1) {
    for (int j=0; j<num_wires_input; j++) {
      output_wires[j]=input_wires[j];
    }
    
    return 0;
  }
  
  if (num_inputs==2) {
    MAXc(garbledCircuit,garblingContext,2*num_wires_input,input_wires,output_wires);
    
    return 0;
  }
  
  if (num_inputs%2==1) {
    odd_number_of_inputs=1;
  } else {
    odd_number_of_inputs=0;
  }
  
  new_input_wires=(int*) malloc(sizeof(int)*((num_inputs/2+odd_number_of_inputs)*num_wires_input));
  for (int i=0; i<num_inputs/2; i++) {
    for (int j=0; j<num_wires_input; j++) {
      input_tmp[j]=input_wires[(2*i)*num_wires_input+j];
      input_tmp[num_wires_input+j]=input_wires[(2*i+1)*num_wires_input+j];
    }
    
    MAXc(garbledCircuit,garblingContext,2*num_wires_input,input_tmp,output_tmp);
    
    for (int j=0; j<num_wires_input; j++) {
      new_input_wires[i*num_wires_input+j]=output_tmp[j];
    }
  }
  
  if (odd_number_of_inputs==1) {
    for (int j=0; j<num_wires_input; j++) {
      new_input_wires[num_inputs/2*num_wires_input+j]=input_wires[(num_inputs-1)*num_wires_input+j];
    }
  }
  
  MultipleMAXc(garbledCircuit,garblingContext,num_inputs/2+odd_number_of_inputs,num_wires_input,new_input_wires,output_wires);
  
  free(new_input_wires);
  
  return 0;
}

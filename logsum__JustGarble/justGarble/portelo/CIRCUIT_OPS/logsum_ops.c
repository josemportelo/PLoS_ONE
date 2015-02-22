/*
 * logsum_ops.c
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

#include "logsum_ops.h"
#include "io_ops.h"
#include "linear_ops.h"
#include "comparison_ops.h"


//-----------------------
// ramp quantization (RQ)
//-----------------------
int Logsum_RQ(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_wires_input_x2, int* input_wires__values,
      int num_inputs__table, int* input_wires__table, int* output_wires, int iteration) {
  
  int num_wires_input=num_wires_input_x2/2;
  int num_wires_input_x2_2=(num_wires_input+1)+(num_wires_input+1);
  int max_inputs[num_wires_input],min_inputs[num_wires_input];
  int input_sub_Mm[num_wires_input_x2],sub_Mm[num_wires_input+1],sub_Mm_pos[num_wires_input],
        input_sub_mM[num_wires_input_x2],sub_mM[num_wires_input+1];
  int input_cmp[num_wires_input_x2],control_wire,output_cmp[num_inputs__table-1];
  int input_mux_n1[num_inputs__table*num_wires_input],input_mux_n2[num_inputs__table*num_wires_input_x2],
        n1x[num_wires_input],n2x[num_wires_input_x2];
  int input_mul_lt[num_wires_input_x2_2],output_mul_lt[num_wires_input_x2_2],
        input_mul_add_lt[2*num_wires_input_x2_2],output_mul_add_lt[num_wires_input_x2_2+1],
        input_final_add[num_wires_input_x2],output_final_add[num_wires_input+1];
  int num_wires_t=(num_inputs__table-1)*num_wires_input,
        num_wires_n1=num_inputs__table*num_wires_input,num_wires_n2=num_inputs__table*num_wires_input_x2;
  
  
  // get value for search in look-up table
  MAXc(garbledCircuit,garblingContext,num_wires_input_x2,input_wires__values,max_inputs);
  MINc(garbledCircuit,garblingContext,num_wires_input_x2,input_wires__values,min_inputs);
  
  //   'sub_Mm' instead of 'sub_mM' for accessing the look-up table, because 0 < sub_Mm < +inf
  for (int j=0; j<num_wires_input; j++) {
    input_sub_Mm[j]=max_inputs[j];
    input_sub_Mm[num_wires_input+j]=min_inputs[j];
  }
  SUBc(garbledCircuit,garblingContext,num_wires_input_x2,input_sub_Mm,sub_Mm);
  
  //   remove sign bit (always '0')
  for (int j=0; j<num_wires_input; j++) {
    sub_Mm_pos[j]=sub_Mm[j];
  }
  
  // get indexes for search in look-up table
  control_wire=InitializeWireZERO(garbledCircuit,garblingContext,input_wires__values[0]); // initialize as '0'
  for (int j=0; j<num_wires_input; j++) {
    input_cmp[num_wires_input+j]=sub_Mm_pos[j];
  }
  for (int i=0; i<num_inputs__table-1; i++) {
    for (int j=0; j<num_wires_input; j++) {
      input_cmp[j]=input_wires__table[i*num_wires_input+j]; // t
    }
    CMP(garbledCircuit,garblingContext,num_wires_input_x2,input_cmp,&control_wire,&output_cmp[i]);
  }
  
  // retrieve the correct values from the look-up table
  for (int i=0; i<num_inputs__table; i++) {
    for (int j=0; j<num_wires_input; j++) {
      input_mux_n1[i*num_wires_input+j]=input_wires__table[(num_wires_t)+i*num_wires_input+j]; // n1
    }
  }
  for (int i=0; i<num_inputs__table; i++) {
    for (int j=0; j<num_wires_input_x2; j++) {
      input_mux_n2[i*num_wires_input_x2+j]=input_wires__table[(num_wires_t+num_wires_n1)+i*num_wires_input_x2+j]; // n2
    }
  }
  MultipleMUX(garbledCircuit,garblingContext,num_inputs__table,num_wires_input,input_mux_n1,output_cmp,n1x);
  MultipleMUX(garbledCircuit,garblingContext,num_inputs__table,num_wires_input_x2,input_mux_n2,output_cmp,n2x);
  
  // compute approximation for the logsum
  //   mN-mX
  for (int j=0; j<num_wires_input; j++) {
    input_sub_mM[j]=min_inputs[j];
    input_sub_mM[num_wires_input+j]=max_inputs[j];
  }
  SUBc(garbledCircuit,garblingContext,num_wires_input_x2,input_sub_mM,sub_mM);
  
  //   n1*(mN-mX)
  for (int j=0; j<num_wires_input+1; j++) {
    input_mul_lt[j]=sub_mM[j];
  }
  for (int j=0; j<num_wires_input; j++) {
    input_mul_lt[(num_wires_input+1)+j]=n1x[j];
  }
  input_mul_lt[(num_wires_input+1)+num_wires_input]=InitializeWireZERO(garbledCircuit,garblingContext,input_wires__table[0]); // increase number of bits by 1 --> zero-padding
  MULc(garbledCircuit,garblingContext,num_wires_input_x2_2,input_mul_lt,output_mul_lt);
  
  //   n1*(mN-mX)+n2
  for (int j=0; j<num_wires_input_x2_2; j++) {
    input_mul_add_lt[j]=output_mul_lt[j];
  }
  for (int j=0; j<num_wires_input_x2; j++) {
    input_mul_add_lt[num_wires_input_x2_2+j]=n2x[j];
  }
  input_mul_add_lt[num_wires_input_x2_2+num_wires_input_x2]=InitializeWireZERO(garbledCircuit,garblingContext,input_wires__table[0]); // increase number of bits by 1 --> zero-padding
  input_mul_add_lt[num_wires_input_x2_2+num_wires_input_x2+1]=InitializeWireZERO(garbledCircuit,garblingContext,input_wires__table[0]); // increase number of bits by 1 --> zero-padding
  ADDc(garbledCircuit,garblingContext,2*num_wires_input_x2_2,input_mul_add_lt,output_mul_add_lt);
  
  //   mX+(n1*(mN-mX)+n2)
  for (int j=0; j<num_wires_input; j++) {
    input_final_add[j]=max_inputs[j];
    input_final_add[num_wires_input+j]=output_mul_add_lt[num_wires_input+j]; // remove 'num_wires_input' LSB ; ignore extra MSB
  }
  ADDc(garbledCircuit,garblingContext,num_wires_input_x2,input_final_add,output_final_add);
  
  for (int j=0; j<num_wires_input; j++) {
    output_wires[j]=output_final_add[j];
  }
  output_wires[num_wires_input]=output_final_add[num_wires_input-1];
  
  return 0;
}

int MultipleLogsum_RQ(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_inputs__values, int num_wires_input, int* input_wires__values,
      int num_inputs__table, int* input_wires__table, int* output_wires, int iteration) {
  
  int num_wires_output=num_wires_input+1;
  int num_wires_t=(num_inputs__table-1)*num_wires_input,num_wires_n1=num_inputs__table*num_wires_input,num_wires_n2=num_inputs__table*(2*num_wires_input),
        new_num_wires_t=(num_inputs__table-1)*num_wires_output,new_num_wires_n1=num_inputs__table*num_wires_output,new_num_wires_n2=num_inputs__table*(2*num_wires_output);
  int input_tmp[2*num_wires_input],output_tmp[num_wires_output],
        *new_input_wires__values,new_input_wires__table[new_num_wires_t+new_num_wires_n1+new_num_wires_n2];
  int odd_number_of_inputs;
  
  
  // number of inputs = 1 --> return input with additional MSB
  if (num_inputs__values==1) {
    for (int j=0; j<num_wires_input; j++) {
      output_wires[j]=input_wires__values[j];
    }
    output_wires[num_wires_output-1]=input_wires__values[num_wires_input-1]; // increase number of bits by 1
    
    return 0;
  }
  
  // number of elements = 2 --> compute logsum of 2 elements each
  if (num_inputs__values==2) {
    Logsum_RQ(garbledCircuit,garblingContext,2*num_wires_input,input_wires__values,
          num_inputs__table,input_wires__table,output_wires,iteration);
    
    return 0;
  }
  
  // number of inputs > 2 --> compute multiple logsums of 2 elements each, iterate
  if (num_inputs__values%2==1) {
    odd_number_of_inputs=1;
  } else {
    odd_number_of_inputs=0;
  }
  
  //   compute multiple logsums of 2 elements each
  new_input_wires__values=(int*) malloc(sizeof(int)*((num_inputs__values/2+odd_number_of_inputs)*num_wires_output));
  for (int i=0; i<num_inputs__values/2; i++) {
    for (int j=0; j<num_wires_input; j++) {
      input_tmp[j]=input_wires__values[(2*i)*num_wires_input+j];
      input_tmp[num_wires_input+j]=input_wires__values[(2*i+1)*num_wires_input+j];
    }
    Logsum_RQ(garbledCircuit,garblingContext,2*num_wires_input,input_tmp,num_inputs__table,input_wires__table,output_tmp,iteration);
    
    for (int j=0; j<num_wires_output; j++) {
      new_input_wires__values[i*num_wires_output+j]=output_tmp[j];
    }
  }
  
  if (odd_number_of_inputs==1) {
    for (int j=0; j<num_wires_input; j++) {
      new_input_wires__values[num_inputs__values/2*num_wires_output+j]=input_wires__values[(num_inputs__values-1)*num_wires_input+j];
    }
    new_input_wires__values[num_inputs__values/2*num_wires_output+(num_wires_output-1)]=
          input_wires__values[(num_inputs__values-1)*num_wires_input+(num_wires_input-1)]; // increase number of bits by 1
  }
  
  //   iterate
  for (int i=0; i<num_inputs__table-1; i++) {
    for (int j=0; j<num_wires_input; j++) {
      new_input_wires__table[i*num_wires_output+j]=input_wires__table[i*num_wires_input+j]; // t
    }
    new_input_wires__table[i*num_wires_output+(num_wires_output-1)]=
          InitializeWireZERO(garbledCircuit,garblingContext,input_wires__table[0]); // increase number of bits by 1 --> zero-padding
  }
  for (int i=0; i<num_inputs__table; i++) {
    for (int j=0; j<num_wires_input; j++) {
      new_input_wires__table[(new_num_wires_t)+i*num_wires_output+j]=input_wires__table[(num_wires_t)+i*num_wires_input+j]; // n1
    }
    new_input_wires__table[(new_num_wires_t)+i*num_wires_output+(num_wires_output-1)]=
          InitializeWireZERO(garbledCircuit,garblingContext,input_wires__table[0]); // increase number of bits by 1 --> zero-padding
  }
  for (int i=0; i<num_inputs__table; i++) {
    for (int j=0; j<2*num_wires_input; j++) {
      new_input_wires__table[(new_num_wires_t+new_num_wires_n1)+i*(2*num_wires_output)+j]=
            input_wires__table[(num_wires_t+num_wires_n1)+i*(2*num_wires_input)+j]; // n2
    }
    new_input_wires__table[(new_num_wires_t+new_num_wires_n1)+i*(2*num_wires_output)+(2*num_wires_output-2)]=
          InitializeWireZERO(garbledCircuit,garblingContext,input_wires__table[0]); // increase number of bits by 1 --> zero-padding
    new_input_wires__table[(new_num_wires_t+new_num_wires_n1)+i*(2*num_wires_output)+(2*num_wires_output-1)]=
          InitializeWireZERO(garbledCircuit,garblingContext,input_wires__table[0]); // increase number of bits by 1 --> zero-padding
  }
  
  MultipleLogsum_RQ(garbledCircuit,garblingContext,num_inputs__values/2+odd_number_of_inputs,num_wires_output,new_input_wires__values,
        num_inputs__table,new_input_wires__table,output_wires,iteration+1);
  
  free(new_input_wires__values);
  
  return 0;
}

//-----------------------
// step quantization (SQ)
//-----------------------
int Logsum_SQ(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_wires_input_x2, int* input_wires__values,
      int num_inputs__table, int* input_wires__table, int* output_wires, int iteration) {
  
  int num_wires_input=num_wires_input_x2/2;
  int num_wires_input_x2_2=(num_wires_input+1)+(num_wires_input+1);
  int max_inputs[num_wires_input],min_inputs[num_wires_input];
  int input_sub_Mm[num_wires_input_x2],sub_Mm[num_wires_input+1],sub_Mm_pos[num_wires_input];
  int input_cmp[num_wires_input_x2],control_wire,output_cmp[num_inputs__table-1];
  int input_mux_n[num_inputs__table*num_wires_input],nx[num_wires_input];
  int input_final_add[num_wires_input_x2],output_final_add[num_wires_input+1];
  int num_wires_t=(num_inputs__table-1)*num_wires_input,num_wires_n=num_inputs__table*num_wires_input;
  
  
  // get value for search in look-up table
  MAXc(garbledCircuit,garblingContext,num_wires_input_x2,input_wires__values,max_inputs);
  MINc(garbledCircuit,garblingContext,num_wires_input_x2,input_wires__values,min_inputs);
  
  //   'sub_Mm' instead of 'sub_mM' for accessing the look-up table, because 0 < sub_Mm < +inf
  for (int j=0; j<num_wires_input; j++) {
    input_sub_Mm[j]=max_inputs[j];
    input_sub_Mm[num_wires_input+j]=min_inputs[j];
  }
  SUBc(garbledCircuit,garblingContext,num_wires_input_x2,input_sub_Mm,sub_Mm);
  
  //   remove sign bit (always '0')
  for (int j=0; j<num_wires_input; j++) {
    sub_Mm_pos[j]=sub_Mm[j];
  }
  
  // get indexes for search in look-up table
  control_wire=InitializeWireZERO(garbledCircuit,garblingContext,input_wires__values[0]); // initialize as '0'
  for (int j=0; j<num_wires_input; j++) {
    input_cmp[num_wires_input+j]=sub_Mm_pos[j];
  }
  for (int i=0; i<num_inputs__table-1; i++) {
    for (int j=0; j<num_wires_input; j++) {
      input_cmp[j]=input_wires__table[i*num_wires_input+j]; // t
    }
    CMP(garbledCircuit,garblingContext,num_wires_input_x2,input_cmp,&control_wire,&output_cmp[i]);
  }
  
  // retrieve the correct value from the look-up table
  for (int i=0; i<num_inputs__table; i++) {
    for (int j=0; j<num_wires_input; j++) {
      input_mux_n[i*num_wires_input+j]=input_wires__table[(num_wires_t)+i*num_wires_input+j]; // n
    }
  }
  MultipleMUX(garbledCircuit,garblingContext,num_inputs__table,num_wires_input,input_mux_n,output_cmp,nx);
  
  // compute approximation for the logsum
  //   mX+n
  for (int j=0; j<num_wires_input; j++) {
    input_final_add[j]=max_inputs[j];
    input_final_add[num_wires_input+j]=nx[j];
  }
  ADDc(garbledCircuit,garblingContext,num_wires_input_x2,input_final_add,output_final_add);
  
  for (int j=0; j<num_wires_input; j++) {
    output_wires[j]=output_final_add[j];
  }
  output_wires[num_wires_input]=output_final_add[num_wires_input-1];
  
  return 0;
}

int MultipleLogsum_SQ(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_inputs__values, int num_wires_input, int* input_wires__values,
      int num_inputs__table, int* input_wires__table, int* output_wires, int iteration) {
  
  int num_wires_output=num_wires_input+1;
  int num_wires_t=(num_inputs__table-1)*num_wires_input,num_wires_n=num_inputs__table*num_wires_input,
        new_num_wires_t=(num_inputs__table-1)*num_wires_output,new_num_wires_n=num_inputs__table*num_wires_output;
  int input_tmp[2*num_wires_input],output_tmp[num_wires_output],
        *new_input_wires__values,new_input_wires__table[new_num_wires_t+new_num_wires_n];
  int odd_number_of_inputs;
  
  
  // number of inputs = 1 --> return input with additional MSB
  if (num_inputs__values==1) {
    for (int j=0; j<num_wires_input; j++) {
      output_wires[j]=input_wires__values[j];
    }
    output_wires[num_wires_output-1]=input_wires__values[num_wires_input-1]; // increase number of bits by 1
    
    return 0;
  }
  
  // number of elements = 2 --> compute logsum of 2 elements each
  if (num_inputs__values==2) {
    Logsum_SQ(garbledCircuit,garblingContext,2*num_wires_input,input_wires__values,
          num_inputs__table,input_wires__table,output_wires,iteration);
    
    return 0;
  }
  
  // number of inputs > 2 --> compute multiple logsums of 2 elements each, iterate
  if (num_inputs__values%2==1) {
    odd_number_of_inputs=1;
  } else {
    odd_number_of_inputs=0;
  }
  
  //   compute multiple logsums of 2 elements each
  new_input_wires__values=(int*) malloc(sizeof(int)*((num_inputs__values/2+odd_number_of_inputs)*num_wires_output));
  for (int i=0; i<num_inputs__values/2; i++) {
    for (int j=0; j<num_wires_input; j++) {
      input_tmp[j]=input_wires__values[(2*i)*num_wires_input+j];
      input_tmp[num_wires_input+j]=input_wires__values[(2*i+1)*num_wires_input+j];
    }
    Logsum_SQ(garbledCircuit,garblingContext,2*num_wires_input,input_tmp,num_inputs__table,input_wires__table,output_tmp,iteration);
    
    for (int j=0; j<num_wires_output; j++) {
      new_input_wires__values[i*num_wires_output+j]=output_tmp[j];
    }
  }
  
  if (odd_number_of_inputs==1) {
    for (int j=0; j<num_wires_input; j++) {
      new_input_wires__values[num_inputs__values/2*num_wires_output+j]=input_wires__values[(num_inputs__values-1)*num_wires_input+j];
    }
    new_input_wires__values[num_inputs__values/2*num_wires_output+(num_wires_output-1)]=
          input_wires__values[(num_inputs__values-1)*num_wires_input+(num_wires_input-1)]; // increase number of bits by 1
  }
  
  //   iterate
  for (int i=0; i<num_inputs__table-1; i++) {
    for (int j=0; j<num_wires_input; j++) {
      new_input_wires__table[i*num_wires_output+j]=input_wires__table[i*num_wires_input+j]; // t
    }
    new_input_wires__table[i*num_wires_output+(num_wires_output-1)]=
          InitializeWireZERO(garbledCircuit,garblingContext,input_wires__table[0]); // increase number of bits by 1 --> zero-padding
  }
  for (int i=0; i<num_inputs__table; i++) {
    for (int j=0; j<num_wires_input; j++) {
      new_input_wires__table[(new_num_wires_t)+i*num_wires_output+j]=input_wires__table[(num_wires_t)+i*num_wires_input+j]; // n
    }
    new_input_wires__table[(new_num_wires_t)+i*num_wires_output+(num_wires_output-1)]=
          InitializeWireZERO(garbledCircuit,garblingContext,input_wires__table[0]); // increase number of bits by 1 --> zero-padding
  }
  
  MultipleLogsum_SQ(garbledCircuit,garblingContext,num_inputs__values/2+odd_number_of_inputs,num_wires_output,new_input_wires__values,
        num_inputs__table,new_input_wires__table,output_wires,iteration+1);
  
  free(new_input_wires__values);
  
  return 0;
}

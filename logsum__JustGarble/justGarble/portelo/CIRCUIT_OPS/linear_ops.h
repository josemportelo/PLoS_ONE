/*
 * linear_ops.h
 *
 *  Created on: 01 Jul 2013
 *      Author: portelo
 */

#ifndef LINEAR_OPS_H_
#define LINEAR_OPS_H_

int ADD(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_wires_input_x2, int* input_wires, int* output_wires);
int MultipleADD(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_inputs, int num_wires_input, int* input_wires, int* output_wires);
int ADDc(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_wires_input_x2, int* input_wires, int* output_wires);
int MultipleADDc(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_inputs, int num_wires_input, int* input_wires, int* output_wires);
int SUB32(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext, int* inputs, int* outputs);
int SUB(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_wires_input_x2, int* input_wires, int* output_wires);
int SUBc(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_wires_input_x2, int* input_wires, int* output_wires);
int MUL(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
     int num_wires_input_x2, int* input_wires, int* output_wires);
int MULc(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_wires_input_x2, int* input_wires, int* output_wires);

#endif /* LINEAR_OPS_H_ */

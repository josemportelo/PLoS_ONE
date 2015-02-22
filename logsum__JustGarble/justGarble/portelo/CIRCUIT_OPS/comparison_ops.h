/*
 * comparison_ops.h
 *
 *  Created on: 03 Jul 2013
 *      Author: portelo
 */

#ifndef COMPARISON_OPS_H_
#define COMPARISON_OPS_H_

int bitMUX(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int* input_wire_1, int* input_wire_2, int* control_wire, int* output_wire);
int MUX(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_wires_input_x2, int* input_wires, int* control_wire, int* output_wires);
int MultipleMUX(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_inputs, int num_wires_input, int* input_wires, int* control_wires, int* output_wires);

int bitCMP(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int* input_wire_1, int* input_wire_2, int* control_wire, int* output_wire);
int CMP(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_wires_input_x2, int* input_wires, int* control_wire, int* output_wires);
int LT(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_wires_input_x2, int* input_wires, int* output_wires);
int LEQ(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_wires_input_x2, int* input_wires, int* output_wires);
int GT(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_wires_input_x2, int* input_wires, int* output_wires);
int GEQ(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_wires_input_x2, int* input_wires, int* output_wires);

int MIN(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_wires_input_x2, int* input_wires, int* output_wires);
int MINc(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_wires_input_x2, int* input_wires, int* output_wires);
// int MultipleMIN(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
//       int num_inputs, int num_wires_input, int* input_wires, int* output_wires);
int MultipleMINc(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_inputs, int num_wires_input, int* input_wires, int* output_wires);
int MAX(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_wires_input_x2, int* input_wires, int* output_wires);
int MAXc(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_wires_input_x2, int* input_wires, int* output_wires);
// int MultipleMAX(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
//       int num_inputs, int num_wires_input, int* input_wires, int* output_wires);
int MultipleMAXc(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_inputs, int num_wires_input, int* input_wires, int* output_wires);

#endif /* COMPARISON_OPS_H_ */

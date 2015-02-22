/*
 * logsum_ops.h
 *
 *  Created on: 08 Jul 2013
 *      Author: portelo
 */

#ifndef LOGSUM_OPS_H_
#define LOGSUM_OPS_H_

int Logsum_RQ(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_wires_input_x2, int* input_wires__values,
      int num_inputs__table, int* input_wires__table, int* output_wires, int iteration);
int MultipleLogsum_RQ(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_inputs__values, int num_wires_input, int* input_wires__values,
      int num_inputs__table, int* input_wires__table, int* output_wires, int iteration);

int Logsum_SQ(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_wires_input_x2, int* input_wires__values,
      int num_inputs__table, int* input_wires__table, int* output_wires, int iteration);
int MultipleLogsum_SQ(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext,
      int num_inputs__values, int num_wires_input, int* input_wires__values,
      int num_inputs__table, int* input_wires__table, int* output_wires, int iteration);

#endif /* LOGSUM_OPS_H_ */

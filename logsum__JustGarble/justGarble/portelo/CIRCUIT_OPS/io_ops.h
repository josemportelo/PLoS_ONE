/*
 * io_ops.h
 *
 *  Created on: 01 Jul 2013
 *      Author: portelo
 */

#ifndef IO_OPS_H_
#define IO_OPS_H_

int InitializeWireZERO(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext, int dummy_wire);
int InitializeWireONE(GarbledCircuit *garbledCircuit, GarblingContext *garblingContext, int dummy_wire);

void BIN2DEC(int num_elements, int num_wires_bin, int *inputs_bin, int *outputs_dec);
void BIN2DECc(int num_elements, int num_wires_bin, int *inputs_bin, int *outputs_dec);
void DEC2BIN(int num_elements, int *inputs_dec, int num_wires_bin, int *outputs_bin);

int ReadInputs(char *input_file_name, int num_inputs, int *inputs_dec);
int ShowGarbledCircuit(GarbledCircuit *garbledCircuit);

#endif /* IO_OPS_H_ */

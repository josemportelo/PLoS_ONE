#!/usr/bin/perl

use warnings;
use strict;

use List::Util qw[min max];
# use Math::Round qw/round/;
# $round = int($num + 0.5);

#----------------
# input arguments
#----------------
my $max_range=2**64;
my $num_inputs=$ARGV[0];     # {2,4,8,16,32,64,128}
my $num_bits=$ARGV[1];       # {8,12,16}
my $scale_num_bits=$ARGV[2]; # defines range of inputs --> [-2^(scale_num_bits-1),2^(scale_num_bits-1)]
my $scaling_factor=2**($num_bits-$scale_num_bits); # 'scale_num_bits' bits (integer) + 'num_bits-scale_num_bits' bits (decimal) --> should be 1, to avoid distortion

my @random_inputs;       my @random_inputs__full;
my @random_inputs_quant; my @random_inputs_quant__full;
my $logsum_ref;          my $logsum_ref__GC;
my $logsum_quant;        my $logsum_quant__GC;
my $error_logsum;        my $error_logsum__full;

my $max_random_inputs;
my $max_random_inputs_quant;

#------------------
# create input data
#------------------
for (my $i=0; $i<$num_inputs; $i++) {
  $random_inputs[$i]=rand($max_range)-($max_range/2);
  $random_inputs_quant__full[$i]=int($random_inputs[$i]/($max_range/2**$num_bits));
  $random_inputs_quant[$i]=$random_inputs_quant__full[$i]/$scaling_factor;
  $random_inputs[$i]/=$max_range/2**$scale_num_bits;
  $random_inputs__full[$i]=$random_inputs[$i]*$scaling_factor;
}

open INPUT_FILE, ">", "input_values_ref.txt" or die $!;
for (my $i=0; $i<$num_inputs; $i++) {
  print INPUT_FILE "$random_inputs[$i]\ti$i\n";
}
close INPUT_FILE;

open INPUT_FILE_quant, ">", "input_values.txt" or die $!;
for (my $i=0; $i<$num_inputs; $i++) {
  print INPUT_FILE_quant "$random_inputs_quant__full[$i]\ti$i\n";
}
close INPUT_FILE_quant;

#---------------
# compute logsum
#---------------
$max_random_inputs=max(@random_inputs);
$logsum_ref=0;
for (my $i=0; $i<$num_inputs; $i++) {
  $logsum_ref+=exp($random_inputs[$i]-$max_random_inputs);
}
$logsum_ref=$max_random_inputs+log($logsum_ref);
$logsum_ref__GC=$logsum_ref*$scaling_factor;

$max_random_inputs_quant=max(@random_inputs_quant);
$logsum_quant=0;
for (my $i=0; $i<$num_inputs; $i++) {
  $logsum_quant+=exp($random_inputs_quant[$i]-$max_random_inputs_quant);
}
$logsum_quant=$max_random_inputs_quant+log($logsum_quant);
$logsum_quant__GC=$logsum_quant*$scaling_factor;

$error_logsum=$logsum_quant-$logsum_ref;

#-------
# output
#-------
print "||| Scaling Factor |||\n";
print "  scaling factor: $scaling_factor\n";
print "||| Inputs |||\n";
for (my $i=0; $i<$num_inputs; $i++) {
  print "  input[$i]: $random_inputs[$i] ; input_quant[$i]: $random_inputs_quant[$i] --> $random_inputs_quant__full[$i]\n";
}
print "||| Logsum |||\n";
printf "  logsum_ref: %03.20f --> %f; logsum_quant: %03.20f --> %f; error: %03.20f\n",$logsum_ref,$logsum_ref__GC,$logsum_quant,$logsum_quant__GC,$error_logsum;

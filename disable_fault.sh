#!/bin/bash

#echo 1 | sudo tee /sys/module/fault/parameters/enable_pa_fault
echo 0 | sudo tee /sys/module/kvm/parameters/enable_kvm_faults
echo 0 | sudo tee /sys/module/kvm/parameters/kvm_faults_counter


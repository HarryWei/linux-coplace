#!/bin/bash

echo 1 | sudo tee /sys/module/fault/parameters/enable_pa_fault

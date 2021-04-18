#!/bin/bash

echo 7179 | sudo tee /sys/module/fault/parameters/enable_pa_fault

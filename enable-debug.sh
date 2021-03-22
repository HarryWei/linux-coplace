#!/bin/bash

echo 1 | sudo tee /sys/module/memory/parameters/debug_ca_flag

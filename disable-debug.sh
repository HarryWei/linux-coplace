#!/bin/bash

echo 0 | sudo tee /sys/module/memory/parameters/debug_ca_flag

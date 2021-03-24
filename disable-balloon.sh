#!/bin/bash

echo 0 | sudo tee /sys/module/memory/parameters/enable_ca_balloon

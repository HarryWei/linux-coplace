#!/bin/bash

echo 1 | sudo tee /sys/module/memory/parameters/enable_ca_balloon

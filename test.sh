#!/bin/sh -x

echo 'Sending osc commands..'
oscsend localhost 7771 /toggle i 1
sleep 1
oscsend localhost 7771 /hslider f 0.05
sleep 1
oscsend localhost 7771 /vslider f 0.8
sleep 1
oscsend localhost 7771 /knob f 0.9

echo 'Sleeping 3 seconds before changing values..'
sleep 3
oscsend localhost 7771 /toggle i 0
sleep 1
oscsend localhost 7771 /hslider f 0.75
sleep 1
oscsend localhost 7771 /vslider f 0.1
sleep 1
oscsend localhost 7771 /knob f 0.1

echo 'done!'

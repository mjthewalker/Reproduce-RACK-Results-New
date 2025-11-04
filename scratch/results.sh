#!/bin/bash

./ns3 run "examples/tcp/rack-example --stopTime=300"
mkdir rack/no-reordering
mv rack/example rack/no-reordering/no-rack

./ns3 run "examples/tcp/rack-example --stopTime=300 --rack=true --dsack=true"
mv rack/example rack/no-reordering/rack

./ns3 run "examples/tcp/rack-example --stopTime=10 --reorder=true"
mkdir rack/reordering
mv rack/example rack/reordering/no-rack

./ns3 run "examples/tcp/rack-example --stopTime=10 --reorder=true --rack=true --dsack=true"
mv rack/example rack/reordering/rack
#!/bin/bash
parent_path=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )

pushd "$parent_path" > /dev/null

g++ -O3 single_run.cpp -o single_run.out

popd > /dev/null

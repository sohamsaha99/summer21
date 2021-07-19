#!/bin/bash
parent_path=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )

pushd "$parent_path" > /dev/null

python3 household.py < params/graph_params.txt

./single_run.out params/simulation_params.txt

python3 jsonUtils.py

popd > /dev/null

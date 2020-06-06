#!/bin/bash

/app/eclipse/eclipse -nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild -import ./example_project/ -build UmbaCppTest/Docker

./example_project/Docker/UmbaCppTest

retVal=$?
if [ $retVal -eq 0 ]; then
    echo "This test run is expected to fail!"
    exit 1
else 
    echo "This test run was expected to fail to demonstrate error printing, so build status is not affected"
    exit 0
fi
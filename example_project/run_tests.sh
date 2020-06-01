#!/bin/bash

set -e
set -x

ls -la

/app/eclipse/eclipse -nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild -import ./example_project/ -build UmbaCppTest/Docker

./example_project/Docker/UmbaCppTest
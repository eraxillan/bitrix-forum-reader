#!/bin/bash
#
# Copyright © 2016-2017 Oleksii Aliakin. All rights reserved.
# Author: Oleksii Aliakin (alex@nls.la).
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

THIS_DIR=$(cd $(dirname $0) && pwd)

export APP_DIR_NAME=SimpleQtQmlApp
export APP_NAME=qtQmlsimple
export SRC_DIR=$(cd "$THIS_DIR/../../" && pwd)
export INSTALL_DIR=/tmp/Qt-Qbs-Application/examples
export SCRIPTS_DIR="$SRC_DIR/scripts"
export DEPLOYMENT_INFO_FILE="$THIS_DIR/deployment_info"

"$SCRIPTS_DIR/build_in_docker.sh"


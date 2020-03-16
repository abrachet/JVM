// Copyright 2020 Alex Brachet
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// This class provides the start method which the jvm creates a frame for
// before calling main.
class __JVM_internal_Start {
    private static native void exit(int exitCode);

    // TODO: this will have exception handling in the future.
    private static void start() {
        // JVM puts the return address here then it finds and calls main itself.
        exit(0);
    }
}

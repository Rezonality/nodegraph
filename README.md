[NodeGraph](https://github.com/cmaughan/nodegraph) - A simple Directed Graph, with SVG UI
===================================================================================================

[![Build Status](https://travis-ci.org/cmaughan/nodegraph.svg?branch=master)](https://travis-ci.org/cmaughan/nodegraph)
[![Build status](https://ci.appveyor.com/api/projects/status/9hgmooahite233av?svg=true)](https://ci.appveyor.com/project/cmaughan/nodegraph)
[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/cmaughan/nodegraph/blob/master/LICENSE)
[![codecov](https://codecov.io/gh/cmaughan/nodegraph/branch/master/graph/badge.svg)](https://codecov.io/gh/cmaughan/nodegraph)
[![Join the chat at https://gitter.im/Resonality/nodegraph](https://badges.gitter.im/Resonality/nodegraph.svg)](https://gitter.im/Resonality/nodegraph?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge) 

Nodegraph is a basic directed graph, used in my live coding project.  It is a work in progress.  The nodes in the graph are evaluated using a 'Pull' architecture, and support concepts of Flow and Control data.  The included sample application shows the node UI you can build from the graph specification. It supports non-linear rotary knobs of linear and non-linear type, buttons and sliders.  There is currently no way to build the graph visually, but you can generate visualizations of it and control input parameters.  This project should very much be considered a work in progress.  The intended use-case is a graph representing an audio synthesizer; you can find that project at [MAudio](https://github.com/cmaughan/MAudio).  This project is cross platform and with a C++17 compiler should build on Mac, Linux and Windows.

![ImGui](screenshots/sample.png)

Because of modern CMake, you can install this library & reference it in your CMake with just a couple of lines.

First, you need to build MUtils, my shared library of useful bits. Hop over to [MUtils](https://github.com/cmaughan/MUtils) and build and install the package.

The NodeGraph build is pretty standard
It is built using these helper scripts:

```
config.bat OR config.sh
build_all.bat OR build.sh
```

Now in your project CMakeLists.txt, you can add the following to reference the library and pull in all the header paths.  Note there are no include/library defines; the target_link_libraries is enough to pull all the necessary things in.  See the example application for how this works.

```
find_package(NodeGraph REQUIRED)
...
target_link_libraries(MyApp PRIVATE NodeGraph::NodeGraph)
```



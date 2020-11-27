[NodeGraph](https://github.com/Rezonality/nodegraph) - A simple Directed Graph, with SVG UI
===================================================================================================
[![Builds](https://github.com/Rezonality/nodegraph/workflows/Builds/badge.svg)](https://github.com/Rezonality/nodegraph/actions?query=workflow%3ABuilds)
[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/Rezonality/nodegraph/blob/master/LICENSE)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/c6ec28a8cd2d4ca0a7f3d087846c0c77)](https://www.codacy.com/gh/Rezonality/nodegraph/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=Rezonality/nodegraph&amp;utm_campaign=Badge_Grade)
[![Codacy Badge](https://app.codacy.com/project/badge/Coverage/c6ec28a8cd2d4ca0a7f3d087846c0c77)](https://www.codacy.com/gh/Rezonality/nodegraph/dashboard?utm_source=github.com&utm_medium=referral&utm_content=Rezonality/nodegraph&utm_campaign=Badge_Coverage)
[![codecov](https://codecov.io/gh/Rezonality/nodegraph/branch/master/graph/badge.svg?token=d1WiVAVDAC)](https://codecov.io/gh/Rezonality/nodegraph)
[![Gitter](https://badges.gitter.im/Rezonality/Nodegraph.svg)](https://gitter.im/Rezonality/Nodegraph?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge)

Nodegraph is a basic directed graph, used in my live coding project.  It is a work in progress.  The nodes in the graph are evaluated using a 'Pull' architecture, and support concepts of Flow and Control data.  The included sample application shows the node UI you can build from the graph specification. It supports non-linear rotary knobs of linear and non-linear type, buttons and sliders.  There is currently no way to build the graph visually, but you can generate visualizations of it and control input parameters.  This project should very much be considered a work in progress.  The intended use-case is a graph representing an audio synthesizer; you can find that project at [MAudio](https://github.com/Rezonality/MAudio).  This project is cross platform and with a C++17 compiler should build on Mac, Linux and Windows.

![ImGui](screenshots/sample.png)

Because of modern CMake, you can install this library & reference it in your CMake with just a couple of lines.

First, you need to build MUtils, my shared library of useful bits. Hop over to [MUtils](https://github.com/Rezonality/MUtils) and build and install the package.

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



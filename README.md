# GraphVisualizer
Visualize Graph for c++

GraphVisualizer Portable Package
================================

On the computer where GraphVisualizer is already configured:
1. Run MakePortablePackage.bat from this kit.
2. Wait for the Desktop\GraphVisualizer_Portable folder and ZIP to be created.
3. Copy GraphVisualizer_Portable.zip to the other Windows PC.
4. Extract the ZIP to a permanent folder, for example C:\Tools\GraphVisualizer.
5. Run setup.bat once. It checks the viewer/compiler and creates a desktop shortcut.
6. Double-click the desktop shortcut "GraphVisualizer".

Daily use:
- The launcher starts the Viewer first.
- It opens a file picker for a .cpp file.
- The selected file replaces examples\main.cpp.
- It runs build_mygraph.bat and then run_mygraph.bat.
- Put the test input in examples\file.in.
- C++ code should use:
    freopen("file.in", "r", stdin);
    freopen("file.out", "w", stdout);

Notes:
- The package includes the MinGW compiler and the deployed Qt runtime/plugins.
- It does not include the Qt SDK, CMake or Ninja because the destination PC does not need to rebuild the Viewer.
- Keep the package in a permanent location after creating the desktop shortcut; do not move it without running setup.bat again.
- The first packaging operation can take several minutes because the MinGW compiler directory is copied.


for c++:
InitGraph()	连接可视化软件
AddNode(u)	添加节点
AddEdge(u,v)	添加无权边
AddEdge(u,v,w,d)	添加带权边
HighLight(u)	高亮节点及其关联边
HighLightEdge(u,v)	高亮指定边
ClearHighLight()	清除高亮
RemoveNode(u)	删除节点
RemoveEdge(u,v)	删除边
ClearGraph()	清空图
SetNodeLabel(u,s)	修改节点文字
Sleep(ms)	插入一个播放延迟操作

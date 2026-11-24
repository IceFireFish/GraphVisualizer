#ifndef GRAPH_VISUALIZER_H
#define GRAPH_VISUALIZER_H

#include <cstdint>
#include <string>

// Connect to the local GraphVisualizer desktop viewer.
// Start GraphVisualizerViewer first (default endpoint: 127.0.0.1:47653).
bool InitGraph(const std::string &host = "127.0.0.1", std::uint16_t port = 47653);
void ShutdownGraph();
bool IsGraphConnected();

void AddNode(int u);
void AddEdge(int u, int v);
void AddEdge(int u, int v, const std::string &weight, int directed = 1);

// Highlight one node and every edge incident to it. The previous highlight is cleared.
void HighLight(int u);
void HighLightEdge(int u, int v);
void ClearHighLight();

void RemoveNode(int u);
void RemoveEdge(int u, int v);
void ClearGraph();

void SetNodeLabel(int u, const std::string &label);
void SetNodeColor(int u, const std::string &color);

// Add a playback wait marker to the history. It does NOT sleep/block the C++ thread.
// During autoplay, the viewer waits for ms before advancing to the next event.
void Sleep(int ms);

#endif // GRAPH_VISUALIZER_H

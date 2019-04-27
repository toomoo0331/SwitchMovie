//
// Created by kei666 on 18/11/14.
//

#ifndef PROJECT_GRAPH_H
#define PROJECT_GRAPH_H

#include <utility>
#include <vector>
#include <queue>
#include <algorithm>
#include <limits>
#include <cfloat>

#include "radix-heap/radix_heap.h"

typedef std::pair<int, float> iPair;

class Graph {
public:
    Graph(int camera_num) : camera_num_(camera_num){};

    int addVertex()
    {
        adj.resize(adj.size() + 1);
        return (int)adj.size() - 1;
    }

    void addEdge(int u, int v,float w)
    {
//        adj[u].push_back(iPair(v, w));
        adj[u].emplace_back(v, w);
    }

    const iPair getIPaifdr(int i) const {
        return adj[i][0];
    }

    std::vector<int> shortestPath(int start, int end)
    {
        //ダイクストラ法の計算をしているはず
//        std::priority_queue<iPair> pq;
        radix_heap::pair_radix_heap<float, int> pq;
        std::vector<int> prever(adj.size(), -1);
        std::vector<float> dist(adj.size(), std::numeric_limits<float>::max());

//        pq.push(iPair(start, 0.0);
        pq.emplace(0.0, start);
        dist[start] = 0.0;
        while(!pq.empty()) {
            int u = pq.top_value();
            float p = pq.top_key();
            pq.pop();
            if (p > dist[u]) continue;
            std::vector<iPair>::iterator it;
            for (it = adj[u].begin(); it != adj[u].end(); ++it) {
                int v = it->first;
                float weight = it->second;
                if (dist[v] > dist[u] + weight /*|| (v % camera_num_ == u % camera_num_ && dist[v] == dist[u] + weight)*/) {
                    dist[v] = dist[u] + weight;
                    prever[v] = u;
                    pq.emplace(dist[v], v);
                }
            }
        }
        std::vector<int> path;
        for (; end != -1; end = prever[end]) {
            int video_num = ((end % camera_num_) > 0) ? end % camera_num_ -1 : camera_num_ - 1;
            path.emplace_back(video_num);
        }
        path.erase(path.begin());
        path.pop_back();
        std::reverse(path.begin(), path.end());
        return path;
    }

private:
    std::vector<std::vector<iPair>> adj;
    int camera_num_;
};

#endif //PROJECT_GRAPH_H
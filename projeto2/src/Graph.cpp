#include "Graph.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <cmath>
#include <chrono>

using namespace std;


//********************** Vertex **********************//
// Constructors
Vertex::Vertex(int id) : id(id) {}
Vertex::Vertex(int id, double latitude, double longitude) : id(id), latitude(latitude), longitude(longitude) {}

// Getters and Setters
int Vertex::getId() const {
    return id;
}
double Vertex::getLatitude() const {
    return this->latitude;
}
double Vertex::getLongitude() const {
    return this->longitude;
}
unordered_map<int, double> Vertex::getAdj() const {
    return adj;
}
bool Vertex::isVisited() const {
    return visited;
}

void Vertex::setLatitude(double lat) {
    this->latitude = lat;
}
void Vertex::setLongitude(double lon) {
    this->longitude = lon;
}
void Vertex::setVisited(bool visited_) {
    this->visited = visited_;
}


void Vertex::addEdge(int dest, double w) {
    if (dest == this->getId()) {
        return;
    }
    adj[dest] = w;
}
void Vertex::removeEdge(int dest) {
    adj.erase(dest);
}



//********************** Graph **********************//

Graph::Graph() {
    unordered_map<int, Vertex*> placeholderMap;
    vertexMap = placeholderMap;
}

Graph::~Graph() {
    for (auto vertex : vertexMap) {
        delete vertex.second;
    }
}

bool Graph::read_edges(const string &file, bool skip) {
    ifstream in(file);
    if (!in) return false;

    string line;
    if (skip) {
        getline(in, line);
    }

    while (getline(in, line)) {
        // MacBook Required to avoid early line exit.
        //line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
        //line.erase(std::remove_if (line.begin(), line.end(), ::isspace), line.end());
        istringstream iss(line);
        string word;
        int id1 = -1, id2 = -1;
        double weight = 0;
        int count = 0;
        while (std::getline(iss, word, ',')) {
            if (count > 2) {
                break;
            }
            switch (count) {
                case 0:
                    id1 = stoi(word);
                    break;
                case 1:
                    id2 = stoi(word);
                    break;
                case 2:
                    weight = stod(word);
                    break;
                default:
                    break;
            }
            count++;
        }
        if (id1 == -1 || id2 == -1) {
            cerr << "Invalid IDs found in line: " << line << endl;
            continue;
        }
        Vertex *v1 = findVertex(id1);
        Vertex *v2 = findVertex(id2);
        if (v1 == nullptr) {
            v1 = new Vertex(id1);
            vertexMap[id1] = v1;
        }
        if (v2 == nullptr) {
            v2 = new Vertex(id2);
            vertexMap[id2] = v2;
        }
        v1->addEdge(id2, weight);
        v2->addEdge(id1, weight);
    }
    return true;
}

bool Graph::read_nodes(const string &file) {
    ifstream in(file);
    if (!in) return false;
    else {
        string line;
        getline(in, line);
        while (getline(in, line)) {
            // MacBook Required to avoid early line exit.
            line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
            line.erase(std::remove_if (line.begin(), line.end(), ::isspace), line.end());

            istringstream iss(line);
            string word;
            int id;
            double latitude;
            double longitude;
            int count = 0;
            while (std::getline(iss, word, ',')) {
                switch (count) {
                    case 0:
                        id = stoi(word);
                        break;
                    case 1:
                        longitude = stod(word);
                        break;
                    case 2:
                        latitude = stod(word);
                        break;
                    default:
                        break;
                }
                count++;
            }
            Vertex *v = findVertex(id);
            if (v == nullptr) {
                v = new Vertex(id, latitude, longitude);
                vertexMap[id] = v;
            }
            else{
                v->setLatitude(latitude);
                v->setLongitude(longitude);
            }
        }
        return true;
    }
}


int Graph::getNumVertex() const {
    return vertexMap.size();
}
unordered_map<int, Vertex*> Graph::getVertexMap() const {
    return vertexMap;
}


Vertex* Graph::findVertex(const int &id) const {
    auto it = vertexMap.find(id);
    if (it != vertexMap.end()) return it->second;
    else return nullptr;
}


void Graph::addAllEdgesTo(Vertex *v, unordered_map<int, double> weights) const {
    for (int i = 0; i < getNumVertex(); i++) {
        if (i == v->getId()) {
            continue;
        }
        getVertexMap()[i]->addEdge(i, weights[i]);
    }
}
unordered_map<int, double> Graph::removeAllEdgesTo(Vertex *v) const {
    unordered_map<int, double> weights;
    for (int i = 0; i < getNumVertex(); i++) {
        if (i == v->getId()) {
            continue;
        }
        weights[i] = v->getAdj()[i];
        v->removeEdge(i);
    }
    return weights;
}


// Brute-Force
vector<int> Graph::bruteForceTSP(int src) const {
    auto start = chrono::high_resolution_clock::now();

    double current_minimum_path = INF;

    vector<int> result;
    vector<vector<int>> paths = this->computeHamiltonianPaths(src);
    for (const auto& path : paths) {
        double path_distance = pathDistance(path);
        if (path_distance < current_minimum_path) {
            result = path;
            current_minimum_path = path_distance;
        }
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    cout << "Approach took " << duration.count() << " seconds." << endl;
    cout << "Path distance is: " << pathDistance(result) << endl;

    for (auto vertex : getVertexMap()) {
        vertex.second->setVisited(false);
    }

    return result;
}

vector<vector<int>> Graph::computeHamiltonianPaths(int source) const {
    Vertex *src = findVertex(source);
    vector<vector<int>> result;
    if (src == nullptr) return result;

    vector<int> path;
    path.push_back(source);

    vector<bool> visited;
    visited.resize(getNumVertex());
    visited[source] = true;

    hamiltonianRecursive(result, path, visited);
    return result;
}
void Graph::hamiltonianRecursive(vector<vector<int>> &result, vector<int> path, vector<bool> visited) const {
    if (path.size() == this->getNumVertex()) {
        path.push_back(path[0]);
        result.push_back(path);
        return;
    }
    for (int i = 0; i < this->getNumVertex(); i++) {
        if (!visited[i]) {
            visited[i] = true;
            path.push_back(i);
            hamiltonianRecursive(result, path, visited);
            path.pop_back();
            visited[i] = false;
        }
    }
}

double Graph::pathDistance(vector<int> path) const {
    double result = 0;
    size_t path_size = path.size();

    for (int i = 0; i < path_size - 1; i++) {
        double step_weight = findVertex(path[i])->getAdj()[path[i + 1]];
        if (step_weight == 0.0) { // As specified in T2.2 - If there is no edge between two vertices (assume there is), use Haversine formula with coords
            result += getHaversine(findVertex(path[i]), findVertex(path[i + 1]));
        }
        else result += step_weight;
    }
    return result;
}

double Graph::convertToRadians(double coord) {
    return coord*M_PI/180;
}
double Graph::getHaversine(Vertex *v1, Vertex *v2) {
    const double earthRadius = 6371000.0; // meters

    const double rad_lat1 = convertToRadians(v1->getLatitude());
    const double rad_lon1 = convertToRadians(v1->getLongitude());
    const double rad_lat2 = convertToRadians(v2->getLatitude());
    const double rad_lon2 = convertToRadians(v2->getLongitude());

    const double delta_lat = rad_lat2 - rad_lat1;
    const double delta_lon = rad_lon2 - rad_lon1;

    const double aux = pow(sin(delta_lat / 2), 2) + cos(rad_lat1) * cos(rad_lat2) * pow(sin(delta_lon / 2), 2);
    const double c = 2.0 * atan2(sqrt(aux), sqrt(1.0 - aux));

    return earthRadius * c;
}


// T4.1
vector<int> Graph::backtrackingTSP(int source, double &dist, double &time) const {
    auto start = chrono::high_resolution_clock::now();

    for (auto vertex : getVertexMap()) {
        vertex.second->setVisited(false);
    }

    Vertex *src = findVertex(source);
    vector<int> best_path;
    if (src == nullptr) {
        cout << "Invalid source!" << endl;
        return best_path;
    }
    vector<int> path;
    path.push_back(source);
    src->setVisited(true);

    recursiveBacktracking(path, best_path);

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    time = duration.count();
    dist = pathDistance(best_path);

    return best_path;
}
void Graph::recursiveBacktracking(vector<int> &path, vector<int> &best_path) const {
    if (path.size() == this->getNumVertex()) {
        path.push_back(path.front());
        if (best_path.empty() || (pathDistance(path) < pathDistance(best_path))) {
            best_path = path;
        }
        path.pop_back();
        return;
    }
    Vertex* currentVertex = findVertex(path.back());
    if (currentVertex == nullptr) {
        return;
    }
    for (int i = 0; i < getNumVertex(); i++) {
        if (i == currentVertex->getId()) {
            continue;
        }
        Vertex* nextVertex = findVertex(i);
        if (!nextVertex->isVisited()) {
            nextVertex->setVisited(true);
            path.push_back(nextVertex->getId());
            recursiveBacktracking(path, best_path);
            path.pop_back();
            nextVertex->setVisited(false);
        }
    }
}

// T4.2
vector<int> Graph::triangularApproximationTSP(int src, double &dist, double &time) const {
    auto start = chrono::high_resolution_clock::now();

    vector<vector<int>> tree = prims();
    vector<int> path;
    vector<bool> visited(getNumVertex(), false);

    preorderDFS(tree, path, src, visited);

    path.push_back(src);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    time = duration.count();
    dist = pathDistance(path);
    return path;
}
vector<vector<int>> Graph::prims() const {
    int numVertices = getNumVertex();
    vector<int> parent(numVertices, -1);
    vector<double> key(numVertices, numeric_limits<double>::max());
    vector<bool> inMST(numVertices, false);

    // Priority queue to store weights and vertices
    auto compare = [](pair<double, int> left, pair<double, int> right) { return left.first > right.first; };
    priority_queue<pair<double, int>, vector<pair<double, int>>, decltype(compare)> pq(compare);

    key[0] = 0;
    pq.emplace(0, 0);

    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();

        inMST[u] = true;

        for (const auto &adjPair : vertexMap.at(u)->getAdj()) {
            int v = adjPair.first;
            double weight = adjPair.second;

            if (!inMST[v] && weight < key[v]) {
                key[v] = weight;
                pq.emplace(key[v], v);
                parent[v] = u;
            }
        }
    }

    vector<vector<int>> mst(numVertices);
    for (int i = 1; i < numVertices; ++i) {
        mst[parent[i]].push_back(i);
        mst[i].push_back(parent[i]);
    }

    return mst;
}
void Graph::preorderDFS(const vector<vector<int>> &tree, vector<int>& res, int current, vector<bool>& visited) const {
    visited[current] = true;
    res.push_back(current);

    for (int neighbor : tree[current]) {
        if (!visited[neighbor]) {
            preorderDFS(tree, res, neighbor, visited);
        }
    }
}

// T4.3
vector<int> Graph::nearestNeighbourTSP(int src, double &dist, double &time) const {
    auto start = chrono::high_resolution_clock::now();

    for (auto vertex : getVertexMap()) {
        vertex.second->setVisited(false);
    }
    vector<int> result;
    Vertex *v = findVertex(src);
    if (v == nullptr) {
        std::cout << "Vertex not found!" << std::endl;
        return result;
    }
    v->setVisited(true);
    result.push_back(src);
    while(result.size() != getNumVertex()) {
        double min_edge = INF;
        Vertex *next = nullptr;
        for (auto edge : v->getAdj()) {
            if (!findVertex(edge.first)->isVisited() && (edge.second < min_edge)) {
                min_edge = edge.second;
                next = findVertex(edge.first);
            }
        }
        if (next == nullptr) {
            cout << "Could not find path!\n";
            return {};
        }
        next->setVisited(true);
        result.push_back(next->getId());
        v = next;
    }
    result.push_back(src);

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    time = duration.count();
    dist = pathDistance(result);
    return result;
}


// T4.4

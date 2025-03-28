#include "Graph.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;


//********************** Vertex **********************//

Location::Location(int id, const string &code) : id(id) ,code(code) {
    indegree = 0;
}


string Location::getCode() const {
    return code;
}
int Location::getId() const {
    return id;
}
vector<Pipe*> Location::getAdj() const {
    return adj;
}
vector<Pipe *> Location::getIncoming() const {
    return incoming;
}
bool Location::isVisited() const {
    return visited;
}
bool Location::isProcessing() const {
    return processing;
}
unsigned int Location::getIndegree() const {
    return indegree;
}
Pipe *Location::getPath() const {
    return path;
}


void Location::setInfo(string code_) {
    this->code = code_;
}
void Location::setVisited(bool visited_) {
    this->visited = visited_;
}
void Location::setProcessing(bool processing_) {
    this->processing = processing_;
}
void Location::setIndegree(unsigned int indegree_) {
    this->indegree = indegree_;
}
void Location::setPath(Pipe *path_) {
    this->path = path_;
}



Pipe* Location::addEdge(Location *d, double w) {
    auto newEdge = new Pipe(this, d, w);
    adj.push_back(newEdge);
    d->indegree += 1;
    d->incoming.push_back(newEdge);
    return newEdge;
}

bool Location::removeEdge(const string& in) {
    bool removedEdge = false;
    auto it = adj.begin();
    while (it != adj.end()) {
        Pipe *edge = *it;
        Location *dest = edge->getDest();
        if (dest->getCode() == in) {
            it = adj.erase(it);
            deleteEdge(edge);
            removedEdge = true; // allows for multiple edges to connect the same pair of vertices (multigraph)
        }
        else {
            it++;
        }
    }
    return removedEdge;
}

void Location::removeOutgoingEdges() {
    auto it = adj.begin();
    while (it != adj.end()) {
        Pipe *edge = *it;
        it = adj.erase(it);
        deleteEdge(edge);
    }
}


void Location::deleteEdge(Pipe *edge) {
    Location *dest = edge->getDest();
    // Remove the corresponding edge from the incoming list
    auto it = dest->incoming.begin();
    while (it != dest->incoming.end()) {
        if ((*it)->getOrig()->getCode() == code) {
            it = dest->incoming.erase(it);
        }
        else {
            it++;
        }
    }
    delete edge;
}



Reservoir::Reservoir(const string &name, const string &municipality, int id, const string& code, double maximumDelivery)
    : Location(id, code), name(name), municipality(municipality), maximumDelivery(maximumDelivery) {}

const string &Reservoir::getName() const {
    return name;
}
const string &Reservoir::getMunicipality() const {
    return municipality;
}
double Reservoir::getMaximumDelivery() const {
    return maximumDelivery;
}


PumpingStation::PumpingStation(int id, const string& code)
    : Location(id,code) {
}



City::City(const string &name, int id, const string &code, double demand, int population)
    : Location(id,code), name(name), demand(demand), population(population) {}

string City::getName() const {
    return name;
}
double City::getDemand() const {
    return demand;
}
double City::getTotalFlow() const {
    return totalFlow;
}

void City::setTotalFlow(double maximumFlow) {
    this->totalFlow = maximumFlow;
}



//********************** Edge **********************//

Pipe::Pipe(Location *orig, Location *dest, double w)
    : orig(orig), dest(dest), weight(w) {
    activated = true;
}


Location* Pipe::getDest() const {
    return dest;
}
double Pipe::getWeight() const {
    return weight;
}
Location* Pipe::getOrig() const {
    return orig;
}
Pipe* Pipe::getReverse() const {
    return reverse;
}
double Pipe::getFlow() const {
    return flow;
}
bool Pipe::isActivated() const {
    return activated;
}


void Pipe::setReverse(Pipe *reverse_) {
    this->reverse = reverse_;
}
void Pipe::setFlow(double flow_) {
    this->flow = flow_;
}

void Pipe::setActivated(bool activated) {
    this->activated = activated;
}



//********************** Graph **********************//

Graph::Graph() {
    vector<Location*> placeholderV;
    unordered_map<string, Reservoir*> placeholderM1;
    unordered_map<string, PumpingStation*> placeholderM2;
    unordered_map<string, City*> placeholderM3;
    unordered_map<string, Pipe*> placeholderM4;

    vertexSet = placeholderV;
    mapReservoirs = placeholderM1;
    mapStations = placeholderM2;
    mapCities = placeholderM3;
    mapPipes = placeholderM4;
}

Graph::Graph(const Graph &g) {
    vertexSet = g.getVertexSet();
    mapReservoirs = g.getMapReservoirs();
    mapStations = g.getMapStations();
    mapCities = g.getMapCities();
    mapPipes = g.getMapPipes();
}


bool Graph::read_file_reservoir(const string &file) {
    ifstream in(file);
    if (!in) return false;
    else {
        string line;
        getline(in, line);
        while (getline(in, line)) {
            istringstream iss(line);
            string word;
            string name;
            string municipality;
            int id;
            string code;
            double maximumDelivery;
            int count = 0;

            while (std::getline(iss, word, ',')) {
                switch (count) {
                    case 0:
                        name = word;
                        break;
                    case 1:
                        municipality = word;
                        break;
                    case 2:
                        id = stoi(word);
                        break;
                    case 3:
                        code = word;
                        break;
                    case 4:
                        maximumDelivery = stod(word);
                        break;
                    default:
                        // Do nothing
                        break;
                }
                count++;
            }
            addVertexReservoir(name, municipality, id, code, maximumDelivery);
        }
        return true;
    }
}

bool Graph::read_file_stations(const string &file) {
    ifstream in(file);
    if (!in) return false;
    else {
        string line;
        getline(in, line);
        while (getline(in, line)) {
            istringstream iss(line);
            string word;
            int id;
            string code;
            int count = 0;

            while (std::getline(iss, word, ',')) {
                switch (count) {
                    case 0:
                        id = stoi(word);
                        break;
                    case 1:
                        code = word;
                        break;
                    default:
                        // Do nothing
                        break;
                }
                count++;
            }
            addVertexStation(id, code);
        }
        return true;
    }
}

bool Graph::read_file_cities(const string &file) {
    ifstream in(file);
    if (!in) return false;
    else {
        string line;
        getline(in, line);
        while (getline(in, line)) {
            istringstream iss(line);
            string word;
            string name;
            int id;
            string code;
            double demand;
            int population;
            string part1;
            string part2;
            int count = 0;

            while (std::getline(iss, word, ',')) {
                switch (count) {
                    case 0:
                        name = word;
                        break;
                    case 1:
                        id = stoi(word);
                        break;
                    case 2:
                        code = word;
                        break;
                    case 3:
                        demand = stod(word);
                        break;
                    case 4:
                        population = stoi(word);
                        break;
                    default:
                        // Do nothing
                        break;
                }
                count++;
            }
            addVertexCity(name, id, code, demand, population);
        }
        return true;
    }
}

bool Graph::read_file_pipes(const string &file) {
    ifstream in(file);
    if (!in) return false;
    else {
        string line;
        getline(in, line);
        while (getline(in, line)) {
            istringstream iss(line);
            string word;
            string service_Point_A;
            string service_Point_B;
            int capacity;
            int direction;
            int count = 0;

            while (std::getline(iss, word, ',')) {
                switch (count) {
                    case 0:
                        service_Point_A = word;
                        break;
                    case 1:
                        service_Point_B = word;
                        break;
                    case 2:
                        capacity = stoi( word);
                        break;
                    case 3:
                        direction = stoi( word);
                        break;
                    default:
                        // Do nothing
                        break;
                }
                count++;
            }
            if (direction) addEdge(service_Point_A, service_Point_B, capacity);
            else addBidirectionalEdge(service_Point_A, service_Point_B, capacity);
        }
        return true;
    }
}



int Graph::getNumVertex() const {
    return vertexSet.size();
}
vector<Location*> Graph::getVertexSet() const {
    return vertexSet;
}

unordered_map<string, Reservoir*> Graph::getMapReservoirs() const {
    return mapReservoirs;
}
unordered_map<string, Reservoir*>& Graph::getMapReservoirs() {
    return mapReservoirs;
}
unordered_map<string, PumpingStation*> Graph::getMapStations() const {
    return mapStations;
}
unordered_map<string, PumpingStation*>& Graph::getMapStations() {
    return mapStations;
}
unordered_map<string, City*> Graph::getMapCities() const {
    return mapCities;
}
unordered_map<string, City*>& Graph::getMapCities() {
    return mapCities;
}
unordered_map<string, Pipe*> Graph::getMapPipes() const {
    return mapPipes;
}
unordered_map<string, Pipe*>& Graph::getMapPipes() {
    return mapPipes;
}



Location* Graph::findVertex(const string &in) const {
    for (auto v : vertexSet) {
        if (v->getCode() == in)
            return v;
    }
    return nullptr;
}

int Graph::findVertexIdx(const string &in) const {
    for (unsigned i = 0; i < vertexSet.size(); i++)
        if (vertexSet[i]->getCode() == in) return i;
    return -1;
}

bool Graph::addVertexReservoir(const string& name, const string& municipality, int id, const string& code, double maximumDelivery) {
    if (mapReservoirs.find(code) != mapReservoirs.end()) return false;
    auto newReservoir = new Reservoir(name, municipality, id, code, maximumDelivery);
    vertexSet.push_back(newReservoir);
    mapReservoirs[code] = newReservoir;
    return true;
}
bool Graph::addVertexStation(int id, const string& code) {
    if (mapStations.find(code) != mapStations.end()) return false;
    auto newStation = new PumpingStation(id,code);
    vertexSet.push_back(newStation);
    mapStations[code] = newStation;
    return true;
}
bool Graph::addVertexCity(const string& name, int id, const string& code, double demand, int population) {
    if (mapCities.find(code) != mapCities.end()) return false;
    auto newCity = new City(name,id,code,demand,population);
    vertexSet.push_back(newCity);
    mapCities[code] = newCity;
    return true;
}

bool Graph::removeVertex(const string &in) {
    for (auto it = vertexSet.begin(); it != vertexSet.end(); it++) {
        if ((*it)->getCode() == in) {
            auto v = *it;
            v->removeOutgoingEdges();
            for (auto u : vertexSet) {
                u->removeEdge(v->getCode());
            }
            vertexSet.erase(it);
            delete v;
            return true;
        }
    }
    return false;
}


bool Graph::addEdge(const string &source, const string &dest, double w) {
    auto v1 = findVertex(source);
    auto v2 = findVertex(dest);
    if (v1 == nullptr || v2 == nullptr) return false;

    auto newPipe = v1->addEdge(v2, w);
    mapPipes[source+'|'+dest] = newPipe;

    return true;
}

bool Graph::removeEdge(const string &source, const string &dest) {
    Location * srcVertex = findVertex(source);
    if (srcVertex == nullptr) return false;
    return srcVertex->removeEdge(dest);
}

bool Graph::addBidirectionalEdge(const string &source, const string &dest, double w) {
    auto v1 = findVertex(source);
    auto v2 = findVertex(dest);
    if (v1 == nullptr || v2 == nullptr) return false;
    auto e1 = v1->addEdge(v2, w);
    auto e2 = v2->addEdge(v1, w);
    e1->setReverse(e2);
    e2->setReverse(e1);

    mapPipes[source+'|'+dest] = e1;
    mapPipes[dest+'|'+source] = e2;

    return true;
}




vector<CityInfo> Graph::getCitiesFlow(bool allCities, const string &oneCity, const string &removeR) {
    Graph g = Graph(*this);

    g.addR0C0(removeR);

    g.edmondsKarp("R_0", "C_0");

    vector<CityInfo> citiesFlow;

    if (allCities) {
        // Associate each city with its respective MaxFlow
        for (const auto& pair : g.mapCities) {
            if (pair.first != "C_0") {
                double maxFlow = 0;
                for (Location *loc : g.vertexSet) {
                    for (Pipe *edge: loc->getAdj()) {
                        if (edge->getDest()->getCode() == pair.first) {
                            maxFlow += edge->getFlow();
                        }
                    }
                }
                pair.second->setTotalFlow(maxFlow);

                CityInfo toPrint = {
                    pair.second->getName(),
                    pair.second->getCode(),
                     pair.second->getDemand(),
                     maxFlow
                };

                citiesFlow.push_back(toPrint);
            }
        }
    }
    else {
        // Associate city with code oneCity with its respective MaxFlow
        auto city = g.mapCities[oneCity];

        double maxFlow = 0;
        for (Location *loc : g.vertexSet) {
            for (Pipe *edge: loc->getAdj()) {
                if (edge->getDest()->getCode() == city->getCode()) {
                    maxFlow += edge->getFlow();
                }
            }
        }
        city->setTotalFlow(maxFlow);

        CityInfo toPrint = {
                city->getName(),
                city->getCode(),
                city->getDemand(),
                maxFlow
        };

        citiesFlow.push_back(toPrint);
    }
    return citiesFlow;
}


void Graph::edmondsKarp(const string &source, const string &target) {
    // Find source and target vertices in the graph
    Location* s = findVertex(source);
    Location* t = findVertex(target);

    // Validate source and target vertices
    if (s == nullptr || t == nullptr || s == t)
        throw std::logic_error("Invalid source and/or target vertex");

    // Initialize flow on all edges to 0
    for (auto v : getVertexSet()) {
        for (auto e: v->getAdj()) {
            e->setFlow(0);
        }
    }

    // While there is an augmenting path, augment the flow along the path
    while (findAugmentingPath(s, t)) {
        double f = findMinResidualAlongPath(s, t);
        augmentFlowAlongPath(s, t, f);
    }
}

bool Graph::findAugmentingPath(Location *s, Location *t) {
    // Mark all vertices as not visited
    for (auto v : getVertexSet()) {
        v->setVisited(false);
    }
    // Mark the source vertex as visited and enqueue it
    s->setVisited(true);
    queue<Location*> q;
    q.push(s);

    // BFS to find an augmenting path
    while (!q.empty() and !t->isVisited()) {
        auto v = q.front();
        q.pop();

        // Process outgoing edges
        for (auto e: v->getAdj()) {
            testAndVisit(q, e, e->getDest(), e->getWeight() - e->getFlow());
        }

        // Process incoming edges
        for (auto e: v->getIncoming()) {
            testAndVisit(q, e, e->getOrig(), e->getFlow());
        }
    }
    // Return true if a path to the target is found, false otherwise
    return t->isVisited();
}
void Graph::testAndVisit(queue<Location*> &q, Pipe *e, Location *w, double residual) {
    // Check if the vertex 'w' is not visited, there is residual capacity, and the edge is activated
    if (!w->isVisited() and residual > 0 and e->isActivated()) {
        // Mark 'w' as visited, set the path through which it was reached, and enqueue it
        w->setVisited(true);
        w->setPath(e);
        q.push(w);
    }
}
double Graph::findMinResidualAlongPath(Location *s, Location *t) {
    double f = INF;

    // Traverse the augmenting path to find the minimum residual capacity
    for (auto v = t; v != s;) {
        auto e = v->getPath();
        if (e->getDest() == v) {
            f = min(f, e->getWeight() - e->getFlow());
            v = e->getOrig();
        }
        else {
            f = min(f, e->getFlow());
            v = e->getDest();
        }
    }
    // Return the minimum residual capacity
    return f;
}

void Graph::augmentFlowAlongPath(Location *s, Location *t, double f) {
    // Traverse the augmenting path and update the flow values accordingly
    for (auto v = t; v != s; ) {
        auto e = v->getPath();
        double flow = e->getFlow();
        if (e->getDest() == v) {
            e->setFlow(flow + f);
            v = e->getOrig();
        }
        else {
            e->setFlow(flow - f);
            v = e->getDest();
        }
    }
}


bool Graph::writeFileMaxFlow() {
    ofstream file("../TotalFlowPerCity.csv");

    if (!file.is_open()) return false;

    file << "City Code,Total Flow" << endl;
    for (const CityInfo& info : getCitiesFlow(true)) {
        file << info.cityCode << "," << info.totalFlow << "\n";
    }
    file.close();
    return true;
}



vector<CityInfo> Graph::verifyCityNeeds() {
    vector<CityInfo> vectorCitiesAffected;

    for (const CityInfo& info : getCitiesFlow(true)) {
        if (info.totalFlow < info.demand) {
            vectorCitiesAffected.push_back(info);
        }
    }

    return vectorCitiesAffected;
}

void Graph::addR0C0(const string &toRemove) {
    if (addVertexReservoir("", "", 0, "R_0", 0) and addVertexCity("",0,"C_0",0,0)) {

        for (const auto& pair: getMapReservoirs()) {
            if (pair.first == toRemove) { //
                addEdge("R_0", pair.first, 0);
            }
            else if (pair.first != "R_0") { // Do not add an Edge to itself
                addEdge("R_0", pair.first, pair.second->getMaximumDelivery());
            }
        }
        for (const auto& pair: getMapCities()) {
            if (pair.first != "C_0") { // Do not add an Edge to itself
                addEdge(pair.first, "C_0", pair.second->getDemand());
            }
        }
    }
}




vector<CityInfo> Graph::removeReservoirCompare(const string &code) {
    vector<CityInfo> mapCityDeficit;

    vector<CityInfo> ogMF = getCitiesFlow(true);
    vector<CityInfo> noR_MF = getCitiesFlow(true, "", code);
    
    for (int i = 0; i < ogMF.size(); i++) {
        // Only add to the affected cities vector, cities whose flow is actually affected
        if (ogMF[i].totalFlow - noR_MF[i].totalFlow > 0) {
            ogMF[i].affectedTotalFlow = noR_MF[i].totalFlow;
            mapCityDeficit.push_back(ogMF[i]);
        }
    }

    return mapCityDeficit;
}


unordered_map<string, vector<CityInfo>>  Graph::removeStationsCompare() {
    unordered_map<string, vector<CityInfo>> mapStationCityDeficit;
    for (const auto& station : mapStations) {
        mapStationCityDeficit[station.first] = removeStationCompare(station.first);
    }
    return mapStationCityDeficit;
}
vector<CityInfo> Graph::removeStationCompare(const string &code) {
    vector<CityInfo> mapCityDeficit;

    vector<CityInfo> ogMF = getCitiesFlow(true);

    // Deactivate Pipes going to PumpingStation (renders station useless)
    for (Pipe* pipe : mapStations[code]->getIncoming()) {
        pipe->setActivated(false);
    }

    vector<CityInfo> noS_MF = getCitiesFlow(true);

    for (int i = 0; i < ogMF.size(); i++) {
        if (ogMF[i].totalFlow - noS_MF[i].totalFlow > 0) {
            ogMF[i].affectedTotalFlow = noS_MF[i].totalFlow;
            mapCityDeficit.push_back(ogMF[i]);
        }
    }

    // Resetting Pipes going to PumpingStation (brings back station)
    for (Pipe* pipe : mapStations[code]->getIncoming()) {
        pipe->setActivated(true);
    }

    return mapCityDeficit;
}


unordered_map<string, vector<CityInfo>>  Graph::removeAllPipesCompare() {
    unordered_map<string, vector<CityInfo>> mapEdgeCityDeficit;

    for (const auto& pipe : mapPipes) {
        istringstream ss(pipe.first);
        string sourceCode;
        string destCode;
        getline(ss, sourceCode, '|');
        getline(ss, destCode);
        mapEdgeCityDeficit[pipe.first] = removePipeCompare(sourceCode, destCode);
    }

    return mapEdgeCityDeficit;
}

vector<CityInfo> Graph::removeMultiplePipesCompare(const set<pair<string, string>> &pipelines) {
    vector<CityInfo> mapCityDeficit;
    vector<CityInfo> ogMF = getCitiesFlow(true);

    // Deactivate Pipes
    for (const auto& pipeline : pipelines) {
        mapPipes[pipeline.first+'|'+pipeline.second]->setActivated(false); // It's guaranteed that this Pipe exists, it has been verified before calling this function.
        if (mapPipes.count(pipeline.second + '|' + pipeline.first) > 0) { // Check if the Pipe in the opposite direction exists.
            mapPipes[pipeline.second+'|'+pipeline.first]->setActivated(false);
        }
    }

    vector<CityInfo> noPs_MF = getCitiesFlow(true);

    for (int i = 0; i < ogMF.size(); i++) {
        if (ogMF[i].totalFlow - noPs_MF[i].totalFlow > 0) {
            ogMF[i].affectedTotalFlow = noPs_MF[i].totalFlow;
            mapCityDeficit.push_back(ogMF[i]);
        }
    }

    // Re-activate Pipes
    for (const auto& pipeline : pipelines) {
        mapPipes[pipeline.first+'|'+pipeline.second]->setActivated(true);
        if (mapPipes.count(pipeline.second + '|' + pipeline.first) > 0) {
            mapPipes[pipeline.second+'|'+pipeline.first]->setActivated(true);
        }
    }

    return mapCityDeficit;
}

vector<CityInfo> Graph::removePipeCompare(const string &sourceCode, const string &destCode) {
    vector<CityInfo> mapCityDeficit;

    vector<CityInfo> ogMF = getCitiesFlow(true);

    // Deactivate Pipe
    mapPipes[sourceCode+'|'+destCode]->setActivated(false);
    if (mapPipes.count(destCode + '|' + sourceCode) == 1) {
        mapPipes[destCode + '|' + sourceCode]->setActivated(false);
    }

    vector<CityInfo> noP_MF = getCitiesFlow(true);

    for (int i = 0; i < ogMF.size(); i++) {
        if (ogMF[i].totalFlow - noP_MF[i].totalFlow > 0) {
            ogMF[i].affectedTotalFlow = noP_MF[i].totalFlow;
            mapCityDeficit.push_back(ogMF[i]);
        }
    }

    // Re-activate Pipe
    mapPipes[sourceCode+'|'+destCode]->setActivated(true);
    if (mapPipes.count(destCode + '|' + sourceCode) == 1) {
        mapPipes[destCode + '|' + sourceCode]->setActivated(true);
    }

    return mapCityDeficit;
}

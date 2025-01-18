#include "Menu.hpp"

#include <string>
#include <set>
#include <iostream>

using namespace std;


Current::Current() {
    currentMenu = new MainMenu(0);
    network = Graph();
    datasetParsed = 0;

    triApproxDist = 0;
    triApproxTime = 0;
}

bool Current::isInt(const string& input) {
    if (input.empty()) return false;
    for (char c : input) {
        if (!isdigit(c)) return false;
    }
    return true;
}

void Current::run() {
    while (currentMenu != nullptr) {
        Menu* nextMenu;
        string input;

        currentMenu->print();

        getline(cin, input); // To be able to read whitespaces possibly present in the user input
        cout << "\n\n\n";

        if (isInt(input)) nextMenu = currentMenu->handleInput(this, stoi(input), "");
        else nextMenu = currentMenu->handleInput(this, -1, input);

        if (nextMenu != currentMenu) delete currentMenu;
        currentMenu = nextMenu;
    }
}

Current::~Current() {
    delete currentMenu;
}


Graph Current::getNetwork() const {
    return network;
}
Graph& Current::getNetwork() {
    return network;
}
uint8_t Current::getParsed() const {
    return datasetParsed;
}
bool Current::isParsed() const {
    return datasetParsed == 0x01 or datasetParsed == 0x02 or datasetParsed == 0x04;
}
double Current::getTriApproxDist() const {
    return triApproxDist;
}
double Current::getTriApproxTime() const {
    return triApproxTime;
}

void Current::resetNetwork() {
    network = Graph();
}
void Current::resetParsed() {
    datasetParsed = 0x00;
}
void Current::resetCompares() {
    triApproxDist = 0;
    triApproxTime = 0;
}

void Current::setParsed(unsigned int pos) {
    datasetParsed |= (1 << pos);
}
void Current::setTriApproxDist(double dist) {
    triApproxDist = dist;
}
void Current::setTriApproxTime(double time) {
    triApproxTime = time;
}



MainMenu::MainMenu(uint8_t datasetParsed) {
    this->datasetParsed = datasetParsed;
}

void MainMenu::print() {
    string titleMessage;
    if (datasetParsed == 0x01) titleMessage = "!A Toy-Graph dataset was parsed!";
    else if (datasetParsed == 0x02) titleMessage = "!A Medium-Size Graph dataset was parsed!";
    else if (datasetParsed == 0x04) titleMessage = "!A Real-World Graph dataset was parsed!";
    string message2 = (datasetParsed != 0x00 and datasetParsed != 0x01) ? " (Too slow for graphs other than Toy-Graphs!)" : "" ;
    string menuText = string("=================================================================================\n")
                              + "                  Welcome to the Water Supply Management System                  \n"
                              + "---------------------------------------------------------------------------------\n"
                              + "Insert the number corresponding to the option you wish to select:\n"
                              + string(datasetParsed == 0 ? "!A DATASET NEEDS TO PARSED BEFORE ANY OTHER OPTION IS SELECTED!\n\n" : titleMessage + "\n\n")
                              + "1. Parse a dataset\n"
                              + "2. Execute the backtracking algorithm" + message2  + "\n"
                              + "3. Execute the algorithm with Triangular Approximation Heuristic\n"
                              + "4. Execute the algorithm with Nearest Neighbour Heuristic\n"
                              + "5. Execute the algorithm ...\n"
                              + "6. Exit\n"
                              + "=================================================================================\n"
                              + "Option: ";
    cout << menuText;
}
Menu* MainMenu::handleInput(Current* menu, int option, string input) {
    Menu* nextMenu;
    vector<int> path;
    double currPathDist;
    double otherPathDist;
    double currTime;
    double otherTime;

    size_t lineLength = 0;

    // Doesn't let any option other than datasets or exiting the tool be selected if a dataset isn't parsed yet.
    if (option != 1 and option != 6 and !menu->isParsed()) {
        cout << "=================================================================================\n";
        cout << "ERROR: A dataset has not been parsed yet!\n";
        return new FooterMenu();
    }

    switch (option) {
        case 1:
            nextMenu = new MenuParseDataset();
            break;
        case 2: // [T4.1] Backtracking Algorithm - starting on node with label 0
            cout << "=================================================================================\n";
            path = menu->getNetwork().backtrackingTSP(0, currPathDist, currTime);
            cout << "Backtracking Algorithm path distance: " + to_string(currPathDist) + "\n";
            cout << "Backtracking Algorithm time taken: " + to_string(currTime) + "\n";
            cout << "Path:\n";
            for (size_t i = 0; i < path.size(); ++i) {
                if (lineLength + to_string(path[i]).length() + string("-->").length() > 80) {
                    cout << "\n -->";
                    lineLength = string("-->").length();
                }
                lineLength += to_string(path[i]).length() + string("-->").length();

                cout << path[i];
                if (i != path.size() - 1) cout << "-->";
            }
            cout << "\n";
            nextMenu = new FooterMenu();
            break;
        case 3: // [T4.2] Triangular Approximation Heuristic - starting on node with label 0
            cout << "=================================================================================\n";
            path = menu->getNetwork().triangularApproximationTSP(0, currPathDist, currTime);
            menu->setTriApproxDist(currPathDist);
            menu->setTriApproxTime(currTime);

            cout << "Triangular Approximation Heuristic:\n";
            cout << " Path distance: " + to_string(currPathDist) + "\n";
            cout << " Time taken: " + to_string(currTime) + " seconds\n";
            cout << "Path:\n";
            for (size_t i = 0; i < path.size(); ++i) {
                if (lineLength + to_string(path[i]).length() + string("-->").length() > 80) {
                    cout << "\n -->";
                    lineLength = string("-->").length();
                }
                lineLength += to_string(path[i]).length() + string("-->").length();

                cout << path[i];
                if (i != path.size() - 1) cout << "-->";
            }
            cout << "\n";
            nextMenu = new FooterMenu();
            break;
        case 4: // [T4.3] Nearest Neighbour Heuristic - starting on node with label 0
            cout << "=================================================================================\n";
            if (menu->getTriApproxDist() == 0 and menu->getTriApproxTime() == 0) {
                // Run if the Triangular Approximation Heuristic hasn't been run yet
                // Don't need the path, only the distance and time
                menu->getNetwork().triangularApproximationTSP(0, otherPathDist, otherTime);
            }
            else {
                otherPathDist = menu->getTriApproxDist();
                otherTime = menu->getTriApproxTime();
            }
            path = menu->getNetwork().nearestNeighbourTSP(0, currPathDist, currTime);
            cout << "Nearest Neighbor Heuristic:\n";
            cout << " Path distance: " + to_string(currPathDist) + "\n";
            cout << " Time taken: " + to_string(currTime) + " seconds\n\n";
            cout << "Comparison:\n";
            cout << " Triangular Approximation Heuristic path distance: " + to_string(otherPathDist) + "\n";
            cout << " Nearest Neighbor path distance: " + to_string(currPathDist) + "\n";
            cout << " Nearest Neighbor's efficiency is " + to_string(currPathDist / otherPathDist) + " times better than Triangular Approximation\n\n";
            cout << " Triangular Approximation Heuristic time taken: " + to_string(otherTime) + " seconds\n";
            cout << " Nearest Neighbor time taken: " + to_string(currTime) + " seconds\n";
            cout << " Nearest Neighbor is " + to_string(otherTime / currTime) + " times faster than Triangular Approximation\n\n";
            cout << "Path:\n";
            for (size_t i = 0; i < path.size(); ++i) {
                if (lineLength + to_string(path[i]).length() + string("-->").length() > 80) {
                    cout << "\n -->";
                    lineLength = string("-->").length();
                }
                lineLength += to_string(path[i]).length() + string("-->").length();

                cout << path[i];
                if (i != path.size() - 1) cout << "-->";
            }
            cout << "\n";
            nextMenu = new FooterMenu();
            break;
        case 5: // [T4.4] ... Heuristic
            nextMenu = new Menu5();
            break;
        case 6: // Exit the tool
            nextMenu = nullptr;
            break;
        default:
            cout << "\n=================================================================================\n";
            cout << "INVALID OPTION! TRY AGAIN!\n";
            nextMenu = this;
            break;
    }
    return nextMenu;
}



void MenuParseDataset::print() {
    string menuText = string("=================================================================================\n")
                              + "                    Choose the type of file you wish to parse                    \n"
                              + "---------------------------------------------------------------------------------\n"
                              + "1. Parse Shipping Toy-Graph\n" // Not fully connected - [T2.4]
                              + "2. Parse Stadiums Toy-Graph\n" // Fully connected - [T2.2] & [T2.3]
                              + "3. Parse Tourism Toy-Graph\n"  // Fully connected - [T2.2] & [T2.3]
                              + "4. Parse Real-World Graph 1\n"
                              + "5. Parse Real-World Graph 2\n"
                              + "6. Parse Real-World Graph 3\n"
                              + "7. Parse Medium-Size Graph - 25 nodes\n" // All fully connected
                              + "8. Parse Medium-Size Graph - 50 nodes\n"
                              + "9. Parse Medium-Size Graph - 75 nodes\n"
                              + "10. Parse Medium-Size Graph - 100 nodes\n"
                              + "11. Parse Medium-Size Graph - 200 nodes\n"
                              + "12. Parse Medium-Size Graph - 300 nodes\n"
                              + "13. Parse Medium-Size Graph - 400 nodes\n"
                              + "14. Parse Medium-Size Graph - 500 nodes\n"
                              + "15. Parse Medium-Size Graph - 600 nodes\n"
                              + "16. Parse Medium-Size Graph - 700 nodes\n"
                              + "17. Parse Medium-Size Graph - 800 nodes\n"
                              + "18. Parse Medium-Size Graph - 900 nodes\n"
                              + "19. Back to Main Menu\n"
                              + "=================================================================================\n"
                              + "Option: ";
    cout << menuText;
}

Menu* MenuParseDataset::handleInput(Current* menu, int option, string input) {
    Menu* nextMenu;

    if (!menu->isParsed() or option == 19) { // If a dataset isn't parsed or the user wishes to go back to the main menu
        switch (option) {
            // Toy-Graphs
            case 1:
                if (menu->getNetwork().read_edges("../dataset/Toy-Graphs/shipping.csv")) menu->setParsed(0);
                break;
            case 2:
                if (menu->getNetwork().read_edges("../dataset/Toy-Graphs/stadiums.csv")) menu->setParsed(0);
                break;
            case 3:
                if (menu->getNetwork().read_edges("../dataset/Toy-Graphs/tourism.csv")) menu->setParsed(0);
                break;
            // Real-World Graphs
            case 4:
                if (menu->getNetwork().read_nodes("../dataset/Real-World-Graphs/graph1/nodes.csv") and
                    menu->getNetwork().read_edges("../dataset/Real-World-Graphs/graph1/edges.csv")) menu->setParsed(2);
                break;
            case 5:
                if (menu->getNetwork().read_nodes("../dataset/Real-World-Graphs/graph2/nodes.csv");
                menu->getNetwork().read_edges("../dataset/Real-World-Graphs/graph2/edges.csv")) menu->setParsed(2);
                break;
            case 6:
                if (menu->getNetwork().read_nodes("../dataset/Real-World-Graphs/graph3/nodes.csv") and
                menu->getNetwork().read_edges("../dataset/Real-World-Graphs/graph3/edges.csv")) menu->setParsed(2);
                break;
            // Medium-Size Graphs
            case 7:
                if (menu->getNetwork().read_edges("../dataset/Extra-Fully-Connected-Graphs/edges_25.csv", false)) menu->setParsed(1);
                break;
            case 8:
                if (menu->getNetwork().read_edges("../dataset/Extra-Fully-Connected-Graphs/edges_50.csv", false)) menu->setParsed(1);
                break;
            case 9:
                if (menu->getNetwork().read_edges("../dataset/Extra-Fully-Connected-Graphs/edges_75.csv", false)) menu->setParsed(1);
                break;
            case 10:
                if (menu->getNetwork().read_edges("../dataset/Extra-Fully-Connected-Graphs/edges_100.csv", false)) menu->setParsed(1);
                break;
            case 11:
                if (menu->getNetwork().read_edges("../dataset/Extra-Fully-Connected-Graphs/edges_200.csv", false)) menu->setParsed(1);
                break;
            case 12:
                if (menu->getNetwork().read_edges("../dataset/Extra-Fully-Connected-Graphs/edges_300.csv", false)) menu->setParsed(1);
                break;
            case 13:
                if (menu->getNetwork().read_edges("../dataset/Extra-Fully-Connected-Graphs/edges_400.csv", false)) menu->setParsed(1);
                break;
            case 14:
                if (menu->getNetwork().read_edges("../dataset/Extra-Fully-Connected-Graphs/edges_500.csv", false)) menu->setParsed(1);
                break;
            case 15:
                if (menu->getNetwork().read_edges("../dataset/Extra-Fully-Connected-Graphs/edges_600.csv", false)) menu->setParsed(1);
                break;
            case 16:
                if (menu->getNetwork().read_edges("../dataset/Extra-Fully-Connected-Graphs/edges_700.csv", false)) menu->setParsed(1);
                break;
            case 17:
                if (menu->getNetwork().read_edges("../dataset/Extra-Fully-Connected-Graphs/edges_800.csv", false)) menu->setParsed(1);
                break;
            case 18:
                if (menu->getNetwork().read_edges("../dataset/Extra-Fully-Connected-Graphs/edges_900.csv", false)) menu->setParsed(1);
                break;
            case 19:
                nextMenu = new MainMenu(menu->getParsed());
                break;
            default:
                cout << "\n=================================================================================\n";
                cout << "INVALID OPTION! TRY AGAIN!\n";
                break;
        }
        if (option >= 1 and option <= 18) nextMenu = new ParseSuccessMenu(menu->isParsed());
    }
    // If a dataset has been parsed, if the user wishes to parse another dataset, either he deletes the data or he uses the already loaded one.
    else nextMenu = new ResetMenu();

    return nextMenu;
}



void Menu5::print() {
    string menuText = string("=================================================================================\n")
                              + "  Insert the identifier label (integer) of the starting node for the algorithm.  \n"
                              + "---------------------------------------------------------------------------------\n"
                              + "                               a.Back to Main Menu                               \n"
                              + "=================================================================================\n"
                              + "Node ID/Option: ";
    cout << menuText;
}

Menu* Menu5::handleInput(Current* menu, int option, string input) {
    Menu* nextMenu;
    vector<int> path;

    if (option == -1 and (input == "a" or input == "A")) nextMenu = new MainMenu(menu->getParsed());
    else if (option < -1) { // Option is now the node's identifier label - needs to be a positive integer
        cout << "\nINVALID OPTION! TRY AGAIN!\n";
        nextMenu = this;
    }
    else {
        if (option > menu->getNetwork().getNumVertex()) {
            cout << "\n=================================================================================\n";
            cout << "NODE NOT IN THE GRAPH! THE GRAPH ONLY HAS " + to_string(menu->getNetwork().getNumVertex()) + " NODES. TRY AGAIN!\n";
            nextMenu = this;
        }
        else {
            cout << "=================================================================================\n";
            cout << "... Heuristic:\n";
            cout << "Path:\n";
            /*
            path = ;
            if (!path.empty()) {
                for (size_t i = 0; i < path.size(); ++i) {
                    if (lineLength + to_string(path[i]).length() + string("-->").length() > 80) {
                        cout << "\n -->";
                        lineLength = string("-->").length();
                    }
                    lineLength += to_string(path[i]).length() + string("-->").length();

                    cout << path[i];
                    if (i != path.size() - 1) cout << "-->";
                }
                cout << "\n";
             }
             else {
                 cout << "No path exists with that starting node\n";
             }
             */
            nextMenu = new FooterMenu();
        }
    }
    return nextMenu;
}


void FooterMenu::print() {
    string menuText = string("---------------------------------------------------------------------------------\n")
                              + "                               1.Back to Main Menu                               \n"
                              + "=================================================================================\n"
                              + "Option: ";
    cout << menuText;
}
Menu* FooterMenu::handleInput(Current* menu, int option, string input) {
    Menu* nextMenu;
    if (option == 1) nextMenu = new MainMenu(menu->getParsed());
    else {
        cout << "\n=================================================================================\n";
        cout << "INVALID OPTION! TRY AGAIN!\n";
        nextMenu = this;
    }
    return nextMenu;
}


ParseSuccessMenu::ParseSuccessMenu(bool datasetSuccess) {
    datasetParsed = datasetSuccess;

}

void ParseSuccessMenu::print() {
    string message = datasetParsed ? "The dataset has been successfully parsed!" : "There was an error while parsing the dataset.";
    string menuText = string("=================================================================================\n")
                              + message + "\n"
                              + "---------------------------------------------------------------------------------\n"
                              + "                               1.Back to Main Menu                               \n"
                              + "=================================================================================\n"
                              + "Option: ";
    cout << menuText;
}
Menu* ParseSuccessMenu::handleInput(Current* menu, int option, string input) {
    Menu* nextMenu;
    if (option == 1) nextMenu = new MainMenu(menu->getParsed());
    else {
        cout << "\n=================================================================================\n";
        cout << "INVALID OPTION! TRY AGAIN!\n";
        nextMenu = this;
    }
    return nextMenu;
}

void ResetMenu::print() {
    string menuText = string("=================================================================================\n")
                              + "You have already parsed a dataset and loaded the data necessary to use this tool.\n"
                              + "To avoid the mixing of data of 2 different datasets:\n"
                              + "Do you wish to delete the loaded data and parse a new dataset or keep the data?\n"
                              + "---------------------------------------------------------------------------------\n"
                              + "1. Delete the data (Go to the Parsing Menu)\n"
                              + "2. Keep the data (Go to the Main Menu)\n"
                              + "=================================================================================\n"
                              + "Option: ";
    cout << menuText;
}
Menu* ResetMenu::handleInput(Current* menu, int option, string input) {
    Menu* nextMenu;
    if (option == 1) {
        menu->resetNetwork();
        menu->resetParsed();
        menu->resetCompares();

        nextMenu = new MenuParseDataset();
    }
    else if (option == 2) nextMenu = new MainMenu(menu->getParsed());
    else {
        cout << "\n=================================================================================\n";
        cout << "INVALID OPTION! TRY AGAIN!\n";
        nextMenu = this;
    }
    return nextMenu;
}

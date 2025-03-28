#include "Menu.hpp"

#include <string>
#include <set>
#include <iostream>
#include <sstream>

using namespace std;


Current::Current() {
    currentMenu = new MainMenu();
    network = Graph();
    filesParsed = 0;
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
uint8_t Current::getFilesParsed() const {
    return filesParsed;
}

void Current::resetNetwork() {
    network = Graph();
}
void Current::resetParsed() {
    filesParsed = 0x00;
}

void Current::setParsed(unsigned int pos) {
    filesParsed |= (1 << pos);
}


void MainMenu::print() {
    string menuText = string("=================================================================================\n")
                              + "                  Welcome to the Water Supply Management System                  \n"
                              + "---------------------------------------------------------------------------------\n"
                              + "Insert the number corresponding to the option you wish to select:\n"
                              + "!EACH TYPE OF FILE NEEDS TO PARSED ONCE BEFORE ANY OTHER OPTION IS SELECTED!\n\n"
                              + "1. Parse the data files\n"
                              + "2. Maximum amount of water that can reach each city\n"
                              + "3. Maximum amount of water that can reach a specific city\n"
                              + "4. Write the maximum amount of water that can reach each city to a file\n"
                              + "5. Can all the water reservoirs supply enough water to all their delivery sites?\n"
                              + "6. Remove a water reservoir and see the affected cities\n"
                              + "7. Remove a pumping station and see the affected cities\n"
                              + "8. Remove each pumping station and see the respective affected cities\n"
                              + "9. Remove one or more pipelines and see the affected cities\n"
                              + "10. Remove each pipeline and see the respective affected cities\n"
                              + "11. Exit\n"
                              + "=================================================================================\n"
                              + "Option: ";
    cout << menuText;
}
Menu* MainMenu::handleInput(Current* menu, int option, string input) {
    Menu* nextMenu;
    double maxFlow = 0;

    // Doesn't let any option other than parsing files or exiting the tool be selected if not all the 4 files are parsed yet.
    if (option != 1 and option != 11 and menu->getFilesParsed() != 0x0F) {
        cout << "=================================================================================\n";
        cout << "ERROR: Not all 4 types of files have been parsed yet!\n";
        return new FooterMenu();
    }

    switch (option) {
        case 1: // [T1.2]
            nextMenu = new Menu1Choose();
            break;
        case 2: // [T2.1]
            cout << "=================================================================================\n";
            for (const CityInfo& info : menu->getNetwork().getCitiesFlow(true)) {
                cout << "(" << info.cityCode << "," << info.cityName << ") - Demand: " << info.demand << " | Total Flow: "
                     << info.totalFlow << " (m^3/sec)\n";
                maxFlow += info.totalFlow;
            }
            cout << "The maximum flow is: " << maxFlow << " m^3/sec \n";
            nextMenu = new FooterMenu();
            break;
        case 3: // [T2.1]
            nextMenu = new Menu3();
            break;
        case 4: // [T2.1]
            cout << "=================================================================================\n";
            if (menu->getNetwork().writeFileMaxFlow()) {
                cout << "File 'TotalFlowFilePerCity.csv' edited/created successfully.\n"
                     << "These effects will be reflected upon exiting the tool.\n";
            }
            else cout << "Failed to open file.\n";
            nextMenu = new FooterMenu();
            break;
        case 5: // [T2.2]
            cout << "=================================================================================\n";
            if (const auto& cityNeeds = menu->getNetwork().verifyCityNeeds(); cityNeeds.empty()) {
                cout << "Yes, all the reservoirs can supply enough water to all the cities!\n";
            }
            else {
                cout << "No! " << string((cityNeeds.size() == 1) ? "The city whose demand is not met is:\n" : "The cities whose demand is not met are:\n");
                for (const CityInfo& info : cityNeeds) {
                    cout << "(" << info.cityCode << "," << info.cityName << "):\n"
                         << "  Demand: " << info.demand << " m^3/sec\n"
                         << "  Total Flow: " << info.totalFlow << " m^3/sec\n"
                         << "  Deficit: " << info.demand - info.totalFlow << " m^3/sec\n";
                }
            }
            nextMenu = new FooterMenu();
            break;
        case 6: // [T3.1]
            nextMenu = new Menu6();
            break;
        case 7: // Extra feature related to [T3.2]
            nextMenu = new Menu7();
            break;
        case 8: // [T3.2]
            cout << "=================================================================================\n";
            for (const auto& pair : menu->getNetwork().removeStationsCompare()) {
                if (pair.second.empty()) cout << "If station " + pair.first + " is removed, no cities are affected.\n";
                else {
                    cout << "If the Pumping Station " << pair.first << " is removed, the affected " << string((pair.second.size() == 1) ? "city is: " : "cities are: ") << "\n";
                    for (const CityInfo& info : pair.second) {
                        cout << "  (" << info.cityCode << "," << info.cityName << ") - Old Flow: " << info.totalFlow << "; New Flow: " << info.affectedTotalFlow << " (m^3/sec)\n";
                        // Uncomment for extra info:
                        // << string(("  (" + info.cityName + "," + info.cityCode + ") - ").size(),' ')
                        // << "Demand: " <<  info.demand << " | Flow with PS: " << info.totalFlow << " (m^3/sec) Flow without PS: " << info.affectedTotalFlow << " (m^3/sec)\n";
                    }
                }
            }
            nextMenu = new FooterMenu();
            break;
        case 9: // Extra feature related to [T3.3]
            nextMenu = new Menu9();
            break;
        case 10: // [T3.3]
            cout << "=================================================================================\n";
            for (const auto& pair : menu->getNetwork().removeAllPipesCompare()) {
                if (pair.second.empty()) cout << "If Pipeline " + pair.first + " is removed, no cities are affected.\n";
                else {
                    cout << "If the Pipeline " << pair.first << " is removed, the affected " << string((pair.second.size() == 1) ? "city is: " : "cities are: ") << "\n";
                    for (const CityInfo& info : pair.second) {
                        cout << "  (" << info.cityCode << "," << info.cityName << ") - Deficit: " << info.totalFlow - info.affectedTotalFlow << " m^3/sec\n";
                        // Uncomment for extra info:
                        // << string(("  (" + info.cityName + "," + info.cityCode + ") - ").size(),' ')
                        // << "Demand: " <<  info.demand << " | Flow with Pipeline: " << info.totalFlow << " (m^3/sec) Flow without Pipeline: " << info.affectedTotalFlow << " (m^3/sec)\n";
                    }
                }
            }
            nextMenu = new FooterMenu();
            break;
        case 11: // Exit the tool
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




void Menu1Choose::print() {
    string menuText = string("=================================================================================\n")
                              + "                    Choose the type of file you wish to parse                    \n"
                              + "---------------------------------------------------------------------------------\n"
                              + "1. Parse a Water Reservoir file\n"
                              + "2. Parse a Pumping Station file\n"
                              + "3. Parse a Delivery Sites file\n"
                              + "4. Parse a Pipelines file\n"
                              + "5. Parse the default small dataset (Madeira dataset - 4 files)\n"
                              + "6. Parse the default big dataset (Continental Portugal - 4 files)\n"
                              + "7. Back to Main Menu\n"
                              + "=================================================================================\n"
                              + "Option: ";
    cout << menuText;
}

Menu* Menu1Choose::handleInput(Current* menu, int option, string input) {
    Menu* nextMenu;

    if (menu->getFilesParsed() != 0x0F) { // If not all files are parsed
        switch (option) {
            case 1: case 2: case 3: case 4:
                nextMenu = new Menu1(option);
                break;
            case 5: // Madeira Dataset Parsing
                // Tries to parse all files, but if just 1 of them fails, it prints a failure message
                if (menu->getNetwork().read_file_reservoir("../dataset/Reservoirs_Madeira.csv")) menu->setParsed(0);
                if (menu->getNetwork().read_file_stations("../dataset/Stations_Madeira.csv")) menu->setParsed(1);
                if (menu->getNetwork().read_file_cities("../dataset/Cities_Madeira.csv")) menu->setParsed(2);
                if (menu->getNetwork().read_file_pipes("../dataset/Pipes_Madeira.csv")) menu->setParsed(3);
                nextMenu = new ParseSuccessMenu(menu->getFilesParsed() == 0x0F, menu->getFilesParsed() == 0x0F);
                break;
            case 6:
                // Tries to parse all files, but if just 1 of them fails, it prints a failure message
                if (menu->getNetwork().read_file_reservoir("../dataset/Reservoir.csv")) menu->setParsed(0);
                if (menu->getNetwork().read_file_stations("../dataset/Stations.csv")) menu->setParsed(1);
                if (menu->getNetwork().read_file_cities("../dataset/Cities.csv")) menu->setParsed(2);
                if (menu->getNetwork().read_file_pipes("../dataset/Pipes.csv")) menu->setParsed(3);
                nextMenu = new ParseSuccessMenu(menu->getFilesParsed() == 0x0F, menu->getFilesParsed() == 0x0F);
                break;
            case 7:
                nextMenu = new MainMenu();
                break;
            default: // Continental Portugal Dataset Parsing
                cout << "\n=================================================================================\n";
                cout << "INVALID OPTION! TRY AGAIN!\n";
                nextMenu = this;
                break;
        }
    }
    else nextMenu = new ResetMenu(); // If all files have been parsed, if the user wishes to parse another file, either he deletes the data or he uses the already loaded one.

    return nextMenu;
}


Menu1::Menu1(unsigned int file_type) {
    this->fileType = file_type;
}

void Menu1::print() {
    string menuText = string("=================================================================================\n")
                              + "               Insert the full path of the file you wish to parse.               \n"
                              + "---------------------------------------------------------------------------------\n"
                              + "                               1.Back to Main Menu                               \n"
                              + "=================================================================================\n"
                              + "Path/Option: ";
    cout << menuText;
}

Menu* Menu1::handleInput(Current* menu, int option, string input) {
    Menu* nextMenu;

    bool parse_success = true;
    switch (option) {
        case -1:
            switch (fileType) {
                case 1:
                    parse_success = menu->getNetwork().read_file_reservoir(input);
                    if (parse_success) menu->setParsed(0);
                    break;
                case 2:
                    parse_success = menu->getNetwork().read_file_stations(input);
                    if (parse_success) menu->setParsed(1);
                    break;
                case 3:
                    parse_success = menu->getNetwork().read_file_cities(input);
                    if (parse_success) menu->setParsed(2);
                    break;
                case 4:
                    parse_success = menu->getNetwork().read_file_pipes(input);
                    if (parse_success) menu->setParsed(3);
                    break;
            }
            nextMenu = new ParseSuccessMenu(parse_success, menu->getFilesParsed() == 0x0F);
            break;
        case 1:
            nextMenu = new MainMenu();
            break;
        default:
            cout << "\n=================================================================================\n";
            cout << "INVALID OPTION! TRY AGAIN!\n";
            nextMenu = this;
            break;
    }
    return nextMenu;
}


void Menu3::print() {
    string menuText = string("=================================================================================\n")
                              + "                  Insert the code that corresponds to the city.                  \n"
                              + "---------------------------------------------------------------------------------\n"
                              + "Example: C_1, C_2, C_3, C_4, ...\n"
                              + "---------------------------------------------------------------------------------\n"
                              + "                               1.Back to Main Menu                               \n"
                              + "=================================================================================\n"
                              + "Code/Option: ";
    cout << menuText;
}

Menu* Menu3::handleInput(Current* menu, int option, string input) {
    Menu* nextMenu;

    switch (option) {
        case -1:
            if (menu->getNetwork().getMapCities().find(input) != menu->getNetwork().getMapCities().end()) {
                cout << "=================================================================================\n";
                for (const CityInfo& info: menu->getNetwork().getCitiesFlow(false, input)) {
                    cout << "(" << info.cityCode << "," << info.cityName << ") - Demand: " << info.demand << " | Total Flow: "
                         << info.totalFlow << " (m^3/sec)\n";
                }
                nextMenu = new FooterMenu();
            }
            else {
                cout << "City code not found! Check your input.\n";
                nextMenu = this;
            }
            break;
        case 1:
            nextMenu = new MainMenu();
            break;
        default:
            cout << "\n=================================================================================\n";
            cout << "INVALID OPTION! TRY AGAIN!\n";
            nextMenu = this;
            break;
    }
    return nextMenu;
}


void Menu6::print() {
    string menuText = string("=================================================================================\n")
                              + "             Insert the code that corresponds to the water reservoir             \n"
                              + "---------------------------------------------------------------------------------\n"
                              + "Example: R_1, R_2, R_3, R_4, ...\n"
                              + "---------------------------------------------------------------------------------\n"
                              + "                               1.Back to Main Menu                               \n"
                              + "=================================================================================\n"
                              + "Code/Option: ";
    cout << menuText;
}

Menu* Menu6::handleInput(Current* menu, int option, string input) {
    Menu* nextMenu;
    switch (option) {
        case -1:
            if (menu->getNetwork().getMapReservoirs().find(input) != menu->getNetwork().getMapReservoirs().end()) {
                cout << "=================================================================================\n";
                if (const vector<CityInfo>& affectedCities = menu->getNetwork().removeReservoirCompare(input); affectedCities.empty()) {
                    cout << "If the Water Reservoir " << input << "is removed, no cities are affected.\n";
                }
                else {
                    cout << "If the Water Reservoir " << input << " is removed, the affected " << string((affectedCities.size() == 1) ? "city is: " : "cities are: ") << "\n";
                    for (const CityInfo &info : affectedCities) {
                        cout << "  (" << info.cityCode << "," << info.cityName << ") - Old Flow: " << info.totalFlow << "; New Flow: " << info.affectedTotalFlow << " (m^3/sec)\n";
                    }
                }
                nextMenu = new FooterMenu();

            }
            else {
                cout << "Reservoir code not found! Check your input.\n";
                nextMenu = this;
            }
            break;
        case 1:
            nextMenu = new MainMenu();
            break;
        default:
            cout << "\n=================================================================================\n";
            cout << "INVALID OPTION! TRY AGAIN!\n";
            nextMenu = this;
            break;
    }
    return nextMenu;
}


void Menu7::print() {
    string menuText = string("=================================================================================\n")
                              + "             Insert the code that corresponds to the pumping station             \n"
                              + "---------------------------------------------------------------------------------\n"
                              + "Example: PS_1, PS_2, PS_3, PS_4, ...\n"
                              + "---------------------------------------------------------------------------------\n"
                              + "                               1.Back to Main Menu                               \n"
                              + "=================================================================================\n"
                              + "Code/Option: ";
    cout << menuText;
}

Menu* Menu7::handleInput(Current* menu, int option, string input) {
    Menu* nextMenu;
    switch (option) {
        case -1:
            if (menu->getNetwork().getMapStations().find(input) != menu->getNetwork().getMapStations().end()) {
                cout << "=================================================================================\n";
                if (const vector<CityInfo>& affectedCities = menu->getNetwork().removeStationCompare(input); affectedCities.empty()) {
                    cout << "If the Pumping Station " << input << "is removed, no cities are affected.\n";
                }
                else {
                    cout << "If the Pumping Station " << input << " is removed, the affected " << string((affectedCities.size() == 1) ? "city is: " : "cities are: ") << "\n";
                    for (const CityInfo &info : affectedCities) {
                        cout << "  (" << info.cityCode << "," << info.cityName << ") - Old Flow: " << info.totalFlow << "; New Flow: " << info.affectedTotalFlow << " (m^3/sec)\n";
                        // Uncomment for extra info:
                        // << string(("(" + info.cityName + "," + info.cityCode + ") - ").size(),' ')
                        // << "Demand: " << info.demand << " | Flow w/ reservoir: " << info.totalFlow << "\n"
                        // << string(("(" + info.cityName + "," + info.cityCode + ") - ").size(),' ')
                        // << "Flow w/o reservoir: " << info.affectedTotalFlow << "\n";
                    }
                }
                nextMenu = new FooterMenu();

            }
            else {
                cout << "Pumping Station code not found! Check your input.\n";
                nextMenu = this;
            }
            break;
        case 1:
            nextMenu = new MainMenu();
            break;
        default:
            cout << "\n=================================================================================\n";
            cout << "INVALID OPTION! TRY AGAIN!\n";
            nextMenu = this;
            break;
    }
    return nextMenu;
}


void Menu9::print() {
    string menuText = string("=================================================================================\n")
                              + "Insert the codes that correspond to the source and destination of the pipeline.\n"
                              + "You can also insert several pipelines to be removed at once. Follow the examples:\n"
                              + "---------------------------------------------------------------------------------\n"
                              + "Single pipeline removal: The codes must be separated by a vertical bar, '|'\n"
                              + "- Format: source_location_code|destination_location_code\n"
                              + "- Example: PS_3|PS_4; R_1|PS_1; PS_11|C_2; ...\n"
                              + "Multiple pipeline removal: Each pair of codes must be separated by a comma, ','\n"
                              + "- Format: src_loc_code|dest_loc_code,src_loc_code|dest_loc_code,...\n"
                              + "- Example: PS_3|PS_4,R_1|PS_1; R_4|PS_10,PS_1|C_10; ...\n"
                              + "---------------------------------------------------------------------------------\n"
                              + "                               1.Back to Main Menu                               \n"
                              + "=================================================================================\n"
                              + "Code/Option: ";
    cout << menuText;
}

Menu* Menu9::handleInput(Current* menu, int option, string input) {
    Menu* nextMenu;

    istringstream inputLine(input);
    string sourceCode, destCode;
    set<pair<string, string>> pipelines;

    if (input.find(',') != string::npos) { // Multiple pipeline input
        option = -2;

        // Get start and end position of first pair
        size_t startPos = 0;
        size_t endPos = input.find(',');
        while (endPos != string::npos) {
            string src_dest = input.substr(startPos, endPos - startPos);
            size_t splitPos = src_dest.find('|');
            if (splitPos != string::npos) pipelines.emplace(src_dest.substr(0, splitPos), src_dest.substr(splitPos + 1));
            else option = -3; // Input is incorrectly formatted

            // Search for the next pair (prepare next iteration)
            startPos = endPos + 1;
            endPos = input.find(',', startPos);
        }

        // Last pair (also works if only 1 pipeline)
        string lastPair = input.substr(startPos);
        size_t splitPos = lastPair.find('|');
        if (splitPos != string::npos) pipelines.emplace(lastPair.substr(0, splitPos), lastPair.substr(splitPos + 1));
        else option = -3; // Input is incorrectly formatted
    }
    else { // Single pipeline input
        if (!input.empty()) {
            getline(inputLine, sourceCode, '|');
            getline(inputLine, destCode);

            if (inputLine.fail()) option = -3;
        }
    }


    switch (option) {
        case -3:
            cout << "The input is incorrectly formatted! Check your input.\n";
            nextMenu = this;
            break;
        case -2:
              for (const auto& pipeline : pipelines) {
                  if (menu->getNetwork().getMapPipes().find(pipeline.first+'|'+pipeline.second) == menu->getNetwork().getMapPipes().end()) {
                      cout << "Either the source's, the destination's, or both codes of one of the pairs were not found!\n"
                           << "Check your input!\n";
                      nextMenu = this;
                      break;
                  }
              }
              if (nextMenu != this) {
                  cout << "=================================================================================\n";
                  if (const vector<CityInfo>& affectedCities = menu->getNetwork().removeMultiplePipesCompare(pipelines); affectedCities.empty()) {
                      cout << "If the " << string((pipelines.size() == 1) ? "Pipeline" : "Pipelines") << ":\n";
                      for (const auto& pipeline : pipelines) {
                          cout << " - " << pipeline.first << '|' << pipeline.second << "\n";
                      }
                      cout << string((pipelines.size() == 1) ? "is removed" : "are removed") << ", no cities are affected.\n";
                  }
                  else {
                      cout << "If the " << string((pipelines.size() == 1) ? "Pipeline" : "Pipelines") << ":\n";
                      for (const auto& pipeline : pipelines) {
                          cout << " - " << pipeline.first << '|' << pipeline.second << "\n";
                      }
                      cout << string((pipelines.size() == 1) ? "is removed" : "are removed") << ", the affected " << string((affectedCities.size() == 1) ? "city is: " : "cities are: ") << "\n";
                      for (const CityInfo &info : affectedCities) {
                          cout << "  (" << info.cityCode << "," << info.cityName << ") - Old Flow: " << info.totalFlow << "; New Flow: " << info.affectedTotalFlow << " (m^3/sec)\n";
                          // Uncomment for extra info:
                          // << string(("(" + info.cityName + "," + info.cityCode + ") - ").size(),' ')
                          // << "Demand: " << info.demand << " | Flow w/ reservoir: " << info.totalFlow << "\n"
                          // << string(("(" + info.cityName + "," + info.cityCode + ") - ").size(),' ')
                          // << "Flow w/o reservoir: " << info.affectedTotalFlow << "\n";
                      }
                  }
                  nextMenu = new FooterMenu();
              }
              break;
        case -1:
            if (menu->getNetwork().getMapPipes().find(input) != menu->getNetwork().getMapPipes().end()) {
                cout << "=================================================================================\n";
                if (const vector<CityInfo>& affectedCities = menu->getNetwork().removePipeCompare(sourceCode, destCode); affectedCities.empty()) {
                    cout << "If the Pipeline " << input << " is removed, no cities are affected.\n";
                }
                else {
                    cout << "If the Pipeline " << input << " is removed, the affected " << string((affectedCities.size() == 1) ? "city is: " : "cities are: ") << "\n";
                    for (const CityInfo &info : affectedCities) {
                        cout << "  (" << info.cityCode << "," << info.cityName << ") - Old Flow: " << info.totalFlow << "; New Flow: " << info.affectedTotalFlow << " (m^3/sec)\n";
                        // Uncomment for extra info:
                        // << string(("(" + info.cityName + "," + info.cityCode + ") - ").size(),' ')
                        // << "Demand: " << info.demand << " | Flow w/ reservoir: " << info.totalFlow << "\n"
                        // << string(("(" + info.cityName + "," + info.cityCode + ") - ").size(),' ')
                        // << "Flow w/o reservoir: " << info.affectedTotalFlow << "\n";
                    }
                }
                nextMenu = new FooterMenu();

            }
            else {
                cout << "Either the source's, the destination's, or both codes were not found!\n"
                     << "Check your input!\n";
                nextMenu = this;
            }
            break;
        case 1:
            nextMenu = new MainMenu();
            break;
        default:
            cout << "\n=================================================================================\n";
            cout << "INVALID OPTION! TRY AGAIN!\n";
            nextMenu = this;
            break;
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
    if (option == 1) nextMenu = new MainMenu();
    else {
        cout << "\n=================================================================================\n";
        cout << "INVALID OPTION! TRY AGAIN!\n";
        nextMenu = this;
    }
    return nextMenu;
}


ParseSuccessMenu::ParseSuccessMenu(bool cfSuccess, bool afSuccess) {
    this->currFiles = cfSuccess;
    this->allFiles = afSuccess;
}

void ParseSuccessMenu::print() {
    string message = currFiles ? (string((allFiles) ? "All the 4 files were" : "The file was") + " parsed successfully!") : "There was an error while parsing the file.";
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
    if (option == 1) nextMenu = new MainMenu();
    else {
        cout << "\n=================================================================================\n";
        cout << "INVALID OPTION! TRY AGAIN!\n";
        nextMenu = this;
    }
    return nextMenu;
}

void ResetMenu::print() {
    string menuText = string("=================================================================================\n")
                              + "You have already parsed 4 files and loaded the data necessary to use this tool.\n"
                              + "To avoid the mixing of data of 2 different water supply systems, please answer\n"
                              + "the following question:\n"
                              + "Do you wish to delete the loaded data and parse new files or keep the data?\n"
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

        nextMenu = new Menu1Choose();
    }
    else if (option == 2) nextMenu = new MainMenu();
    else {
        cout << "\n=================================================================================\n";
        cout << "INVALID OPTION! TRY AGAIN!\n";
        nextMenu = this;
    }
    return nextMenu;
}

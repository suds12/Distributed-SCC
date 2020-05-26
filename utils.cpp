//
// Created by norris on 5/25/20.
//

#include <string>
#include <iostream>
#include "utils.hpp"

using namespace std;

#define PRINT_HELP_AND_EXIT {cout << desc << endl; exit(1);}
#define CHECK_REQUIRED(optname)  {if (options.count(optname)) {\
        cout << optname << " file:  " << options[optname].as<string>() << "." << endl; \
    } else { \
        cerr << "Required " << optname << " file was not provided." << endl; \
        PRINT_HELP_AND_EXIT \
    }}

void process_options(int argc, char* argv[], po::variables_map& options) {
    // Declare the supported options.
    po::options_description desc("Options:");
    desc.add_options()
            ("help,h", "produce help message")
            ("graph,g", po::value<string>(), "input graph file")
            ("changes,c", po::value<string>(), "file containing list of edges to add/delete")
            ("scc,s", po::value<string>(), "file containing initial SCC")
            ;

    po::store(po::parse_command_line(argc, argv, desc), options);
    po::notify(options);

    if (options.count("help")) PRINT_HELP_AND_EXIT;

    CHECK_REQUIRED("graph");
    CHECK_REQUIRED("changes");
    CHECK_REQUIRED("scc");

}
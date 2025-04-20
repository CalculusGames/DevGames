#include <iostream>
#include <format>
#include <string>
#include <unordered_map>
#include <sstream>

#include <CLI/CLI.hpp>

#ifndef DEVGAMES_VERSION
#define DEVGAMES_VERSION "unknown"
#endif

#ifndef DEVGAMES_BUILTIN_LIST
#define DEVGAMES_BUILTIN_LIST ""
#endif

#ifndef DEVGAMES_BUILTIN_INSTALL_DIR
#define DEVGAMES_BUILTIN_INSTALL_DIR ""
#endif

#ifndef DEVGAMES_THIRD_PARTY_INSTALL_DIR
#define DEVGAMES_THIRD_PARTY_INSTALL_DIR ""
#endif

#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__) || defined(__MINGW32__)
#define DEVGAMES_EXECUTABLE_EXT ".exe"
#define FILE_SEPARATOR "\\"
#else
#define DEVGAMES_EXECUTABLE_EXT ""
#define FILE_SEPARATOR "/"
#endif

CLI::App app{"Terminal games for bored developers"};
std::unordered_map<std::string, std::string> game_map{};

std::vector<std::string> split(const std::string& s, const std::string& del) {
    std::vector<std::string> tokens;

    size_t pos = 0;
    std::string token;
    std::string temp = s;
    while ((pos = temp.find(del)) != std::string::npos) {
        token = temp.substr(0, pos);
        tokens.push_back(token);
        temp.erase(0, pos + del.length());
    }
    tokens.push_back(temp);

    return tokens;
}

void findThirdPartyGames(std::vector<std::string>& list) {
    std::stringstream path;
    path << DEVGAMES_THIRD_PARTY_INSTALL_DIR << "/games.txt";
    std::ifstream file(path.str());
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            list.push_back(line);

            std::stringstream game_path;
            game_path << DEVGAMES_THIRD_PARTY_INSTALL_DIR << FILE_SEPARATOR << line;
            std::ifstream game_file(game_path.str());
            if (!game_file.is_open()) {
                std::cerr << "Third-Party Game '" << line << "' not found in " << DEVGAMES_THIRD_PARTY_INSTALL_DIR << std::endl;
                continue;
            }
            game_map[line] = game_path.str();
        }
        file.close();
    }
}

int main(int argc, char** argv) {
    argv = app.ensure_utf8(argv);
    app.usage("game <game|command> [options]");
    app.allow_windows_style_options();
    app.set_version_flag("--version", std::format("devgames v{}", DEVGAMES_VERSION), "Show DevGames version information");

    app.failure_message(CLI::FailureMessage::help);
    app.require_subcommand(1);

    std::vector<std::string> list = split(DEVGAMES_BUILTIN_LIST, ",");
    std::vector<std::string> list0;
    std::copy_if(list.begin(), list.end(), std::back_inserter(list0), [](const std::string& game) { return !game.empty(); });

    for (const auto& game : list0) {
        std::stringstream path;
        path << DEVGAMES_BUILTIN_INSTALL_DIR << FILE_SEPARATOR << game;

        if (!std::filesystem::exists(path.str())) {
            std::cerr << "Game '" << game << "' not found in " << DEVGAMES_BUILTIN_INSTALL_DIR << std::endl;
            continue;
        }
        game_map[game] = path.str();
    }
    findThirdPartyGames(list0);

    if (list.empty()) {
        std::cerr << "No games found. Please check your installation." << std::endl;
        return 1;
    }

    bool verbose = false;

    // list
    app.add_subcommand("list", "List all available games")
        ->callback([&]() {
            if (verbose) {
                std::cout << "Searching in " << DEVGAMES_BUILTIN_INSTALL_DIR << " and " << DEVGAMES_THIRD_PARTY_INSTALL_DIR << std::endl;
                std::cout << "Found " << list0.size() << " games." << std::endl;
            }
    
            std::cout << "Available games:" << std::endl;
            for (const auto& game : list0) {
                std::cout << " - " << game << std::endl;
            }
        })
        ->add_flag("-v,--verbose", verbose, "Enable verbose output");

    // run
    for (const std::string& game : list0) {
        const std::string path = game_map[game];
        if (path.empty()) {
            std::cerr << "Game '" << game << "' not found in the game map." << std::endl;
            continue;
        }

        std::stringstream executable_stream;
        executable_stream << path << FILE_SEPARATOR << game << DEVGAMES_EXECUTABLE_EXT;
        const std::string executable = executable_stream.str();

        if (!std::filesystem::exists(executable)) {
            std::cerr << "Game executable '" << executable << "' not found." << std::endl;
            continue;
        }

        app.add_subcommand(game, "Run the '" + game + "' game")
            ->callback([game, path, executable, &verbose]() {
                if (verbose) {
                    std::cout << "Running " << game << " at '" << path << "' ..." << std::endl;
                }

                // TODO Validate Hashing of the game executable

                if (verbose) std::cout << "Executing command: " << executable << std::endl;

                std::stringstream command_stream;
                command_stream << "\"" << executable << "\"";
                std::string command = command_stream.str();
                if (verbose) std::cout << "Command: " << command << std::endl;

                std::system(command.c_str());
            })
            ->add_flag("-v,--verbose", verbose, "Enable verbose output");
    }

    CLI11_PARSE(app, argc, argv);
    return 0;
}
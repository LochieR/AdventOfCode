#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>

struct Set {
    int Red = 0, Green = 0, Blue = 0;
};

struct Game {
    int ID;
    int NumSets;
    Set Sets[5];
};

int CountOccurrences(const std::string& str, char targetChar)
{
    int count = 0;
    for (char ch : str)
    {
        if (ch == targetChar)
        {
            count++;
        }
    }
    return count;
}

std::vector<Game> ReadGamesFromFile(std::string_view path)
{
    std::ifstream file(path.data());

    std::vector<Game> games;

    std::string line;
    while (std::getline(file, line))
    {
        Game game{};

        std::istringstream iss(line);
        std::string token;

        iss >> token; // 'Game'
        iss >> game.ID; // Game ID

        char colon;
        iss >> colon; // ':'
        
        int numSets = CountOccurrences(line, ';') + 1;

        game.NumSets = numSets;

        for (int i = 0; i < numSets; i++)
        {
            Set set{};
            std::string fullSet;
            if (i != numSets - 1)
                std::getline(iss, fullSet, ';');
            else
                std::getline(iss, fullSet);

            std::istringstream setISS(fullSet);
            
            while (!setISS.eof())
            {
                int count;
                setISS >> count;

                std::string color;
                setISS >> color;

                char c = color[color.size() - 1];
                if (!(c >= 'a' && c <= 'z'))
                    color.pop_back();

                if (color == "red")
                    set.Red = count;
                else if (color == "green")
                    set.Green = count;
                else if (color == "blue")
                    set.Blue = count;
            }

            game.Sets[i] = set;
        }

        games.push_back(game);
    }

    file.close();

    return games;
}

int main()
{
    auto games = ReadGamesFromFile("inputs/Day2.txt");

    for (const auto& game : games) {
        std::cout << "Game ID: " << game.ID << "\nSets:\n";
        for (const auto& set : game.Sets) {
            std::cout << " Red: " << set.Red << ", Green: " << set.Green << ", Blue: " << set.Blue << std::endl;
        }
        std::cout << std::endl;
    }
    return 0;
}

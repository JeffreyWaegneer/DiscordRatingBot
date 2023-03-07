#include <vector>
#include "Player.h"
#include <iostream>>

void PrintTrueSkills(std::vector<Player>& players)
{
    // Print the updated skill levels of the players
    for (int i = 0; i < players.size(); i++)
    {
        std::cout << "Player " << i << ": Rating = " << players[i].getRating() << std::endl;
    }
}

// The following function simulates a match between two players and updates their skill levels
void simulateMatch(Player& winner, Player& loser)
{
    float winnerRating = winner.getRating();
    float loserRating = loser.getRating();

    winner.updateSkillLevel(loserRating, true);
    loser.updateSkillLevel(winnerRating, false);
}

// The following function simulates a series of matches and returns the updated skill levels of the players
std::vector<Player> simulateSeries(std::vector<Player> players, std::vector<std::pair<int, int>> matches)
{
    for (auto m : matches)
    {
        simulateMatch(players[m.first], players[m.second]);
        PrintTrueSkills(players);
    }

    return players;
}

int temp()
{
    // Create a set of players with their initial skill levels
    std::vector<Player> players = { Player(1200), Player(1000), Player(1200), Player(800) };

    // Simulate a series of matches
    std::vector<std::pair<int, int>> matches = { {0, 1} };
    players = simulateSeries(players, matches);
    PrintTrueSkills(players);

    return 0;
}
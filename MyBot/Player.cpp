#include "Player.h"
#include <math.h>
#include <fstream>
#include <iostream>
#include <sstream>

int Player::K = 32;
std::vector<Player*> Player::Players;
std::string Player::Filename;

float Player::CalculateProbability(int ratingPlayer1, int ratingPlayer2)
{
    return 1.0 
        / (1 
            + pow(10, 
                1.0 * (ratingPlayer1 - ratingPlayer2) / 400));
}

// Computes the player's win probability against another player
double Player::getWinProbability(float otherRating) const
{
    return CalculateProbability(otherRating, this->_rating);
}

// Updates the player's skill level based on the outcome of a match
void Player::updateSkillLevel(float otherRating, bool win)
{
    float probWin = CalculateProbability(otherRating, this->_rating);

    if (win)
        this->_rating = this->_rating + K * (1 - probWin);
    else
        this->_rating = this->_rating + K * (0 - probWin);
}

void Player::updateSkillLevel(float myTeamRating, float otherTeamRating, bool win)
{
    float probWin = CalculateProbability(otherTeamRating, myTeamRating);

    if (win)
        this->_rating = this->_rating + K * (1 - probWin);
    else
        this->_rating = this->_rating + K * (0 - probWin);
}

float Player::getRating()
{
    return _rating;
}

void Player::GetPlayersFromFile()
{
    for (size_t i = 0; i < Players.size(); i++)
    {
        free(Players[i]);
        Players[i] = nullptr;
    }
    Player::Players.clear();

    // Open the input file
    std::ifstream infile(Filename);

    // Check if the file was opened successfully
    if (!infile.is_open()) 
    {
        std::cerr << "Error: could not open file " << Filename << std::endl;
    }
    else
    {
        // Loop through each line of the file
        std::string line;
        while (std::getline(infile, line))
        {
            // Split the line into columns
            std::stringstream ss(line);
            std::string name;
            std::getline(ss, name, ',');
            std::string ratingStr;
            std::getline(ss, ratingStr, ',');
            float rating = std::stof(ratingStr);
            Player* p = new Player(name, rating);
            Player::Players.push_back(p);
        }
    }
}

void Player::Init(std::string filename)
{
    Filename = filename;
    GetPlayersFromFile();
}

void Player::SavePlayers()
{
    // Open the output file
    std::ofstream outfile(Filename);

    // Check if the file was opened successfully
    if (!outfile.is_open()) {
        std::cerr << "Error: could not open file " << Filename << std::endl;
        throw "Could not open file";
    }

    outfile.clear();

    for (size_t i = 0; i < Player::Players.size(); i++)
    {
        Player* p = Player::Players[i];
        outfile << p->_name << "," << std::to_string(p->_rating) << std::endl;
    }

    // Close the output file
    outfile.close();
    std::cout << "User data saved successfully!" << std::endl;
}

Player* Player::GetPlayer(std::string name)
{
    for (size_t i = 0; i < Players.size(); i++)
    {
        if (Players[i]->_name == name)
            return Players[i];
    }

    Player* newPlayer = new Player(name);
    Players.push_back(newPlayer);

    return newPlayer;
}

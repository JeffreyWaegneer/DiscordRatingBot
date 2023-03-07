#pragma once
#include <string>
#include <vector>
#include <algorithm>
class Player
{
private:
    static int K;
    static float CalculateProbability(int ratingPlayer1, int ratingPlayer2);
    static std::vector<Player *> Players;
    static void GetPlayersFromFile();
    static std::string Filename;

    std::string _name;
    float _rating;

    static bool compareRatings(Player* one, Player* two)
    {
        return one->_rating > two->_rating;
    }

public:
    Player() : _name("Temp"), _rating(1200) { }
    Player(std::string name) : _name(name), _rating(1200) { }
    Player(float rating) : _name("Temp"), _rating(1200) { }
    Player(std::string name, float rating) : _name(name), _rating(rating) { }
    


    // Computes the player's win probability against another player
    double getWinProbability(float otherRating) const;

    // Updates the player's skill level based on the outcome of a match
    void updateSkillLevel(float otherRating, bool win);

    void updateSkillLevel(float myTeamRating, float otherTeamRating, bool win);

    //Getters and setters
    float getRating();
    std::string getName()
    {
        return _name;
    }
    
    static void Init(std::string filename);
    static void SavePlayers();
    static Player* GetPlayer(std::string name);
    static std::vector<Player*> GetOrderedList()
    {
        std::sort(Players.begin(), Players.end(), compareRatings);
        return Players;
    }
};
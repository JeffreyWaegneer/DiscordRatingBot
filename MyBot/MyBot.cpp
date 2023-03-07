#include <dpp/dpp.h>
#include "Player.h"
#include <vector>

/* Be sure to place your token in the line below.
 * Follow steps here to get a token:
 * https://dpp.dev/creating-a-bot-application.html
 * When you invite the bot, be sure to invite it with the 
 * scopes 'bot' and 'applications.commands', e.g.
 * https://discord.com/oauth2/authorize?client_id=940762342495518720&scope=bot+applications.commands&permissions=139586816064
 */
const std::string BOT_TOKEN_FILENAME = "discord.config";

const std::string GetToken()
{
    // Open the input file
    std::ifstream infile(BOT_TOKEN_FILENAME);

    // Check if the file was opened successfully
    if (!infile.is_open())
    {
        std::cerr << "Error: could not open file " << BOT_TOKEN_FILENAME << std::endl;
        return "";
    }
    else
    {
        // Loop through each line of the file
        std::string token;
        std::getline(infile, token);
        return token;
    }
}

Player* GetPlayerFromEvent(const dpp::slashcommand_t& event, std::string name)
{
    dpp::snowflake snowflake;

    try
    {
        snowflake = std::get<dpp::snowflake>(event.get_parameter(name));
    }
    catch (const std::exception&)
    {
        return nullptr;
    }

    dpp::user* userPtr = dpp::find_user(snowflake);
    return Player::GetPlayer(userPtr->get_mention());
}

int main()
{
    /* Create bot cluster */
    dpp::cluster bot(GetToken());

    /* Output simple log messages to stdout */
    bot.on_log(dpp::utility::cout_logger());

    /* Handle slash command */
    bot.on_slashcommand([](const dpp::slashcommand_t& event) 
    {
        Player::Init(event.command.get_guild().name + event.command.get_channel().name + ".elo");
        dpp::command_interaction cmd_data = std::get<dpp::command_interaction>(event.command.data);
        if (cmd_data.name == "match") 
        {
            unsigned short winTeamSize = 1;
            Player* winner = GetPlayerFromEvent(event, "winner");
            Player* winner2 = GetPlayerFromEvent(event, "winner2");
            Player* winner3 = GetPlayerFromEvent(event, "winner3");

            unsigned short loseTeamSize = 1;
            Player* loser = GetPlayerFromEvent(event, "loser");
            Player* loser2 = GetPlayerFromEvent(event, "loser2");
            Player* loser3 = GetPlayerFromEvent(event, "loser3");

            float winnerRating = winner->getRating();
            float loserRating = loser->getRating();

            if (winner2 != nullptr)
            {
                winnerRating += winner2->getRating();
                winTeamSize++;
            }
            if (winner3 != nullptr)
            {
                winnerRating += winner3->getRating();
                winTeamSize++;
            }
            if (loser2 != nullptr)
            {
                loserRating += loser2->getRating();
                loseTeamSize++;
            }
            if (loser3 != nullptr)
            {
                loserRating += loser3->getRating();
                loseTeamSize++;
            }

            float averageRatingWinners = winnerRating / winTeamSize;
            float averageRatingLosers = loserRating / loseTeamSize;

            winner->updateSkillLevel(averageRatingWinners, averageRatingLosers, true);
            if (winner2 != nullptr) winner2->updateSkillLevel(averageRatingWinners, averageRatingLosers, true);
            if (winner3 != nullptr) winner3->updateSkillLevel(averageRatingWinners, averageRatingLosers, true);
            loser->updateSkillLevel(averageRatingLosers, averageRatingWinners, false);
            if (loser2 != nullptr) loser2->updateSkillLevel(averageRatingLosers, averageRatingWinners, false);
            if (loser3 != nullptr) loser3->updateSkillLevel(averageRatingLosers, averageRatingWinners, false);

            Player::SavePlayers();

            std::string reply = winner->getName() + " new rating is: " + std::to_string(winner->getRating()) + "\n";
            if (winner2 != nullptr) reply += winner2->getName() + " new rating is: " + std::to_string(winner2->getRating()) + "\n";
            if (winner3 != nullptr) reply += winner3->getName() + " new rating is: " + std::to_string(winner3->getRating()) + "\n";
            reply += loser->getName() + " new rating is " + std::to_string(loser->getRating()) + "\n";
            if (loser2 != nullptr) reply += loser2->getName() + " new rating is: " + std::to_string(loser2->getRating()) + "\n";
            if (loser3 != nullptr) reply += loser3->getName() + " new rating is: " + std::to_string(loser3->getRating()) + "\n";

            event.reply(reply);
        }
        else if (cmd_data.name == "scoreboard")
        {
            std::vector<Player *> orderedPlayerlist = Player::GetOrderedList();
            std::string reply = "";
            if (orderedPlayerlist.empty())
                reply += "No users on the scoreboard.";
            for (size_t i = 0; i < orderedPlayerlist.size(); i++)
            {
                if (i == 0)
                    reply += ":first_place: : ";
                else if (i == 1)
                    reply += ":second_place: : ";
                else if (i == 2)
                    reply += ":third_place: : ";
                else
                    reply += std::to_string(i) + "th : ";

                reply += orderedPlayerlist[i]->getName() + "\n";
            }

            event.reply(reply);
        }
        else if (cmd_data.name == "rating")
        {
            Player* player = GetPlayerFromEvent(event, "player");
            if (player == nullptr) player = Player::GetPlayer(event.command.get_issuing_user().get_mention());

            std::string reply = "";
            reply += player->getName() + " rating: " + std::to_string(player->getRating());

            event.reply(reply);
        }
    });

    /* Register slash command here in on_ready */
    bot.on_ready([&bot](const dpp::ready_t& event)
    {

        /* Wrap command registration in run_once to make sure it doesnt run on every full reconnection */
        if (dpp::run_once<struct register_bot_commands>())
        {
            std::vector<dpp::slashcommand> commands;

            dpp::slashcommand matchcommand("match", "Add results of a match", bot.me.id);
            matchcommand.add_option(dpp::command_option(dpp::co_user, "winner", "Winner of the match", true));
            matchcommand.add_option(dpp::command_option(dpp::co_user, "loser", "Loser of the match", true));
            matchcommand.add_option(dpp::command_option(dpp::co_user, "loser2", "Second member of the losing team", false));
            matchcommand.add_option(dpp::command_option(dpp::co_user, "loser3", "Third member of the losing team", false));
            matchcommand.add_option(dpp::command_option(dpp::co_user, "winner2", "Second member of the winning team", false));
            matchcommand.add_option(dpp::command_option(dpp::co_user, "winner3", "Third member of the winning team", false));
            commands.push_back(matchcommand);

            dpp::slashcommand scoreboardcommand("scoreboard", "Get the scoreboard", bot.me.id);
            commands.push_back(scoreboardcommand);

            dpp::slashcommand getratingcommand("rating", "Get your rating", bot.me.id);
            getratingcommand.add_option(dpp::command_option(dpp::co_user, "player", "The rating to return", false));
            commands.push_back(getratingcommand);

            bot.global_bulk_command_create(commands);
        }
    });

    /* Start the bot */
    bot.start(dpp::st_wait);

    return 0;
}

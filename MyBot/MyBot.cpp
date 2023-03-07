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
const std::string BOT_TOKEN = "NzY4MjE0NTk3MjY1NTIyNzM4.GErPIk.OsHDxuu1vJ43pa8voLd9HHZJigmApkDIW5jUCQ";

int main()
{
    Player::Init("test.txt");

    /* Create bot cluster */
    dpp::cluster bot(BOT_TOKEN);

    /* Output simple log messages to stdout */
    bot.on_log(dpp::utility::cout_logger());

    /* Handle slash command */
    bot.on_slashcommand([](const dpp::slashcommand_t& event) 
    {
        dpp::command_interaction cmd_data = std::get<dpp::command_interaction>(event.command.data);
        if (cmd_data.name == "match") 
        {
            dpp::user* winner = dpp::find_user(std::get<dpp::snowflake>(event.get_parameter("winner")));
            dpp::user* loser = dpp::find_user(std::get<dpp::snowflake>(event.get_parameter("loser")));

            //dpp::snowflake winner2 = std::get<dpp::snowflake>(event.get_parameter("winner2"));

            Player* winnerPl = Player::GetPlayer(winner->get_mention());
            Player* loserPl = Player::GetPlayer(loser->get_mention());

            float winnerRating = winnerPl->getRating();
            float loserRating = loserPl->getRating();

            winnerPl->updateSkillLevel(loserRating, true);
            loserPl->updateSkillLevel(winnerRating, false);

            Player::SavePlayers();

            std::string reply = winner->get_mention() + " new rating is: " + std::to_string(winnerPl->getRating()) + "\n" + loser->get_mention() + " new rating is " + std::to_string(loserPl->getRating());

            event.reply(reply);
        }
        else if (cmd_data.name == "scoreboard")
        {
            std::vector<Player *> orderedPlayerlist = Player::GetOrderedList();
            std::string reply = "";
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

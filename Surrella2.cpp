#include <dpp/dpp.h>

#include "utils/Definitions.h"
#include "utils/Log.h"

#include <mpg123.h>

UPOINTER<DISCORD> Bot;

void PlaySong(const dpp::voiceconn* voice, const std::string& url) {
    LOG_INFO("Playing song: {}", url);

    std::vector<uint8_t> pcmdata;

    mpg123_init();

    int err = 0;
    size_t done;
    int channels, encoding;
    long rate;

    mpg123_handle* mh = mpg123_new(nullptr, &err);
    mpg123_param(mh, MPG123_FORCE_RATE, 48000, 48000.0);

    VAL bufferSize = mpg123_outblock(mh);
    VAL buffer = new unsigned char[bufferSize];

    mpg123_open(mh, ""); //path removed for privacy
    mpg123_getformat(mh, &rate, &channels, &encoding);

    unsigned int counter = 0;
    for (int totalBytes = 0; mpg123_read(mh, buffer, bufferSize, &done) == MPG123_OK;) {
        for (auto i = 0; i < bufferSize; i++) {
            pcmdata.push_back(buffer[i]);
        }
        counter += bufferSize;
        totalBytes += done;
    }
    delete[] buffer;
    mpg123_close(mh);
    mpg123_delete(mh);

    if (voice && voice->voiceclient && voice->voiceclient->is_ready()) {
        voice->voiceclient->send_audio_raw(reinterpret_cast<uint16_t*>(pcmdata.data()), pcmdata.size());
    }
}

void LoadCommands(DISCORD& discord) {
    const dpp::slashcommand joinCommand("join", "Joins user's channel", discord.me.id);
    dpp::slashcommand playCommand("play", "Plays a song", discord.me.id);

    playCommand.add_option(dpp::command_option(dpp::co_string, "url", "The song to play", true));

    discord.on_slashcommand([&discord](const dpp::slashcommand_t& event) {
        VAL guild = find_guild(event.command.guild_id);
        VAL sender = event.command.member;

        if (event.command.get_command_name() == "join") {
            VAR output = "Joining voice channel...";

            if (!guild->connect_member_voice(sender.user_id, false, true)) {
                output = "Failed to join voice channel!";
                LOG_ERROR("User is not in a voice channel!");
            }
            else {
                LOG_INFO("Joined voice channel!");
            }

            event.reply(output);
        }
        else if (event.command.get_command_name() == "play") {
            VAL song = std::get<std::string>(event.get_parameter("url"));

            if (!guild->connect_member_voice(sender.user_id, false, true)) {
                event.reply("Failed to join voice channel!");
                LOG_ERROR("User is not in a voice channel!");
                return;
            }

            LOG_INFO("Joined voice channel!");

            event.reply("Playing song: " + song);

            PlaySong(event.from->get_voice(guild->id), song);
        }
    });

    discord.global_command_create(joinCommand);
    discord.global_command_create(playCommand);
}

auto Init(const std::string& token) {
    Bot = CREATE_UPOINTER(DISCORD, token,
                          dpp::i_default_intents | dpp::i_guild_voice_states | dpp::i_guild_messages | dpp::
                          i_guild_message_reactions | dpp::i_guilds | dpp::i_guild_members | dpp::i_guild_presences |
                          dpp::i_direct_messages | dpp::i_direct_message_reactions);

    Bot->on_ready([bot=&Bot](const dpp::ready_t& event) {
        VAL instance = bot->get();

        LOG_INFO("Bot is ready! Logged in as {}#{}", instance->me.username, instance->me.discriminator);
    });

    LoadCommands(*Bot);

    Bot->start(true);

    for (std::string line; line != "exit"; std::getline(std::cin, line)) {
        if (line == "hello") {
            std::cout << "Hello, world!" << std::endl;
        }
    }
}

int main(const int argc, char* argv[]) {
    InitLogger();

    if (argc < 2) {
        LOG_ERROR("No token provided!");
        return 1;
    }

    LOG_INFO("D++ version: {}", dpp::utility::version());

    LOG_INFO("Starting bot...");

    Init(argv[1]);
}

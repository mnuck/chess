// This is where you build your AI for the Chess game.

#ifndef JOUEUR_CHESS_AI_H
#define JOUEUR_CHESS_AI_H

#include "chess.h"
#include "game.h"
#include "gameObject.h"
#include "piece.h"
#include "player.h"

#include "../../joueur/baseAI.h"

#include "BixNix/Engine.h"

class Chess::AI : public Joueur::BaseAI
{
    public:
        /// <summary>
        /// This is a pointer to the Game object itself,
        /// it contains all the information about the current game
        /// </summary>
        Chess::Game* game;

        /// <summary>
        /// This is a pointer to your AI's player. This AI class 
        /// is not a player, but it should command this Player.
        /// </summary>
        Chess::Player* player;

        /// <summary>
        /// This returns your AI's name to the game server.
        /// Just replace the string.
        /// </summary>
        /// <returns>string of you AI's name.</returns>
        std::string getName();

        /// <summary>
        /// This is automatically called when the game first starts, 
        /// once the Game object and all GameObjects have been initialized,
        /// but before any players do anything.
        /// </summary>
        void start();

        /// <summary>
        /// This is automatically called every time the game 
        /// (or anything in it) updates.
        /// </summary>
        void gameUpdated();

        /// <summary>
        /// This is automatically called when the game ends.
        /// </summary>
        /// <param name="won">true if your player won, false otherwise</param>
        /// <param name="reason">a string explaining why you 
        /// won or lost</param>
        void ended(bool won, std::string reason);

        /// <summary>
        /// This is called every time the AI is asked to respond with a
        /// command during their turn
        /// </summary>
        /// <returns>represents if you want to end your turn. true means
        /// end the turn, false means to keep your turn going and re-call
        /// runTurn()</returns>
        bool runTurn();

protected:
        /// <summary>
        /// Converts a SIG-Game formatted move to BixNix format
        /// </summary>
        BixNix::Move siggame2bixnix(Chess::Move* move);

        /// <summary>
        /// Converts a BixNix move to SIG-Game format and sends it
        /// </summary>
        void sendBixNixMove(const BixNix::Move&);

        /// <summary>
        /// The BixNix engine
        /// </summary>
        BixNix::Engine _engine;
};

#endif

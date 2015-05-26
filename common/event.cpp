#include "event.h"
#include <cassert>

using namespace std;

const char* toString(Event ev)
{
    switch (ev) {
    case Event::GAME_INIT:
        return "GAME_INIT";

    case Event::ROUND_INIT:
        return "ROUND_INIT";

    case Event::DEAL_CARD:
        return "DEAL_CARD";

    case Event::ASK_FOR_BID:
        return "ASK_FOR_BID";

    case Event::BID_PLACED:
        return "BID_PLACED";

    case Event::GOT_BID:
        return "GOT_BID";

    case Event::COLOR_SELECTED:
        return "COLOR_SELECTED";

    case Event::PUT_CARDS_ON_TABLE:
        return "PUT_CARDS_ON_TABLE";

    case Event::KILL_CARD:
        return "KILL_CARD";

    case Event::BEGIN_PLAY:
        return "BEGIN_PLAY";

    case Event::PLAY_CARD:
        return "PLAY_CARD";

    case Event::PLAY_ROUND_FINISH:
        return "PLAY_ROUND_FINISH";

    case Event::PLAYER_COLD:
        return "PLAYER_COLD";

    case Event::ROUND_OVER:
        return "ROUND_OVER";

    case Event::GAME_OVER:
        return "GAME_OVER";

    default:
        assert(!true);
        return "UNKNOWN";
    }
}

#ifndef EVENT_H
#define EVENT_H


enum event_type
{
    GAME_INIT = 0,
    ROUND_INIT,
    DEAL_CARD,
    ASK_FOR_BID,
    BID_PLACED,
    GOT_BID,
    COLOR_SELECTED,
    PUT_CARDS_ON_TABLE,
    KILL_CARD,
    BEGIN_PLAY,
    PLAY_CARD,
    PLAY_ROUND_FINISH,
    PLAYER_COLD,
    ROUND_OVER,
    GAME_OVER
};

#endif // EVENT_H

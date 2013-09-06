#ifndef DATA_H
#define DATA_H

#include <string>
#include <deque>
#include <vector>

namespace pictomir2course {

typedef struct {
    size_t width;
    size_t height;
}
size2d_t;

typedef struct {
    unsigned int x;
    unsigned int y;
}
point_t;

typedef struct {
    point_t first;
    point_t second;
}
wall_t;

typedef enum {
    SOUTH,
    WEST,
    NORTH,
    EAST
}
direction_t;

typedef struct {
    point_t position;
    direction_t direction;
}
actor_t;

typedef std::deque<point_t>
point_list_t;

typedef std::deque<wall_t>
wall_list_t;

typedef struct {
    size2d_t size;
    actor_t actor;
    point_list_t painted;
    point_list_t pointed;
    wall_list_t walls;
}
environment_t;

typedef enum {
    NO_HINT, TEXT_HINT, IMAGE_HINT
}
hint_type_t;

typedef struct {
    hint_type_t hint_type;
    std::vector<char> data;
}
hint_t;

typedef enum {
    NO_COMMAND,
    TURN_LEFT,
    TURN_RIGHT,
    GO_FORWARD,
    DO_ACTION,
    CALL_1,
    CALL_2,
    CALL_3,
    CALL_4,
    CALL_5,
    CALL_6,
    COND_WAS_ACTION,
    COND_NOT_ACTION,
    COND_IS_WALL,
    COND_NOT_WALL,
    REP_FOREVER,
    REP_2,
    REP_3,
    REP_4,
    REP_5,
    REP_6
}
command_t;

typedef struct {
    command_t repeater;
    command_t condition;
    std::vector<command_t> commands;
    std::string comment;
}
algorithm_t;

typedef std::deque<algorithm_t>
program_t;

typedef struct {
    std::string title;
    environment_t environment;
    program_t program;
    hint_t hint;
}
task_t;

typedef struct {
    std::string title;
    std::deque<std::string> authors;
    std::string copyright;
    std::string license;
    std::string homepage;
    std::deque<task_t> tasks;
}
game_t;

}

#endif

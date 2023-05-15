#ifndef GAME_TYPES
#define GAME_TYPES

#include "engine_types.h"

#define INVENTORY_SLOTS 15

// Mobs

typedef enum {
    STUPID = 0,
    COWARD = 1,
    INTELLIGENT = 2
} MobBehavior;

typedef struct {
    Vector2D position;
    Vector2D targetPosition;
    int health;
    MobBehavior mobBehavior;
    unsigned long timeSinceLastUpdate; // Em microsegundos
    int chasingPlayer;
    int pathStep;
    Vector2D *path;
} Mob;

// Mapa
typedef struct map {
	int object;  //0:local onde pode andar|1:parede|2:passagem de nível|3:vazio|4:lava|5:relva|6:flor|7:água|8:margem|9:baú|10:porta|11:chave|12:chão sala fechada
} MAP;

// Mundo de Mapas
typedef struct world {
	MAP** map;
    Mob *mobs;
    int mobQuantity;
    int created; // valida se um nível já foi gerado ou não
} World;

// Items

typedef enum {
    NONE = 0,
    MELEE_WEAPON = 1,
    RANGED_WEAPON = 2,
    BOMB = 3,
    MISCELLANEOUS = 4,
    SPECIAL = 5
} ItemType;

typedef struct {
    char name[100];
    ItemType type;
    char description[400];
    char sprite[200];
} Item;

typedef struct {
    int size;
    ItemType type;
    Item items[10];
} ItemsTable;

/*typedef struct {
    Item items[5];
    Vector2D position;
} Chest;*/

typedef struct {
    Item items[INVENTORY_SLOTS];
} Inventory;

// Jogador

typedef struct {
    char name[15];
    int health;
    Vector2D position;
    int gold;
    Inventory inventory;
    int selectedSlot;
    int speedMultiplier;
    unsigned long timeSinceDrownStart; 
} Player;

typedef struct {
    Vector2D pathPos;
    Vector2D *path;
    int pathStep;
    int moving;
    unsigned long timeSinceLastUpdate; // Em microsegundos
} PathBehaviour;

// Estado do jogo

typedef struct {
    Player player;
    int gameOver; // 1 - perdeu o jogo
    int paused;
    int pathSelection;
    PathBehaviour pathState;
} GameState;

#endif
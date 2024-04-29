#include <string>
#include <cstring>
#include <SFML/Graphics.hpp>

class Player
{
public:
    float x, y;
    int animation_frame = 0;
    sf::Sprite sprite;
    int moving = 0;
    int jumping = 0;
    int left = 0;
    int last_direction = 0;
    int width = 0;
    int height = 0;
    sf::Texture player_texture_walking;
    sf::Texture player_texture_idle;
    // Constructor
    Player(sf::Texture *player_texture_idlee, sf::Texture *player_texture_walkinge, int sX, int sY);

    // Destructor
    ~Player();
    void Die();

private:
};

// Constructor implementation
Player::Player(sf::Texture *player_texture_idlee, sf::Texture *player_texture_walkinge, int sX, int sY)
{
    player_texture_idle = *player_texture_idlee;
    player_texture_walking = *player_texture_walkinge;

    x = sX;
    y = sY;
    animation_frame = 0;
    sprite.setTexture(player_texture_idle);
    sprite.setPosition(sX, sY);
    sprite.setTextureRect(sf::IntRect(0, 0, 24, 32));
    sprite.setScale(3, 3);
    width = 24 * 3;
    height = 32 * 3;
}

// Destructor implementation
Player::~Player()
{
    // Perform any necessary cleanup here
}

class Ground
{
public:
    float x, y;
    sf::Sprite sprite;
    int width = 64;
    int height = 64;
    // Constructor
    Ground(int x, int y, sf::Texture *ground_texturee);
    void set_position(float x, float y);
    // Destructor
    ~Ground();

private:
};

// Constructor implementation
Ground::Ground(int x, int y, sf::Texture *ground_texturee)
{
    sprite.setTexture(*ground_texturee);
    sprite.setPosition(x, y);
    sprite.setTextureRect(sf::IntRect(336, 367, 32, 32));
    sprite.setScale(2, 2);
}

void Ground::set_position(float xx, float yy)
{
    x = xx;
    y = yy;
    sprite.setPosition(x, y);
}

// Destructor implementation
Ground::~Ground()
{
    // Perform any necessary cleanup here
}

class Enemy
{
public:
    float x, y;
    int animation_frame = 0;
    sf::Sprite sprite;
    int moving = 0;
    int left = 0;
    int width = 0;
    int height = 0;
    float walk_left_right = 0;
    float offset = 0;
    // Constructor
    Enemy(sf::Texture *enemy_texture_walkinge, float x, float y);

    // Destructor
    ~Enemy();

private:
};

// Constructor implementation

Enemy::Enemy(sf::Texture *enemy_texture_walkinge, float xx, float yy)
{
    x = xx;
    y = yy;
    animation_frame = 0;
    sprite.setTexture(*enemy_texture_walkinge);
    sprite.setPosition(x, y);
    sprite.setTextureRect(sf::IntRect(10, 24, 18, 8));
    sprite.setScale(4, 4);
    width = 18 * 4;
    height = 8 * 4;
}

// Destructor implementation
Enemy::~Enemy()
{
    // Perform any necessary cleanup here
}

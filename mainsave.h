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
    int atacking = 0;
    float time_to_atack_again = 0;
    int start_pos_x = 0;
    int start_pos_y = 0;
    sf::Texture player_texture_walking;
    sf::Texture player_texture_idle;
    sf::Texture player_texture_atack;
    // Constructor
    Player(sf::Texture *player_texture_idlee, sf::Texture *player_texture_walkinge, sf::Texture *player_texture_atacke, int sX, int sY);

    // Destructor
    ~Player();
    void Die();

private:
};

// Constructor implementation
Player::Player(sf::Texture *player_texture_idlee, sf::Texture *player_texture_walkinge, sf::Texture *player_texture_atacke, int sX, int sY)
{
    player_texture_idle = *player_texture_idlee;
    player_texture_walking = *player_texture_walkinge;
    player_texture_atack = *player_texture_atacke;
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
    int width = 48;
    int height = 48;
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
    sprite.setScale(1.5, 1.5);
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
    bool dead = false;
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

class Star
{
public:
    float x, y;
    sf::Sprite sprite;
    int width = 64;
    int height = 64;
    int animation_frame = 0;
    // Constructor
    Star(int x, int y, sf::Texture *star_texturee);
    void set_position(float x, float y);
    // Destructor
    ~Star();

private:
};

// Constructor implementation
Star::Star(int x, int y, sf::Texture *star_texturee)
{
    sprite.setTexture(*star_texturee);
    sprite.setPosition(x, y);
    sprite.setTextureRect(sf::IntRect(0, 0, 32, 32));
    sprite.setScale(2, 2);
}

void Star::set_position(float xx, float yy)
{
    x = xx;
    y = yy;
    sprite.setPosition(x, y);
}

// Destructor implementation
Star::~Star()
{
    // Perform any necessary cleanup here
}
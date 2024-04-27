#include <string>
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

    int width = 0;
    int height = 0;
    sf::Texture player_texture_walking;
    sf::Texture player_texture_idle;
    // Constructor
    Player(sf::Texture *player_texture_idlee, sf::Texture *player_texture_walkinge);

    // Destructor
    ~Player();

private:
};

// Constructor implementation
Player::Player(sf::Texture *player_texture_idlee, sf::Texture *player_texture_walkinge)
{
    x = 500;
    y = 500;
    animation_frame = 0;
    sprite.setTexture(*player_texture_idlee);
    sprite.setPosition(800, 800);
    sprite.setTextureRect(sf::IntRect(0, 0, 24, 32));
    sprite.setScale(3, 3);
    width = 24 * 3;
    height = 32 * 3;
    player_texture_idle = *player_texture_idlee;
    player_texture_walking = *player_texture_walkinge;
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
    sf::Texture ground_texture;
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
    sf::Texture enemy_texture_walking;
    // Constructor
    Enemy(sf::Texture *enemy_texture_walkinge);

    // Destructor
    ~Enemy();

private:
};

// Constructor implementation

Enemy::Enemy(sf::Texture *enemy_texture_walkinge)
{
    x = 500;
    y = 500;
    animation_frame = 0;
    sprite.setTexture(*enemy_texture_walkinge);
    sprite.setPosition(800, 800);
    sprite.setTextureRect(sf::IntRect(0, 0, 24, 32));
    sprite.setScale(3, 3);
    width = 24 * 3;
    height = 32 * 3;
    enemy_texture_walking = *enemy_texture_walkinge;
}

// Destructor implementation
Enemy::~Enemy()
{
    // Perform any necessary cleanup here
}
